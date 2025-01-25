#ifndef HTTP_POPU
#define HTTP_POPU

#include "HttpHeader.h"
#include <boost/asio.hpp>
#include "HttpHeader.h"
#include "HttpHeaderParser.h"
#include <iterator>
#include <utility>

enum class ParserState {

  waiting_for_headers,
  done

};

struct HttpResponsePopulator {
  // Deifne an iterator type for Boost.Asio's streambuf
  typedef boost::asio::buffers_iterator<
      boost::asio::streambuf::const_buffers_type>
      Iterator;

  // Const to indicate whether the match condition is satisfied
  const bool done = true;
  const bool notDone = false;
  ParserState _state = ParserState::waiting_for_headers;
  std::string _buffer;
  std::string _doubleEmptyLines = "\r\n\r\n";
  HttpHeader _httpHeader;

  const HttpHeader &ReadHeaderData() { return _httpHeader; }

  // Funcor operator: defines the match condition
  template <typename Iterator>
  std::pair<Iterator, bool> operator()(Iterator beginItr, Iterator endItr) {
    if (ParserState::done == _state) {
      return std::make_pair(beginItr, done);
    }
    _buffer.insert(_buffer.end(), beginItr, endItr);

    if (ParserState::waiting_for_headers == _state) {
      auto loc = std::search(begin(_buffer), std::end(_buffer),
                             std::begin(_doubleEmptyLines),
                             std::end(_doubleEmptyLines));
      if (std::end(_buffer) == loc) {
        return std::make_pair(endItr, notDone);
      }

      // This will get us the full header
      std::string headerData(std::begin(_buffer), loc);

      _httpHeader = HttpHeaderParser{}(headerData);
      // _headerData = headerData;
      // ReadHeader(std::move(headerData));
      return std::make_pair(endItr, done);
    }

    // Expecting to never reach here
    return std::make_pair(beginItr, done);
  }
};

#endif // HTTP_POPU
