#ifndef I_HTTP_RESPONSE_HANDLER_
#define I_HTTP_RESPONSE_HANDLER_

class IHttpResponseHandler {
public:
    virtual void send(
            int socket,
            int statusCode,
            const char* reasonPhrase,
            const char* headers,
            const char* content,
            size_t contentLength
    ) const = 0;
    virtual ~IHttpResponseHandler() {};
};

#endif //I_HTTP_RESPONSE_HANDLER_