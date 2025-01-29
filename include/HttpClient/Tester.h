#ifndef TESTER_H
#define TESTER_H

#include "ConnectionParameter.h"
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "RequestGenerators.h"
#include "openssl/ssl.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <iostream>
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

    // std::string link = "https://httpbin.org/get";
    std::string link = "https://www.google.com/";

    ConnectionParameter connectionParameter = make_connection_parameter(link);
    // std::cout <<"[DEBUG] useTls: " << connectionParameter.useTls <<
    // std::endl;

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
                  std::cout << "Error occured. Error message: " << err.message()
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

  static std::shared_ptr<boost::asio::ssl::context> PrepareSSLContext() {
    using boost::asio::ssl::context;

    auto sslContext = std::make_unique<context>(context::tlsv12);

    try {
      sslContext->load_verify_file("../certs/cacert.pem");
    } catch (const std::exception &ex) {
      std::cerr << "[ERROR] Failed to load certificate: " << ex.what() << '\n';
      throw; // Or handle gracefully
    }

    sslContext->set_verify_mode(context::verify_peer);

    sslContext->set_verify_callback(
        [](bool preVerified, boost::asio::ssl::verify_context &verifyContext) {
          if (!preVerified) {
            // Inspect verification details if pre-verification fails
            char subjectName[256];
            X509 *cert =
                X509_STORE_CTX_get_current_cert(verifyContext.native_handle());
            X509_NAME_oneline(X509_get_subject_name(cert), subjectName,
                              sizeof(subjectName));
            std::cerr
                << "[WARNING] Certificate verification failed for subject: "
                << subjectName << '\n';
          }
          return preVerified;
        });

    // Suggesting boost what and what not cipher to use
    // However, some server use these and this will prohibt to connect to those servers
    const char *defaultCipherList = "HIGH:!ADH:!MD5:!RC4:!SRP:!PSK:!DSS";
    auto ret =
        SSL_CTX_set_cipher_list(sslContext->native_handle(), defaultCipherList);
    if (ret) {
      std::cerr << "[ERROR]: Error in setting cipher list. Set cipher list: "
                << ret << std::endl;
    }

    // Custom options for boost.asio and openssl. What to use or not to use
    auto options = context::default_workarounds | context::no_sslv2 |
                   context::no_sslv3 | context::no_tlsv1 | context::no_tlsv1_1;
    sslContext->set_options(options);

    return sslContext;
  }
};
#endif
