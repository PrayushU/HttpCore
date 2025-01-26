#ifndef TESTER_H
#define TESTER_H

#include "ConnectionParameter.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "RequestGenerators.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <system_error>
// #include <boost/beast/ssl.hpp>

struct Tester {

  boost::asio::any_io_executor _executer;

  explicit Tester(boost::asio::io_context &ioContext)
      : _executer(ioContext.get_executor()) {}

  void operator()() {

    HttpClientParameters paramters;
    paramters._executor = _executer;
    paramters._sslContext = PrepareSSLContext();

    auto httpClient = HttpClient::Make(std::move(paramters));

    std::string link = "https://httpbin.org/get";

    ConnectionParameter connectionParameter = make_connection_parameter(link);
    // std::cout <<"[DEBUG] useTls: " << connectionParameter.useTls << std::endl;

    httpClient->ConnectAsync(
        std::move(connectionParameter),
        [httpClient, link](std::error_code err) {
          if (err) {
            std::cout << "ConnectAsync failed: " << err.message() << std::endl;
            return;
          }
          std::cout << "ConnectAsync succeeded" << std::endl;

          HttpRequest request = make_get_request(link);
          httpClient->SendAsync(
              request, [](std::error_code err, HttpResponse response) {
                if (err) {
                  std::cout << "error occured. Error message: " << err.message()
                            << std::endl;
                  return;
                }

                std::cout << "Request succeeded:" << std::endl << std::endl;

                print(response);
              });
        });
  }

  static void print(HttpResponse &response) {

    for (auto &startLinePart : response.Header.StartLine.parts) {
      std::cout << startLinePart << " ";
    }
    std::cout << std::endl;

    for (auto &header : response.Header.Headers) {
      std::cout << header.Name << ": " << header.Value << std::endl;
    }

    std::cout << std::endl << response.Body;
  }

  std::shared_ptr<boost::asio::ssl::context> PrepareSSLContext() {
    using namespace boost::asio;

    auto sslContext =
        std::make_shared<ssl::context>(boost::asio::ssl::context::tlsv12);
    // sslContext->set_verify_mode(ssl::verify_peer);
    sslContext->set_verify_mode(ssl::verify_none);

    sslContext->set_verify_callback(
        [](bool preVerified, ssl::verify_context &verifyContext) {
          // return preVerified;
          return true;
        });
    return sslContext;
  }
};

#endif
