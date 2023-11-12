#ifndef SRC_CGI_CGIREQUEST_HPP_
#define SRC_CGI_CGIREQUEST_HPP_

#include <stdint.h>

#include <string>
#include <vector>

class CGIRequest {
 public:
    const std::string method;
    const std::string accept;
    const std::string agent;

    const std::string serverRoot;
    const std::string uri;
    const std::string querystring;

    const std::string contentLen;
    const std::string contentType;
    const std::vector<char>& body;

    const std::string customScriptPath;

    CGIRequest(
      const std::string method,
      const std::string accept,
      const std::string agent,
      const std::string serverRoot,
      const std::string uri,
      const std::string querystring,
      const std::string contentLen,
      const std::string contentType,
      const std::vector<char>& body,
      const std::string customScriptPath = std::string());

    ~CGIRequest();
};

#endif /* SRC_CGI_CGIREQUEST_HPP_ */
