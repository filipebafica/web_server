#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "../Interfaces/IHttpRequestHandler.hpp"

#define SPACE_DELIMITER " "
#define COLONS_DELIMITER ":"
#define END_OF_LINE_DELIMITER "\r\n"
#define REQUEST_HEADER_START_INDEX 1
#define REQUEST_HEADER_PAIR_SIZE 2

class HttpRequestHandler : public IHttpRequestHandler {
private:
    std::map<std::string, std::string> headers;
    std::string body;

public:
    void parseRequest(Webserver* webserver) {
        // Split the request into lines
        std::vector<std::string> lines;
        lines = this->tokenize(webserver->getBuffer(), END_OF_LINE_DELIMITER);

        if (lines.empty()) {
            return;
        }

        // Parse Request-Line (first line)
        this->parseRequestLine(lines[0]);

        // Parse Request-Header
        this->parseRequestHeader(lines);

//        // Parse body (for simplicity, this parser assumes content length is specified)
//        if (this->headers.find("Content-Length") != this->headers.end()) {
//            int contentLength = std::stoi(this->headers["Content-Length"]);
//            this->body = httpRequest.substr(httpRequest.size() - contentLength);
//        }
    }

    const std::map<std::string, std::string>& getRequestHeaders() const {
        return this->headers;
    }

//    const std::string& getBody() const {
//        return this->body;
//    }

    void readRequest(int clientSocket, Webserver* webserver) {
        // Clear the buffer and read data from the client socket
        std::memset(webserver->getBuffer(), 0, sizeof(*webserver->getBuffer()));
        int bytesRead = read(clientSocket, webserver->getBuffer(), sizeof(*webserver->getBuffer()));

        if (bytesRead < 0) {
            std::cerr << "Error reading request" << std::endl;
        } else if (bytesRead == 0) {
            std::cerr << "Client disconnected" << std::endl;
        }

        std::cout << "********** REQUEST **********" << std::endl;
        std::cout << webserver->getBuffer() << std::endl;
    }

private:
    std::vector<std::string> tokenize(char* s, const char* delim) {
        std::vector<std::string> tokens;

        char *token = strtok(s, delim);
        while (token != NULL) {
            tokens.push_back(token);
            token = strtok(NULL, delim);
        }

        return tokens;
    }

    void parseRequestLine(const std::string& line) {
        // Parse the request line (e.g., "GET /path/to/resource HTTP/1.1")
        std::vector<std::string> tokens;
        tokens = this->tokenize((char*)line.c_str(), SPACE_DELIMITER);
        this->headers["Method"] = tokens[0];
        this->headers["Route"] = tokens[1];
        this->headers["Version"] = tokens[2];
    }

    void parseRequestHeader(std::vector<std::string> lines) {
        std::vector<std::string> tokens;
        std::string line;
        for (size_t i = REQUEST_HEADER_START_INDEX; i < lines.size(); ++i) {
            line = lines[i];
            tokens = this->tokenize((char*)line.c_str(), COLONS_DELIMITER);
            if (tokens.size() == REQUEST_HEADER_PAIR_SIZE) {
                this->headers[tokens[0]] = tokens[1];
            }
        }
    }
};
