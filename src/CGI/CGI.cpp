#include <CGI.hpp>

#define BUFFSIZE    4096
#define CGI_BIN     "php-cgi"

#define RD 0 /* Read end pipe index */
#define WR 1 /* Write end pipe index */

// #include <fcntl.h>

/*
uint8_t *body =  (uint8_t *)\
    "-----------------------------115407863137634137613693109055\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"inception.txt\"\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "0Lza(#Ad6VtXE7zR0LZXdRN$\n"
    "\n"
    "\n"
    "DOMAIN_NAME=vlima-nu.42.fr\n"
    "\n"
    "MYSQL_HOST=mariadb\n"
    "MYSQL_DATABASE=wordpress\n"
    "MYSQL_ROOT=root\n"
    "MYSQL_ROOT_PASSWORD='0Lza(#Ad6VtXE7zR0LZXdRN$'\n"
    "MYSQL_USER=vlima-nu\n"
    "MYSQL_PASSWORD='0Lza(#Ad6VtXE7zR0LZXdRN$'\n"
    "WORDPRESS_USER=cadet\n"
    "WORDPRESS_PASSWORD=123\n"
    "-----------------------------115407863137634137613693109055--";
    // "\r\n\r\n";

const char *contentType = \
    "multipart/form-data; boundary=---------------------------115407863137634137613693109055";

*/
#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

CGI::CGI(void) {}

CGI::~CGI() {}

CGIResponse* CGI::execute(const CGIRequest& request) {
    int pipeFdToChild[2];
    int pipeFdFromChild[2];

    // if (request.body.size()) {
    //     int inputFile = open("tmp", O_CREAT | O_WRONLY | O_TRUNC);

    //     if (inputFile > 0) {
    //         write(inputFile, request.body.data(), request.body.size());
    //         close(inputFile);
    //     } else {
    //         std::cout << "Failed to open file to write" << std::endl;
    //     }
    // }

    if (pipe(pipeFdToChild) == -1 || pipe(pipeFdFromChild) == -1) {
        std::cerr << "pipe() failed" << std::endl;
        return new CGIResponse(500);
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "fork() failed" << std::endl;
        return new CGIResponse(500);
    }

    if (!pid) {
        /* Child process */
        char * const argv[2] = {
            const_cast<char *>(CGI_BIN),
            NULL
        };

        this->_setEnvironment(request);
        close(pipeFdToChild[WR]);

        // size_t bytes;    
        // char buffer[256];

        // std::cout << "Input from child process: " << std::endl;
        // while ((bytes = read(pipeFdToChild[RD], buffer, sizeof(buffer) - 1)) > 0) {
        //     buffer[bytes] = 0;
        //     std::cout << buffer;
        // }
        // std::cout << std::endl;


        // if (request.body.size()) {
        //     int inputFile = open("tmp", O_RDONLY);
        //     if (inputFile > 0) {
        //         dup2(inputFile, STDIN_FILENO);
        //     } else {
        //         std::cout << "Failed to open file to read" << std::endl;
        //     }
        // }

        dup2(pipeFdFromChild[WR], STDOUT_FILENO);
        dup2(pipeFdToChild[RD], STDIN_FILENO);

        close(pipeFdToChild[RD]);
        close(pipeFdFromChild[RD]);

        close(pipeFdFromChild[WR]);

        /* CGI_PATH Being defined on Makefile */
        execve(CGI_PATH, argv, this->_envp.data());
    }
    /* Parent process */

    close(pipeFdToChild[RD]);
    /* Writes request body through the pipe to the child process */
    if (request.body.size()) {
        std::cout << __func__ << ": Request body size: " << request.body.size() << std::endl;
        write(pipeFdToChild[WR], request.body.data(), request.body.size());
        // std::cout << __func__ << ": Request body size: " << strlen((char *)body) + 1 << std::endl;
        // write(pipeFdToChild[WR], body, strlen((char *)body) + 1);
    }
    close(pipeFdToChild[WR]);
    waitpid(pid, NULL, 0);
    close(pipeFdFromChild[WR]);

    std::string rawCGIResponse;
    char buffer[BUFFSIZE + 1];
    int bytes;

    while ((bytes = read(pipeFdFromChild[RD], buffer, BUFFSIZE)) > 0) {
        buffer[bytes] = 0;
        rawCGIResponse.append(buffer);
    }
    close(pipeFdFromChild[RD]);

    std::cout << __func__ << ": " << rawCGIResponse << std::endl;
    return new CGIResponse(rawCGIResponse);
}

void CGI::_setEnvironment(const CGIRequest& request) {
    this->_envp.setVariable("REDIRECT_STATUS", "200");
    this->_envp.setVariable("GATEWAY_INTERFACE", "CGI/1.1");
    this->_envp.setVariable("REQUEST_SCHEME", "http");
    this->_envp.setVariable("SERVER_PROTOCOL", "HTTP/1.1");

    this->_envp.setVariable("PATH_INFO", request.uri);
    this->_envp.setVariable("SCRIPT_NAME", CGI_PATH);
    this->_envp.setVariable("REQUEST_URI", request.uri);
    this->_envp.setVariable("REQUEST_METHOD", request.method);
    this->_envp.setVariable("HTTP_ACCEPT", request.accept);
    this->_envp.setVariable("HTTP_USER_AGENT", request.agent);

    this->_envp.setVariable("DOCUMENT_ROOT", request.serverRoot);
    if (!request.customScriptPath.empty()) {
        this->_envp.setVariable("SCRIPT_FILENAME", request.customScriptPath);
    } else {
        this->_envp.setVariable("SCRIPT_FILENAME", request.serverRoot + request.uri);
    }
    if (request.method == "POST") {
        this->_envp.setVariable("CONTENT_LENGTH", SSTR(request.body.size()));
        this->_envp.setVariable("CONTENT_TYPE", request.contentType);
        // this->_envp.setVariable("CONTENT_LENGTH", "509");
        // this->_envp.setVariable("CONTENT_TYPE", contentType);
    }
    if (request.querystring.size()) {
        this->_envp.setVariable("QUERY_STRING", request.querystring);
    }
    this->_envp.dumpEnv();
}
