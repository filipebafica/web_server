#include <CGI.hpp>

#define BUFFSIZE    4096
#define CGI_BIN     "php-cgi"

#define RD 0 /* Read end pipe index */
#define WR 1 /* Write end pipe index */

CGI::CGI(void) {}

CGI::~CGI() {}

CGIResponse* CGI::execute(const CGIRequest& request) {
    int pipeFdToChild[2];
    int pipeFdFromChild[2];

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
        write(pipeFdToChild[WR], request.body.c_str(), request.body.size() + 1);
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
        this->_envp.setVariable("CONTENT_LENGTH", request.contentLen);
        this->_envp.setVariable("CONTENT_TYPE", request.contentType);
    }
    if (request.querystring.size()) {
        this->_envp.setVariable("QUERY_STRING", request.querystring);
    }
}
