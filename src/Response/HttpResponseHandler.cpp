#include <sstream>
#include <iostream>
#include <unistd.h>
#include "../Interfaces/IHttpResponseHandler.hpp"

class HttpResponseHandler : public IHttpResponseHandler {
public:
    void send(int statusCode, int socket, const char* content) {
        std::stringstream response;

        response << "HTTP/1.1" << " " << statusCode << " " << "\r\n" << "\r\n" << content;
        write(socket, response.str().c_str(), response.str().size());
    }
};