#ifndef HTTP_POPU
#define HTTP_POPU

#include "HttpHeader.h"
#include "HttpHeaderParser.h"
#include "HttpResponse.h"
#include <boost/asio.hpp>
#include <iostream>
#include <iterator>
#include <utility>

enum class ParserState {

  waiting_for_headers,
  waiting_for_body,
  done

};

enum class TransferMethod { content_length, chunked, none };

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
  std::string _doubleNewLines = "\r\n\r\n";
  HttpResponse _httpResponse;
  TransferMethod _transferMethod = TransferMethod::none;
  int _expectedMessageSize{};
  int _beginningOfBodyIndex{};

  HttpResponse &TheResponse() { return _httpResponse; }

  // Funcor operator: defines the match condition
  template <typename Iterator>
  std::pair<Iterator, bool> operator()(Iterator beginItr, Iterator endItr) {
    if (ParserState::done == _state) {
      return std::make_pair(beginItr, done);
    }
    _buffer.insert(_buffer.end(), beginItr, endItr);

    if (ParserState::waiting_for_headers == _state) {
      auto loc =
          std::search(begin(_buffer), std::end(_buffer),
                      std::begin(_doubleNewLines), std::end(_doubleNewLines));
      if (std::end(_buffer) == loc) {
        return std::make_pair(endItr, notDone);
      }

      // This will get us the full header
      std::string headerData(std::begin(_buffer), loc);

      // TODO: add error checks in the parse
      auto possibleHeader = HttpHeaderParser{}(headerData);

      if (!possibleHeader.first) {
        _state = ParserState::done;
        return std::make_pair(endItr, done);
      }
      _state = ParserState::waiting_for_body;

      _httpResponse.Header = std::move(possibleHeader.second);
      _transferMethod = DetermineTransferMethod(_httpResponse.Header);
      if (TransferMethod::none == _transferMethod) {
        return std::make_pair(endItr, done);
      }

      if (TransferMethod::chunked == _transferMethod) {
        // TODO: Support in future
        _state = ParserState::done;
        return std::make_pair(endItr, done);
      }

      _state = ParserState::waiting_for_body;
      int bodySize;
      try {
        bodySize = std::stoi(
            _httpResponse.Header.Get("Content-Length")
                ->Value); // Content length value, the size of the rqst
      } catch (const std::exception &) {
        _state = ParserState::done;
        return std::make_pair(endItr, done);
      }

      _expectedMessageSize =
          headerData.size() + bodySize + _doubleNewLines.size();
      _beginningOfBodyIndex = headerData.size() + _doubleNewLines.size();
      if (_expectedMessageSize > _buffer.size()) {
        return std::make_pair(endItr, notDone);
      }

      _beginningOfBodyIndex = headerData.size() + _doubleNewLines.size();
      auto beginningOfBody = std::begin(_buffer) + _beginningOfBodyIndex;
      auto endOfBody = std::begin(_buffer) + _expectedMessageSize;

      _state = ParserState::done;
      _httpResponse.Body = std::string(beginningOfBody, endOfBody);
      return std::make_pair(endItr, done);
    } else if (ParserState::waiting_for_body == _state) {
      if (_expectedMessageSize > _buffer.size()) {
        return std::make_pair(endItr, notDone);
      }
      _state = ParserState::done;
      auto beginningOfBody = std::begin(_buffer) + _beginningOfBodyIndex;
      auto endOfBody = std::begin(_buffer) + _expectedMessageSize;
      _httpResponse.Body = std::string(beginningOfBody, endOfBody);
      return std::make_pair(endItr, done);
    }
    // Expecting to never reach here
    return std::make_pair(beginItr, done);
  }
  TransferMethod DetermineTransferMethod(HttpHeader &header) {
    auto encoding = header.Get("Transfer-Encoding");
    if (encoding && encoding->Value == "chunked") {
      return TransferMethod::chunked;
    }
    auto contentLength = header.Get("Content-Length");
    if (contentLength && !contentLength->Value.empty()) {

      return TransferMethod::content_length;
    }
    return TransferMethod::none;
  }
};

#endif // HTTP_POPU
