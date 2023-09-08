class IHttpResponseHandler {
public:
    virtual void send(int statusCode, int socket, const char* content) = 0;
};