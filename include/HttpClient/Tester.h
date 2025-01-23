#ifndef TESTER_H
#define TESTER_H


#include <iostream>
#include <boost/asio.hpp>
#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"



struct Tester{
  boost::asio::any_io_executor _executer;

  explicit Tester(boost::asio::io_context & ioContext) : _executer(ioContext.get_executor()){

  }

  void operator()(){

    HttpClientParameters paramters;
    paramters._executor = _executer;

    auto httpClient = HttpClient::Make(std::move(paramters));

    HttpRequest request;
    httpClient->SendAsync(request,
                          [](std::error_code err, HttpResponse response){
                          if(err){
                            std::cout << "error occured. Error message: " << err.message() << std::endl;
                            return;
                          }
                          std::cout << "Request succeeded. " << std::endl
                                    << "Response code : " << response.StatusCode << std::endl
                                    << "Response body : " << response.Body << std::endl
                                    << std::endl;

                          }
    );
  }
};

#endif
