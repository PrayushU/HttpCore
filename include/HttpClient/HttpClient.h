#include <memory>
#include <HttpClientParamters.h>

struct HttpClient{
  static std::shared_ptr<HttpClient> Make(HttpClientParameters httpClientParamters){
    return std::make_shared<HttpClient>();
  } 
};
