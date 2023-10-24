#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "./CGIEnvironment.hpp"
#include "../Interfaces/IHttpRequestHandler.hpp"
#include "../Interfaces/ICGI.hpp"

class CGI : public ICGI {
 public:
    CGI();
    ~CGI();

    CGIResponse* execute(IHttpRequestHandler *httpRequestHandler) const;
};
