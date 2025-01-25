#ifndef HTTP_RES_STREAM_PARS
#define HTTP_RES_STREAM_PARS
#include <boost/asio.hpp>
#include <utility>
#include "HttpResponsePopulator.h"

struct HttpResponseStreamParser {

  // Constructor: takes a pointer to an HttpresponsePopulator
  HttpResponseStreamParser(HttpResponsePopulator * populator) : _populator(populator){}

  // Define an iterator type for Boost.Asio's strambuf
  typedef boost::asio::buffers_iterator<
      boost::asio::streambuf::const_buffers_type>
      Iterator;

  //Functor operator: delegates to the HttpresponsePoulator
  template <typename Iterator>
  std::pair<Iterator, bool> operator()(Iterator beginItr, Iterator endItr) {

    // Call the HttpResponsePopulator's operator()
    return (*_populator)(beginItr, endItr);
  }


  // Pointer to the HttpResponsePopulator
  HttpResponsePopulator * _populator = nullptr;
};

// Specialise the Boost.Asio's is_match_condition trait for HttpResponseStreamParser
namespace boost {
namespace asio {
template <>
struct is_match_condition<HttpResponseStreamParser> : public boost::true_type {
};
} // namespace asio
} // namespace boost

#endif // HTTP_RES_STREAM_PARS
