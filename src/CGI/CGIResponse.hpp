#include <cstdlib>
#include <string>
#include <vector>

class CGIResponse {
 public:
    explicit CGIResponse(int statusCode);
    explicit CGIResponse(const std::string&);
    ~CGIResponse();

    int         getCGIStatus(void) const;
    const char* getCGIHeaders(void) const;
    const char* getCGIBody(void) const;

 private:
    int                       _status;
    std::string               _headers;
    std::string               _body;
    std::vector<std::string>  _responseParts;
};
