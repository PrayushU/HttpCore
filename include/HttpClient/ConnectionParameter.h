#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/url.hpp>
#include <string>
#include <algorithm>
#include <vector>

struct ConnectionParameter {
  std::string host = {};
  std::string port = {};
};

inline std::string GuessPortForScheme(auto& parsedLink){

  auto scheme = parsedLink->scheme();
  static std::vector<std::pair<std::string, std::string>> mappings = {
    {"http", "80"},
    {"https", "443"},
    {"ssl","443"}
  };

  auto loc = std::find_if(std::begin(mappings), std::end(mappings), [&scheme](std::pair<std::string, std::string>& item){return item.first==scheme;});

  if(std::end(mappings) != loc){
    return (*loc).second;
  }

  //Default value if not found
  return std::string();
}


inline ConnectionParameter make_connection_parameter(std::string link) {

  boost::system::result<boost::url_view> parsedLink = boost::urls::parse_uri(link);

  ConnectionParameter connectionParameter;
  connectionParameter.host = parsedLink->host_address();
  connectionParameter.port = parsedLink->port();
  if(connectionParameter.port.empty()){
    connectionParameter.port = GuessPortForScheme(parsedLink);
  }


  return connectionParameter;
}

#endif // !CONNECTION
