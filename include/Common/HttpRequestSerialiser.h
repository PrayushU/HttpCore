#ifndef HTTP_REQ_SERIAL_H
#define HTTP_REQ_SERIAL_H

#include "HttpRequest.h"
#include <sstream>
#include <string>
#include <iostream>

struct HttpRequestSerialiser {
  HttpRequestSerialiser(const HttpRequest &httpRequest) : _httpRequest(httpRequest) {}

  std::string operator()() {

    // GET /get HTTP/1.1
    // Host: httpbin.org
    std::stringstream ss;
    ss << to_string(_httpRequest._httpverb) << " " << _httpRequest._path << " " << "HTTP/1.1\r\n"
       << "Host: " << _httpRequest._host << "\r\n\r\n";
    std::cout << "[DEBUG]: "<<std::endl << ss.str() << std::endl;
    return ss.str();
  }

private:
  const HttpRequest &_httpRequest;
};

#endif
