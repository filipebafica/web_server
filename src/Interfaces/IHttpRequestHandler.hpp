#ifndef I_HTTP_REQUEST_HANDLER_
#define I_HTTP_REQUEST_HANDLER_

#include <string>
#include <map>

class IHttpRequestHandler {
public:
    virtual void parseRequest() = 0;
    virtual const std::map<std::string, std::string>& getRequestHeaders() const = 0;
    virtual void readRequest(int clientSocket) = 0;
    virtual ~IHttpRequestHandler() {};
};

#endif //I_HTTP_REQUEST_HANDLER_