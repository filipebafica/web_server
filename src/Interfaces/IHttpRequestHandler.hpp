#ifndef I_HTTP_REQUEST_HANDLER_
#define I_HTTP_REQUEST_HANDLER_

#include <string>
#include <map>
#include "../Webserver.cpp"

class IHttpRequestHandler {
public:
    virtual void readRequest(int clientSocket, Webserver* webserver) = 0;
    virtual void parseRequest(Webserver* webserver) = 0;
    virtual const std::map<std::string, std::string>& getRequestHeaders() const = 0;
    virtual ~IHttpRequestHandler() {};
};

#endif //I_HTTP_REQUEST_HANDLER_