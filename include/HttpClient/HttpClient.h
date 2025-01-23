#ifndef HTTPCLIENT_T 
#define HTTPCLIENT_T


#include "HttpRequest.h"
#include "HttpResponse.h"
#include <memory>
#include <HttpClientParamters.h>

struct HttpClient{
  static std::shared_ptr<HttpClient> Make(HttpClientParameters httpClientParamters){
    return std::shared_ptr<HttpClient>(new HttpClient(std::move(httpClientParamters)));
  } 

  template<typename Func>
  void SendAsync(const HttpRequest& request, Func func){
    post(_httpClientParameters._executor, 
         [f=std::move(func)](){
         HttpResponse response;
         response.Body = "glad to know you";
         f(std::error_code(), std::move(response));
         });
  }

private:

  explicit HttpClient(HttpClientParameters httpClientParameters) : 
    _httpClientParameters(std::move(httpClientParameters)){


  }

  HttpClientParameters _httpClientParameters;

};


#endif 
