#ifndef HTTPCLIENT_T
#define HTTPCLIENT_T

#include "ConnectionParameter.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <HttpClientParamters.h>
#include <boost/asio.hpp>
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

    _resolver.async_resolve(
        connectionParameter.host, connectionParameter.port,
        [f = std::move(callable), sharedThis = this->shared_from_this(),
         this](const boost::system::error_code &err,
               const boost::asio::ip::tcp::resolver::results_type
                   &endpoints) mutable {
          if (err) {
            std::cout << "err occurred: " << err.message() << std::endl;
            f(err);
            DeferDelection(); // keeping the shared pointer alive
            return;
          }

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
          // std::cout << "successfully resolved " << std::endl;
          // f(std::error_code());
        });
  }

  template <typename Callable>
  void SendAsync(const HttpRequest &request, Callable callable) {
    boost::asio::post(_httpClientParameters._executor,
                      [this, f = std::move(callable),
                       sharedThis = this->shared_from_this()]() mutable {
                        HttpResponse response;
                        response.Body = "glad to know you";
                        f(std::error_code(), std::move(response));
                        DeferDelection();
                      });
  }

private:
  explicit HttpClient(HttpClientParameters httpClientParameters)
      : _httpClientParameters(std::move(httpClientParameters)),
        _resolver(_httpClientParameters._executor),
        _socket(_httpClientParameters._executor) {}

  HttpClientParameters _httpClientParameters;
  boost::asio::ip::tcp::resolver _resolver;
  boost::asio::ip::tcp::socket _socket;

  void DeferDelection() {
    boost::asio::post(_httpClientParameters._executor,
                      [sharedThis = this->shared_from_this()]() mutable {
                        sharedThis.reset();
                      });
  }
};

#endif
