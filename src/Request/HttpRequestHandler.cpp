#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "../Interfaces/IHttpRequestHandler.hpp"

#define SPACE_DELIMITER " "
//#define COLONS_DELIMITER ":"
#define COLONS_DELIMITER ": "
#define END_OF_LINE_DELIMITER "\r\n"
#define HEADER_AND_BODY_DELIMITER "\r\n\r\n"
#define REQUEST_HEADER_START_INDEX 1
#define REQUEST_HEADER_PAIR_SIZE 2
#define EXPECTED_NUMBER_OF_TOKENS_IN_A_REQUEST_LINE 3

class HttpRequestHandler : public IHttpRequestHandler {
private:
    char buffer[1024];
    std::map<std::string, std::string> request;

public:
    void readRequest(int clientSocket) {
        // Clear the buffer and read data from the client socket
        std::memset(this->buffer, 0, sizeof(this->buffer));
        int bytesRead = read(clientSocket, this->buffer, sizeof(this->buffer));

        if (bytesRead < 0) {
            std::cerr << "Error reading request" << std::endl;
        } else if (bytesRead == 0) {
            std::cerr << "Client disconnected" << std::endl;
        }

        std::cout << "********** REQUEST **********" << std::endl;
        std::cout << this->buffer << std::endl;
    }

    void parseRequest() {
        // Parses Request-Line (first line)
        this->parseRequestLine();

        // Parses Request-Header
        this->parseRequestHeader();

        // Parses the Request-Body
        this->parseRequestBody();
    }

    const std::map<std::string, std::string>& getRequest() const {
        return this->request;
    }

    std::string getHeader(const std::string& key) const {
        if (this->request.find(key) == this->request.end()) {
            return std::string("");
        }
        return this->request.find(key)->second;
    }

//    const std::string& getBody() const {
//        return this->body;
//    }

private:
    void parseRequestLine() {
        /* Parses the request line (e.g., "GET /path/to/resource HTTP/1.1") */

        // Splits the request into lines
        std::vector<std::string> lines = this->tokenize(
                this->buffer,

                END_OF_LINE_DELIMITER
        );

        if (lines.empty()) {
            return;
        }

        // Splits the first line (request-line) into tokens
        std::vector<std::string> tokens = this->tokenize((char*)lines[0].c_str(), SPACE_DELIMITER);

        if (tokens.empty() || tokens.size() != EXPECTED_NUMBER_OF_TOKENS_IN_A_REQUEST_LINE) {
            return;
        }

        this->request["Method"] = tokens[0];
        this->request["Route"] = tokens[1];
        this->request["Version"] = tokens[2];
    }

    void parseRequestHeader() {
        /* Parses the request headers (e.g., "
            Host: www.example.com
            Content-Type: application/octet-stream
            Accept-Language: en-US,en;q=0.5
         ") */


        // Splits the request blocks (header and body)
        std::vector<std::string> blocks = this->tokenize(this->buffer, HEADER_AND_BODY_DELIMITER);

        if (blocks.empty()) {
            return;
        }

        // Splits the request into lines
        std::vector<std::string> lines = this->tokenize(blocks[0].c_str(), END_OF_LINE_DELIMITER);

        if (lines.empty()) {
            return;
        }

        // Adds to the request vector the remaining headers (request-header)
        for (size_t i = REQUEST_HEADER_START_INDEX; i < lines.size(); ++i) {
            std::string line = lines[i];
            std::vector<std::string> tokens = this->tokenize((char*)line.c_str(), COLONS_DELIMITER);
            if (tokens.size() == REQUEST_HEADER_PAIR_SIZE) {
                this->request[tokens[0]] = tokens[1];
            }
        }
    }

    void parseRequestBody() {
        /* Parses and stores the message-body */

        // Splits the request blocks (header and body)
        std::vector<std::string> tokens = this->tokenize(this->buffer, HEADER_AND_BODY_DELIMITER);

        // Validates if there is a body
        if (tokens.empty() || tokens.size() < 2) {
            return;
        }

        if (this->getHeader("Transfer-Encoding") == std::string("chunked")) {
           this->setChunkedRequest(tokens[1]);
            return;
        }

        this->request["Body"] = tokens[1];
    }

    void setChunkedRequest(std::string chunkedBody) {
        std::string body;
        size_t pos = 0;
        while (pos < chunkedBody.size()) {
            // Finds the position of the CRLF (END_OF_LINE_DELIMITER)
            size_t crlfPos = chunkedBody.find(END_OF_LINE_DELIMITER, pos);
            if (crlfPos == std::string::npos) {
                // Invalid format
                break;
            }

            // Extract the chunk size // 16 is used to define the base of the chunk length
            std::string chunkSizeStr = chunkedBody.substr(pos, crlfPos - pos);
            long chunkSize = strtol(chunkSizeStr.c_str(), NULL, 16);

            if (chunkSize == 0) {
                // End of chunked data
                break;
            }

            // Move the position past the current CRLF
            pos = crlfPos + strlen(END_OF_LINE_DELIMITER);

            // Append the chunk data to the result
            body += chunkedBody.substr(pos, chunkSize);

            // Move the position past the chunk data and CRLF
            pos += chunkSize + strlen(END_OF_LINE_DELIMITER);
        }

        this->request["Body"] = body;

        std::cout << "********** BODY **********" << std::endl;
        std::cout <<  this->request["Body"] << std::endl;
    }

    std::vector<std::string> tokenize(const char* s, const char* delim) {
        std::vector<std::string> tokens;
        std::string text = s;
        std::string delimiter = delim;

        size_t pos = 0;
        while ((pos = text.find(delimiter)) != std::string::npos) {
            std::string token = text.substr(0, pos);
            tokens.push_back(token);
            text.erase(0, pos + delimiter.length());
        }

        if (!text.empty()) {
            tokens.push_back(text);
        }

        return tokens;
    }
};
