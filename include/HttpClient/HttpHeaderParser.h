#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "HttpHeader.h"
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>

struct HttpHeaderParser {

  HttpHeader operator()(const std::string data) {

    std::vector<std::string> lines;
    boost::split(lines, data, boost::is_any_of("\r\n"));
    if (lines.empty()) {
      return HttpHeader{};
    }

    auto &startLine = lines[0];
    HttpHeader httpHeader;
    boost::split(httpHeader.StartLine.parts, startLine, boost::is_any_of(" "));
    lines.erase(lines.begin());

    for (auto &line : lines) {
    }
    // httpHeader.StartLine.parts
    return httpHeader;
  }
  // HttpHeader ReadHeader(std::string headerData){
  //
  // }
};

#endif
