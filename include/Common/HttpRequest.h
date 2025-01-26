#ifndef HTTPREQ_H
#define HTTPREQ_H

#include <string>

enum class HttpVerb { GET, POST };

inline std::string to_string(HttpVerb httpVerb) {
  switch (httpVerb) {
  case HttpVerb::GET:
    return "GET";
  case HttpVerb::POST:
    return "POST";
  default:
    return "";
  }
}
struct HttpRequest {
  HttpVerb _httpverb;
  std::string _path;
  std::string _host;
};

#endif
