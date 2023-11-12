#ifndef I_HTTP_REQUEST_HANDLER_
#define I_HTTP_REQUEST_HANDLER_

#include <stdint.h>

#include <string>
#include <map>
#include <vector>

class IHttpRequestHandler {
public:
    virtual void readRequest(int clientSocket, char** buffer, int bufferSize) = 0;
    virtual void parseRequest(char* buffer, int defaultBufferHeaderSize, int clientMaxBodySize) = 0;
    virtual const std::map<std::string, std::string>& getRequest() const = 0;
    virtual std::string getHeader(const std::string& key) const = 0;
    virtual const std::vector<char>& getBody(void) const = 0;
    virtual ~IHttpRequestHandler() {};
};

#endif //I_HTTP_REQUEST_HANDLER_