#include <CGI.hpp>
#include <CGIRequest.hpp>
#include <CGIResponse.hpp>

const char *body =  \
    "-----------------------------126829857566315249935482909\n"
    "Content-Disposition: form-data; "
    "name=\"fileToUpload\"; filename=\"helloToCGI.txt\"\n"
    "Content-Type: text/plain\n"
    "\n"
    "Hello CGI\n"
    "-----------------------------126829857566315249935482909--";

const char *contentType = \
    "multipart/form-data;"
    " boundary=---------------------------126829857566315249935482909";

int main(void) {
    CGI cgi;

    CGIRequest cgiRequest(
        "POST",
        "",
        "",
        "/home/vlima-nu/42/web-server/static",
        "/postFile.php",
        "",
        "230",
        contentType,
        body
    );

    CGIResponse* cgiResponse = cgi.execute(cgiRequest);

    std::cout << cgiResponse->getCGIStatus() << std::endl;
    std::cout << cgiResponse->getCGIHeaders() << std::endl;
    std::cout << cgiResponse->getCGIBody() << std::endl;
    delete cgiResponse;

    return 0;
}
