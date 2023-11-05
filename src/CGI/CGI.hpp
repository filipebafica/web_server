#ifndef SRC_CGI_CGI_HPP_
#define SRC_CGI_CGI_HPP_

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <CGIEnvironment.hpp>
#include <CGIRequest.hpp>
#include <ICGI.hpp>
#include <IHttpRequestHandler.hpp>

class CGI : public ICGI {
 public:
    CGI(void);
    ~CGI();

    CGIResponse* execute(const CGIRequest& request);

 private:
    CGIEnvironment _envp;

    void _setEnvironment(const CGIRequest& request);
};

#endif /* SRC_CGI_CGI_HPP_ */
