#include <CGIResponse.hpp>

/**
 * The constructor parses the CGI response string and extracts the status code,
 * headers, and body.
 * 
 * @param response The `response` parameter is a string that represents the
 * entire response received from a CGI script or server.
 */
CGIResponse::CGIResponse(const std::string& response) : _status(200) {
    if (!response.length()) {
        this->_status = 500;
        return;
    }

    size_t headerEnd = response.find("\r\n\r\n");

    if (headerEnd != std::string::npos) {
        this->_headers  = response.substr(0, headerEnd);
        this->_body     = response.substr(headerEnd + 4);

        size_t statusPos = this->_headers.find("Status:");

        if (statusPos != std::string::npos) {
            _status = std::atoi(this->_headers.c_str() + statusPos + 7);
            this->_headers.erase(
                statusPos,
                this->_headers.find("\r\n", statusPos) + 2 - statusPos
            );
        }
    } else {
        this->_body       = response;
    }
}

CGIResponse::CGIResponse(int statusCode)
    : _status(statusCode)
    , _headers()
    , _body() {}

CGIResponse::~CGIResponse(void) {}

int CGIResponse::getCGIStatus(void) const {
    return this->_status;
}

const char* CGIResponse::getCGIHeaders(void) const {
    return this->_headers.c_str();
}

const char* CGIResponse::getCGIBody(void) const {
    return this->_body.c_str();
}
