#ifndef CONNECTION_H
#define CONNECTION_H

#include <algorithm>
#include <boost/url.hpp>
#include <string>
#include <vector>

struct ConnectionParameter {
  std::string host = {};
  std::string port = {};
  bool useTls = false;
};

inline bool GuessTlsNeed(auto &parsedLink) {
  auto scheme = parsedLink->scheme();
  static std::vector<std::pair<std::string, bool>> mappings = {
      {"http", false}, {"https", true}, {"ssl", true}};

  auto loc = std::find_if(std::begin(mappings), std::end(mappings),
                          [&scheme](std::pair<std::string, bool> &item) {
                            return item.first == scheme;
                          });

  if(std::end(mappings) != loc){
    return (*loc).second;
  }

  return false;
}

inline std::string GuessPortForScheme(auto &parsedLink) {

  auto scheme = parsedLink->scheme();
  static std::vector<std::pair<std::string, std::string>> mappings = {
      {"http", "80"}, {"https", "443"}, {"ssl", "443"}};

  auto loc = std::find_if(std::begin(mappings), std::end(mappings),
                          [&scheme](std::pair<std::string, std::string> &item) {
                            return item.first == scheme;
                          });

  if (std::end(mappings) != loc) {
    return (*loc).second;
  }

  // Default value if not found
  return std::string();
}

inline ConnectionParameter make_connection_parameter(std::string link) {

  boost::system::result<boost::url_view> parsedLink =
      boost::urls::parse_uri(link);

  ConnectionParameter connectionParameter;
  connectionParameter.host = parsedLink->host_address();
  connectionParameter.port = parsedLink->port();
  if (connectionParameter.port.empty()) {
    connectionParameter.port = GuessPortForScheme(parsedLink);
  }

  connectionParameter.useTls = GuessTlsNeed(parsedLink);

  return connectionParameter;
}

#endif // !CONNECTION
