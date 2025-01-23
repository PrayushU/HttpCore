#pragma once
#include <boost/asio.hpp>
#include "HttpClient.h"

struct Tester{
  boost::asio::any_io_executor _executer;

  explicit Tester(boost::asio::io_context & ioContext) : _executer(ioContext.get_executor()){

  }

  void operator()(){

    HttpClientParameters paramters;
    paramters._executor = _executer;

    auto httpClient = HttpClient::Make(std::move(paramters));
  }
};
