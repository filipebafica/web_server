#ifndef SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_
#define SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_

#include <unistd.h>

#include <iostream>
#include <sstream>

#include <IHttpResponseHandler.hpp>

class HttpResponseHandler : public IHttpResponseHandler {
 public:
    void send(int socket,
              int statusCode,
              const char* headers,
              const char* content) const;
};

#endif  /* SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_ */