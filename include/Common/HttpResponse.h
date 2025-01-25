#ifndef HTTPRESP_T
#define HTTPRESP_T

#include "HttpHeader.h"
#include <string>

struct HttpResponse {
  int StatusCode{400};
  std::string Body{};
  HttpHeader Header;
};

#endif
