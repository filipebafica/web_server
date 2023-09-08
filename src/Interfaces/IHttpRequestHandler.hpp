#include <string>

class IHttpRequestHandler {
public:
    virtual void parse(char* s) = 0;
    virtual const std::map<std::string, std::string>& getHeaders() const = 0;
    virtual ~IHttpRequestHandler() {};
};
