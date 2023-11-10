#include <HttpResponseHandler.hpp>

void HttpResponseHandler::send(int socket,
                               int statusCode,
                               const char* reasonPhrase,
                               const char* headers,
                               const char* content) const {
    std::stringstream response;
    std::cout << headers << std::endl;
    response << "HTTP/1.1"
                << " "
                << statusCode
                << " "
                << reasonPhrase
                << "\r\n"
                << headers
                << "\r\n"
                << "\r\n"
                << content;

    write(socket, response.str().c_str(), response.str().size());
}
