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
    void readRequest(int clientSocket, std::vector<char>& clientBuffer);
    void parseRequest(std::vector<char>& clientBuffer, int defaultBufferHeaderSize, int clientMaxBodySize);
    const std::map<std::string, std::string>& getRequest() const;
    std::string getHeader(const std::string& key) const;
    const std::vector<char>& getBody(void) const;
    void parseURI(void);

private:
    std::map<std::string, std::string> request;
    std::vector<char> body;
    size_t bodyLen;
    size_t requestLen;
    char defaultBuffer[1024 * 1024];

    void validateRequest(char* buffer, int defaultBufferHeaderSize, size_t clientMaxBodySize);
    void parseRequestLine(char* buffer);
    void parseRequestHeader(char* buffer);
    void parseRequestBody(char* buffer);
    void setChunkedRequest(char* chunkedBody);
    std::vector<std::string> tokenize(const char* s, const char* delim);

    static std::string  _decodeURI(const std::string& encoded);
    static char         _decodePercentEncoded(const char* hexDigits);
    static int          _hex2int(char c);
    static std::string  _checkCGIExtension(const std::string& decodedURI);
    static std::size_t _find(const char *buffer, std::size_t pos, std::size_t bufferLen, const char *delim);
};

#endif  // HTTP_REQUEST_HANDLER_HPP
