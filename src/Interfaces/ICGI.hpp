#ifndef I_CGI_HPP_
#define I_CGI_HPP_

#include <CGIRequest.hpp>
#include <CGIResponse.hpp>
#include <IHttpRequestHandler.hpp>

class ICGI {
public:
    virtual CGIResponse* execute(const CGIRequest& request) = 0;
    virtual ~ICGI() {};
};

#endif //I_CGI_HPP_
