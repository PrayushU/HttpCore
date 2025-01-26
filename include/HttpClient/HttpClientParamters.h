#ifndef HTTPPARAM_T
#define HTTPPARAM_T

#include <boost/asio.hpp>
#include <boost/asio/ssl/context.hpp>

struct HttpClientParameters {
  boost::asio::any_io_executor _executor;
  std::shared_ptr<boost::asio::ssl::context> _sslContext;
};

#endif
