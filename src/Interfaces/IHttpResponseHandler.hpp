#ifndef I_HTTP_RESPONSE_HANDLER_
#define I_HTTP_RESPONSE_HANDLER_

#include <vector>

class IHttpResponseHandler {
public:
    virtual void send(
            int socket,
            int statusCode,
            const char* reasonPhrase,
            const char* headers,
            std::vector<char>& binaryContent
    ) const = 0;

    virtual void send(
            int socket,
            int statusCode,
            const char* reasonPhrase,
            const char* headers,
            const char* content
    ) const = 0;
    virtual ~IHttpResponseHandler() {};
};

#endif //I_HTTP_RESPONSE_HANDLER_