#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <string.h>
#include "../Interfaces/IParser.hpp"

#define SPACE_DELIMITER " "
#define COLONS_DELIMITER ":"
#define END_OF_LINE_DELIMITER "\r\n"
#define REQUEST_HEADER_START_INDEX 1
#define REQUEST_HEADER_PAIR_SIZE 2

class HttpRequestParser : public IParser {
private:
    std::map<std::string, std::string> headers;
    std::string body;

public:
    void parse(char* httpRequest) {
        // Split the request into lines
        std::vector<std::string> lines;
        lines = this->tokenize(httpRequest, END_OF_LINE_DELIMITER);

        if (lines.empty()) {
            throw std::exception();
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

    void parseRequestLine(const std::string& line) {
        // Parse the request line (e.g., "GET /path/to/resource HTTP/1.1")
        std::vector<std::string> tokens;
        tokens = this->tokenize((char*)line.c_str(), SPACE_DELIMITER);
        this->headers["Method"] = tokens[0];
        this->headers["Path"] = tokens[1];
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

    const std::map<std::string, std::string>& getHeaders() const {
        return this->headers;
    }

//    const std::string& getBody() const {
//        return this->body;
//    }

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
};
