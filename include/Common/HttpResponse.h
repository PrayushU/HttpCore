#ifndef HTTPRESP_T
#define HTTPRESP_T

#include <string>

struct HttpResponse {
  int StatusCode = 400;
  std::string Body = {};
};

#endif
