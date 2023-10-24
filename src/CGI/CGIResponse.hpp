#include <cstdlib>
#include <string>
#include <vector>

using std::string;
using std::vector;

class CGIResponse {
 public:
    explicit CGIResponse(const string&);
    ~CGIResponse();

    int         getCGIStatus(void) const;
    const char* getCGIHeaders(void) const;
    const char* getCGIBody(void) const;

 private:
    int              _status;
    string           _headers;
    string           _body;
    vector<string>   _responseParts;
};
