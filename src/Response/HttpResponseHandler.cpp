#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
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
            const char* resources = webserver->getResources(
                    requestHeaders["Method"],
                    requestHeaders["Route"]
            );

           if (std::string("GET") == requestHeaders["Method"]) {
               this->responseWriter(
                       clientSocket,
                       200,
                       "",
                       this->getContent(resources).c_str()
               );

               return;
           }

           if (std::string("POST") == requestHeaders["Method"]) {
               this->uploadFile(
                       this->getContent(resources).c_str(),
                       webserver->getUploadPath(requestHeaders["Route"])
               );

               this->responseWriter(
                       clientSocket,
                       201,
                       "",
                       "file was uploaded successfully"
               );

               return;
           }

           if (std::string("DELETE") == requestHeaders["Method"]) {
               this->responseWriter(
                       clientSocket,
                       200,
                       "",
                       "DELETE has been made"
               );

               return;
           }

        } catch (MethodNotAllowedException& exception) {
            this->responseWriter(
                    clientSocket,
                    405,
                    "",
                    "Method not allowed\n"
            );

        } catch (RouteNotFoundException& exception) {
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

    void uploadFile(const char* content, const char* fileName) const {
        FILE* destinationFile = fopen(fileName, "w+");
        fwrite(content, strlen(content), strlen(content), destinationFile);
        fclose(destinationFile);
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