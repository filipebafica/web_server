#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <unistd.h>
#include "../Interfaces/IHttpResponseHandler.hpp"

class HttpResponseHandler : public IHttpResponseHandler {
public:
    void send(
            int clientSocket,
            Webserver* webserver,
            std::map<std::string, std::string> requestHeaders
        ) const {
        try {
            const char* resourcesPath = webserver->getResourcesPath(
                    requestHeaders["Method"],
                    requestHeaders["Path"]
                    );

            this->responseWriter(
                    clientSocket,
                    200,
                    "",
                    this->getContent(resourcesPath).c_str()
            );

        } catch (MethodNotAllowedException& exception) {
            this->responseWriter(
                    clientSocket,
                    405,
                    "",
                    "Method not allowed\n"
            );

        } catch (PathNotFoundException& exception) {
            this->responseWriter(
                    clientSocket,
                    404,
                    "",
                    this->getContent(webserver->getErrorPage()).c_str()
            );
        }
    }

private:
    std::string getContent(const char* path) const {
        std::ifstream file;
        std::string content;
        std::string line;

        file.open(path, std::ios::binary);
        while(getline(file, line)) {
            content += line;
        }
        file.close();

        return content;
    }

    void responseWriter(int socket, int statusCode, const char* headers, const char* content) const {
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
};