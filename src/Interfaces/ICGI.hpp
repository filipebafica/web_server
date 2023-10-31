#ifndef I_CGI_HPP_
#define I_CGI_HPP_

#include "../CGI/CGIResponse.hpp"

class ICGI {
public:
    virtual CGIResponse* execute(IHttpRequestHandler *httpRequestHandler) const = 0;
    virtual ~ICGI() {};
};

#endif //I_CGI_HPP_
