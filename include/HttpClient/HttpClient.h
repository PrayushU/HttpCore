#ifndef HTTPCLIENT_T
#define HTTPCLIENT_T

#include "ConnectionParameter.h"
#include "HttpRequest.h"
#include "HttpRequestSerialiser.h"
#include "HttpResponse.h"
#include "HttpResponsePopulator.h"
#include "HttpResponseStreamParser.h"
#include <HttpClientParamters.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <memory>
#include <system_error>

struct HttpClient : std::enable_shared_from_this<HttpClient> {

  static std::shared_ptr<HttpClient>
  Make(HttpClientParameters httpClientParamters) {
    return std::shared_ptr<HttpClient>(
        new HttpClient(std::move(httpClientParamters)));
  }

  template <typename Callable>
  void ConnectAsync(ConnectionParameter connectionParameter,
                    Callable callable) {

    _useTls = connectionParameter.useTls;
    _resolver.async_resolve(
        connectionParameter.host, connectionParameter.port,
        [f = std::move(callable), sharedThis = this->shared_from_this(), this,
         host = connectionParameter.host](
            const boost::system::error_code &err,
            const boost::asio::ip::tcp::resolver::results_type
                &endpoints) mutable {
          if (err) {
            std::cout << "err occurred: " << err.message() << std::endl;
            f(err);
            DeferDelection(); // keeping the shared pointer alive
            return;
          }

          if (_useTls) {
            if (!SSL_set_tlsext_host_name(_tlsSocket->native_handle(),
                                          host.c_str())) {
              f(std::make_error_code(
                  static_cast<std::errc>(static_cast<int>(::ERR_get_error()))));
              return;
            }

            boost::asio::async_connect(
                _tlsSocket->lowest_layer(), endpoints,
                [f2 = std::move(f), sharedThis = std::move(sharedThis),
                 this](boost::system::error_code err,
                       boost::asio::ip::tcp::endpoint) {
                  if (err) {
                    std::cout << "Connect error occurred : " << err.message()
                              << std::endl;
                    f2(err);
                    DeferDelection();
                    return;
                  }

                  _tlsSocket->async_handshake(
                      boost::asio::ssl::stream_base::client,
                      [this, sharedThis = std::move(sharedThis),
                       f3 = std::move(f2)](boost::system::error_code err) {
                        f3(err);
                        DeferDelection();
                      });
                });
          } else {
            boost::asio::async_connect(
                _socket, endpoints,
                [f2 = std::move(f), sharedThis = std::move(sharedThis),
                 this](boost::system::error_code err,
                       boost::asio::ip::tcp::endpoint) {
                  if (err) {
                    std::cout << "Connect error occurred : " << err.message()
                              << std::endl;
                    f2(err);
                    DeferDelection();
                    return;
                  }
                  f2(std::error_code());
                  DeferDelection();
                });
          }
        });
  }

  template <typename Callable>
  void SendAsync(const HttpRequest &httpRequest, Callable callable) {
    ReadResponseAsync(std::move(callable));
    SendMessageAsync(HttpRequestSerialiser{httpRequest}());
  }

private:
  explicit HttpClient(HttpClientParameters httpClientParameters)
      : _httpClientParameters(std::move(httpClientParameters)),
        _resolver(_httpClientParameters._executor),
        _socket(_httpClientParameters._executor),
        _httpResponseStreamParser(&_httpResponsePopulator) {

    if (_httpClientParameters._sslContext) {
      using namespace boost::asio;
      _tlsSocket = std::make_shared<ssl::stream<ip::tcp::socket>>(
          _httpClientParameters._executor, *_httpClientParameters._sslContext);
    }
  }

  HttpClientParameters _httpClientParameters;
  boost::asio::ip::tcp::resolver _resolver;
  boost::asio::ip::tcp::socket _socket;
  std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
      _tlsSocket;

  boost::asio::streambuf _request;
  boost::asio::streambuf _response;

  HttpResponsePopulator _httpResponsePopulator;
  HttpResponseStreamParser _httpResponseStreamParser;

  bool _useTls = false;

  void DeferDelection() {
    boost::asio::post(_httpClientParameters._executor,
                      [sharedThis = this->shared_from_this()]() mutable {
                        sharedThis.reset();
                      });
  }

  template <typename Callable> void ReadResponseAsync(Callable callable) {

    if (_useTls) {
      if (_tlsSocket && _tlsSocket->lowest_layer().is_open()) {
        ReadResponseAsyncT(*_tlsSocket, std::move(callable));
      } else {
        std::cerr << "Error: TLS socket is not open for reading." << std::endl;
        callable(boost::system::error_code(boost::asio::error::not_connected,
                                           boost::system::system_category()),
                 HttpResponse{});
      }
    } else {
      if (_socket.is_open()) {
        ReadResponseAsyncT(_socket, std::move(callable));
      } else {
        std::cerr << "Error: Socket is not open for reading." << std::endl;
        callable(boost::system::error_code(boost::asio::error::not_connected,
                                           boost::system::system_category()),
                 HttpResponse{});
      }
    }
  }

  template <typename Callable, typename SocketType>
  void ReadResponseAsyncT(SocketType &socket, Callable callable) {
    boost::asio::async_read_until(
        socket, _response, _httpResponseStreamParser,
        [this, sharedThis = shared_from_this(), f = std::move(callable)](
            boost::system::error_code err, std::size_t size) mutable {
          if (err) {
            std::cerr << "Error: Read error occurred: " << err.message()
                      << std::endl;
            f(err, HttpResponse{});
            return;
          }

          std::cout << "Message read successfully, size: " << size << " bytes."
                    << std::endl;

          auto response = _httpResponsePopulator.TheResponse();

          f(std::error_code(), std::move(response));

          DeferDelection();
        });
  }

  void SendMessageAsync(const std::string &content) {
    if (_useTls) {
      if (_tlsSocket && _tlsSocket->lowest_layer().is_open()) {
        SendMessageAsyncT(*_tlsSocket, content);
      } else {
        std::cerr << "Error: TLS socket is not open." << std::endl;
      }
    } else {
      if (_socket.is_open()) {
        SendMessageAsyncT(_socket, content);
      } else {
        std::cerr << "Error: Socket is not open." << std::endl;
      }
    }
  }

  template <typename SocketType>
  void SendMessageAsyncT(SocketType &socket, const std::string &content) {
    auto buffer = std::make_shared<std::string>(content); // Use separate buffer
    boost::asio::async_write(
        socket, boost::asio::buffer(*buffer),
        [sharedThis = shared_from_this(), buffer](boost::system::error_code err,
                                                  std::size_t size) {
          if (err) {
            std::cerr << "Send error occurred: " << err.message()
                      << " (code: " << err.value() << ")" << std::endl;
            return;
          }
          std::cout << "Message sent successfully, size: " << size << " bytes."
                    << std::endl;
        });
  }
};

#endif
