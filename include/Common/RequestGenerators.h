#ifndef REQUEST_GEN_H
#define REQUEST_GEN_H

#include <boost/url.hpp>
#include "HttpRequest.h"

static HttpRequest make_get_request(std::string link){

  auto parsedLink = boost::urls::parse_uri(link);


  HttpRequest request;
  request._host = parsedLink->host_address();
  request._httpverb = HttpVerb::GET;
  request._path = parsedLink->path();

  return request;

}


#endif 
