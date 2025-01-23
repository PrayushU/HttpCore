#ifndef TESTER_H
#define TESTER_H

#include "ConnectionParameter.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <boost/asio.hpp>
#include <iostream>
#include <system_error>

struct Tester {
  boost::asio::any_io_executor _executer;

  explicit Tester(boost::asio::io_context &ioContext)
      : _executer(ioContext.get_executor()) {}

  void operator()() {

    HttpClientParameters paramters;
    paramters._executor = _executer;

    auto httpClient = HttpClient::Make(std::move(paramters));

    ConnectionParameter connectionParameter;
    connectionParameter.host = "google.com";
    connectionParameter.port = "80";
    httpClient->ConnectAsync(
        std::move(connectionParameter), [httpClient](std::error_code err) {
          if (err) {
            std::cout << "ConnectAsync failed" << std::endl;
            return;
          }
          std::cout << "ConnectAsync succeeded" << std::endl;

          //Chaining, so sendAsync after ConnectAsync
          HttpRequest request;
          httpClient->SendAsync(request, [](std::error_code err,
                                            HttpResponse response) {
            if (err) {
              std::cout << "error occured. Error message: " << err.message()
                        << std::endl;
              return;
            }

            std::cout << "Request succeeded. " << std::endl
                      << "Response code : " << response.StatusCode << std::endl
                      << "Response body : " << response.Body << std::endl
                      << std::endl;
          });
        });

    // std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
};

#endif
