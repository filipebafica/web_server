#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <CGIEnvironment.hpp>
#include <CGIRequest.hpp>
#include <ICGI.hpp>
#include <IHttpRequestHandler.hpp>

class CGI : public ICGI {
 public:
    CGI();
    ~CGI();

    CGIResponse* execute(const CGIRequest& request) const;
};
