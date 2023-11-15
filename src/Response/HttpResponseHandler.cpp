#include <HttpResponseHandler.hpp>

void HttpResponseHandler::send(int socket,
                               int statusCode,
                               const char* reasonPhrase,
                               const char* headers,
                               std::vector<char>& binaryContent) const {
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

    std::string str = response.str();

    binaryContent.insert(binaryContent.begin(), str.begin(), str.end());

    write(socket, binaryContent.data(), binaryContent.size());
}

void HttpResponseHandler::send(int socket,
                               int statusCode,
                               const char* reasonPhrase,
                               const char* headers,
                               const char* content) const {
    std::stringstream response;
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


    // Sends the HTTP headers
    write(socket, response.str().c_str(), response.str().size());
}
