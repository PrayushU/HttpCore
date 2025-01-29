#include "HttpRequest.h"
#include "RequestGenerators.h"
#define CATCH_MAIN_CONFIG
#include <boost/asio.hpp>
#include "catch_amalgamated.hpp"
#include "HttpClient/ConnectionParameter.h"
#include "Common/RequestGenerators.h"

TEST_CASE("Http Connection Paramters Parse"){
  auto parameter1 = make_connection_parameter("http://whatever.com/getrequest/get");
  CHECK(parameter1.host == "whatever.com");
  CHECK(parameter1.port == "80");


  auto parameter2 = make_connection_parameter("http://whatever.com:9991/getrequest/path");
  CHECK(parameter2.port == "9991");

  auto parameter3 = make_connection_parameter("https://whatever.com/getrequest/get");
  CHECK(parameter3.port == "443");
}


TEST_CASE("Get Request Generated Parsing"){
  auto getRequest1 = make_get_request("http://httpbin.org/get");
  CHECK(getRequest1._host == "httpbin.org");
  CHECK(getRequest1._httpverb== HttpVerb::GET);
  CHECK(getRequest1._path == "/get");
}



