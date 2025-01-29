
#ifndef STRESS_TEST_H
#define STRESS_TEST_H

#include "Tester.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>
#include <iostream>
#include <system_error>
// #include <boost/beast/ssl.hpp>

struct StressTester {

  boost::asio::any_io_executor _executer;

  explicit StressTester(boost::asio::io_context &ioContext)
      : _executer(ioContext.get_executor()) {}

  void operator()() {}
};
#endif

