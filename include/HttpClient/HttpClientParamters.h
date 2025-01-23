#include <boost/asio.hpp>

struct HttpClientParameters{
  boost::asio::any_io_executor _executor;
};
