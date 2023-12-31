#include <CGIRequest.hpp>

CGIRequest::CGIRequest(
    const std::string method,
    const std::string accept,
    const std::string agent,
    const std::string serverRoot,
    const std::string uri,
    const std::string querystring,
    const std::string contentLen,
    const std::string contentType,
    const std::vector<char>& body,
    const std::string customScriptPath)
    : method(method)
    , accept(accept)
    , agent(agent)
    , serverRoot(serverRoot)
    , uri(uri)
    , querystring(querystring)
    , contentLen(contentLen)
    , contentType(contentType)
    , body(body)
    , customScriptPath(customScriptPath) {}

CGIRequest::~CGIRequest() {}
