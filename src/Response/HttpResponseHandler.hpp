#ifndef SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_
#define SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_

#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <IHttpResponseHandler.hpp>

class HttpResponseHandler : public IHttpResponseHandler {
 public:
    void send(int socket,
              int statusCode,
              const char* reasonPhrase,
              const char* headers,
              std::vector<char>& binaryContent) const;

    void send(int socket,
              int statusCode,
              const char* reasonPhrase,
              const char* headers,
              const char* content) const;
};

#endif  /* SRC_RESPONSE_HTTPRESPONSEHANDLER_HPP_ */
