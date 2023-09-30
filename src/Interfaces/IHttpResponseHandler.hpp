#ifndef I_HTTP_RESPONSE_HANDLER_
#define I_HTTP_RESPONSE_HANDLER_

#include "../Webserver.cpp"

class IHttpResponseHandler {
public:
    virtual void send(
            int clientSocket,
            Webserver* webserver,
            std::map<std::string, std::string> requestHeaders
    ) const = 0;
};

#endif //I_HTTP_RESPONSE_HANDLER_