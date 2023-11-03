#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <cstdlib>

#include <IHttpRequestHandler.hpp>

class HttpRequestHandler : public IHttpRequestHandler {
public:
    void readRequest(int clientSocket);
    void parseRequest(void);
    const std::map<std::string, std::string>& getRequest() const;
    std::string getHeader(const std::string& key) const;
    void parseURI(void);

private:
    char buffer[1024];
    std::map<std::string, std::string> request;

    void parseRequestLine(void);
    void parseRequestHeader(void);
    void parseRequestBody(void);
    void setChunkedRequest(std::string chunkedBody);
    std::vector<std::string> tokenize(const char* s, const char* delim);

    static std::string  _decodeURI(const std::string& encoded);
    static char         _decodePercentEncoded(const char* hexDigits);
    static int          _hex2int(char c);
    static std::string  _checkCGIExtension(const std::string& decodedURI);
};

#endif  // HTTP_REQUEST_HANDLER_HPP
