#include "./CGIResponse.hpp"

/**
 * The constructor parses the CGI response string and extracts the status code,
 * headers, and body.
 * 
 * @param response The `response` parameter is a string that represents the
 * entire response received from a CGI script or server.
 */
CGIResponse::CGIResponse(const std::string& response) : _status(200) {
    size_t headerEnd = response.find("\r\n\r\n");

    if (headerEnd != std::string::npos) {
        _headers    = response.substr(0, headerEnd);
        _body       = response.substr(headerEnd + 4);

        size_t statusPos = _headers.find("Status:");

        if (statusPos != std::string::npos) {
            _status = std::atoi(_headers.c_str() + statusPos + 7);
            _headers.erase(
                statusPos,
                _headers.find("\r\n", statusPos) + 2 - statusPos
            );
        }
    } else {
        _body       = response;
    }
}

CGIResponse::~CGIResponse(void) {}

int CGIResponse::getCGIStatus(void) const {
    return _status;
}

const char* CGIResponse::getCGIHeaders(void) const {
    return _headers.c_str();
}

const char* CGIResponse::getCGIBody(void) const {
    return _body.c_str();
}
