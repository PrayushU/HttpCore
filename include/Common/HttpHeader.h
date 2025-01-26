#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <algorithm>
#include <string>
#include <vector>

struct StartLine {
  std::vector<std::string> parts;
};

struct HeaderField {
  std::string Name;
  std::string Value;
};

struct HttpHeader {
  StartLine StartLine;
  void Add(HeaderField field) { Headers.push_back(std::move(field)); }
  HeaderField *Get(const std::string &name) {
    auto loc = std::find_if(
        std::begin(Headers), std::end(Headers),
        [&name](const HeaderField &field) { return field.Name == name; });
    return std::end(Headers) == loc ? nullptr : &(*loc);
  }
  std::vector<HeaderField> Headers;
};

#endif
