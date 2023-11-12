#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

#include <IHttpRequestHandler.hpp>
#include <Exceptions.hpp>

class HttpRequestHandler : public IHttpRequestHandler {
public:
    void readRequest(int clientSocket, char** buffer, int bufferSize);
    void parseRequest(char* buffer, int defaultBufferHeaderSize, int clientMaxBodySize);
    const std::map<std::string, std::string>& getRequest() const;
    std::string getHeader(const std::string& key) const;
    const std::vector<char>& getBody(void) const;
    void parseURI(void);

private:
    std::map<std::string, std::string> request;
    std::vector<char> body;
    size_t requestLen;

    void validateRequest(char* buffer, int defaultBufferHeaderSize, int clientMaxBodySize);
    void parseRequestLine(char* buffer);
    void parseRequestHeader(char* buffer);
    void parseRequestBody(char* buffer);
    void setChunkedRequest(std::string chunkedBody);
    std::vector<std::string> tokenize(const char* s, const char* delim);

    static std::string  _decodeURI(const std::string& encoded);
    static char         _decodePercentEncoded(const char* hexDigits);
    static int          _hex2int(char c);
    static std::string  _checkCGIExtension(const std::string& decodedURI);
};

#endif  // HTTP_REQUEST_HANDLER_HPP
