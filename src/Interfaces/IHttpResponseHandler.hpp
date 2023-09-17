#ifndef I_HTTP_RESPONSE_HANDLER_
#define I_HTTP_RESPONSE_HANDLER_

class IHttpResponseHandler {
public:
    virtual void responseWriter(int socket, int statusCode, const char* headers, const char* content) = 0;
};

#endif //I_HTTP_RESPONSE_HANDLER_