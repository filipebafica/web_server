#include <HttpResponseHandler.hpp>

void HttpResponseHandler::send(int socket,
                               int statusCode,
                               const char* headers,
                               const char* content) const {
    std::stringstream response;

    response << "HTTP/1.1"
                << " "
                << statusCode
                << "\r\n"
                << headers
                << "\r\n"
                << "\r\n"
                << content;

    write(socket, response.str().c_str(), response.str().size());
}
