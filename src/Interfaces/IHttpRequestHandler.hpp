#ifndef I_HTTP_REQUEST_HANDLER_
#define I_HTTP_REQUEST_HANDLER_

#include <string>
#include <map>

class IHttpRequestHandler {
public:
    virtual void parse(char* s) = 0;
    virtual const std::map<std::string, std::string>& getHeaders() const = 0;
    virtual ~IHttpRequestHandler() {};
};

#endif //I_HTTP_REQUEST_HANDLER_