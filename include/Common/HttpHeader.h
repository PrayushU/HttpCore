#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <vector>

struct StartLine {
  std::vector<std::string> parts;
};

struct HttpHeader {
  StartLine StartLine;
};

#endif
