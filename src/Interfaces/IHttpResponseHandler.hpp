class IHttpResponseHandler {
public:
    virtual void responseWriter(int socket, int statusCode, const char* headers, const char* content) = 0;
};