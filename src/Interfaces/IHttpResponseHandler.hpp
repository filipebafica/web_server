#ifndef I_HTTP_RESPONSE_HANDLER_
#define I_HTTP_RESPONSE_HANDLER_

class IHttpResponseHandler {
public:
    virtual void send(
            int socket,
            int statusCode,
            const char* headers,
            const char* content
    ) const = 0;
};

#endif //I_HTTP_RESPONSE_HANDLER_