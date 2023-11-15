#include <HttpResponseHandler.hpp>

//void HttpResponseHandler::send(int socket,
//                               int statusCode,
//                               const char* reasonPhrase,
//                               const char* headers,
//                               const char* content) const {
//    std::stringstream response;
//    response << "HTTP/1.1"
//                << " "
//                << statusCode
//                << " "
//                << reasonPhrase
//                << "\r\n"
//                << headers
//                << "\r\n"
//                << "\r\n"
//                << content;
//
//    write(socket, response.str().c_str(), response.str().size());
//}

void HttpResponseHandler::send(int socket,
                               int statusCode,
                               const char* reasonPhrase,
                               const char* headers,
                               const char* content,
                               size_t contentLength) const {
    std::stringstream response;
    response << "HTTP/1.1"
             << " "
             << statusCode
             << " "
             << reasonPhrase
             << "\r\n"
             << headers
             << "\r\n"
             << "\r\n";

    // Sends the HTTP headers
    write(socket, response.str().c_str(), response.str().size());

    // Sends the binary content
    write(socket, content, contentLength);
}
