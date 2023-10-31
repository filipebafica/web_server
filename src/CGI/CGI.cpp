#include "./CGI.hpp"

#define BUFFSIZE    4096
#define CGI_BIN     "php-cgi"
#define CGI_PATH    "/usr/bin/php-cgi"

CGI::CGI(void) {}

CGI::~CGI() {}

CGIResponse* CGI::execute(IHttpRequestHandler *httpRequestHandler) const {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe() failed");
        exit(1);
    }

    pid_t p = fork();

    if (p < 0) {
        std::cerr << "fork() failed" << std::endl;
        exit(1);
    }

    if (!p) {
        /* Child process */
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        CGIEnvironment envp;

        envp.setVariable("REDIRECT_STATUS", "200");
        envp.setVariable("GATEWAY_INTERFACE", "CGI/1.1");
        envp.setVariable("REQUEST_SCHEME", "http");
        envp.setVariable("SERVER_PROTOCOL", "HTTP/1.1");

        const string &uri       = httpRequestHandler->getHeader("Route");
        const string &method    = httpRequestHandler->getHeader("Method");
        const string &accept    = httpRequestHandler->getHeader("Accept");
        const string &agent     = httpRequestHandler->getHeader("User-Agent");

        envp.setVariable("PATH_INFO", uri); /** TODO: Decode URI */
        envp.setVariable("SCRIPT_NAME", uri);
        envp.setVariable("REQUEST_URI", uri);
        envp.setVariable("REQUEST_METHOD", method);
        envp.setVariable("HTTP_ACCEPT", accept);
        envp.setVariable("HTTP_USER_AGENT", agent);
        /**
         * TODO: Add methods to get the following key-values:
         *  - query string
         *  - CONTENT_LENGTH
         *  - CONTENT_TYPE

        const string &serverRootPath    = getServerRootPath();

        envp.setVariable("SCRIPT_FILENAME", serverRootPath + uri); // Full path to script file
        envp.setVariable("DOCUMENT_ROOT", serverRootPath); // Server root path
        envp.setVariable("CONTENT_LENGTH", getContentLen());
        envp.setVariable("CONTENT_TYPE", getContentType());

        if (hasQueryString()) {
            envp.setVariable("QUERY_STRING", getQueryString());
        }
        */
        char *args[2] = { (char*)CGI_BIN, NULL };

        execve(CGI_PATH, args, envp.data());

        return new CGIResponse(NULL);

    } else {
        /* Parent process */
        close(pipe_fd[1]);

        string  rawCGIResponse;
        char    buffer[BUFFSIZE];

        while (read(pipe_fd[0], buffer, BUFFSIZE) > 0) {
            rawCGIResponse.append(buffer);
        }
        int status;

        waitpid(p, &status, 0);

        return new CGIResponse(rawCGIResponse);
    }
}
