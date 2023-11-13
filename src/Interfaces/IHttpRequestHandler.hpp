#ifndef I_HTTP_REQUEST_HANDLER_
#define I_HTTP_REQUEST_HANDLER_

#include <stdint.h>

#include <string>
#include <map>
#include <vector>

class IHttpRequestHandler {
public:
    virtual void readRequest(int clientSocket, std::vector<char>& clientBuffer) = 0;
    virtual void parseRequest(std::vector<char>& clientBuffer, int defaultBufferHeaderSize, int clientMaxBodySize) = 0;
    virtual const std::map<std::string, std::string>& getRequest() const = 0;
    virtual std::string getHeader(const std::string& key) const = 0;
    virtual const std::vector<char>& getBody(void) const = 0;
    virtual ~IHttpRequestHandler() {};
};

#endif //I_HTTP_REQUEST_HANDLER_