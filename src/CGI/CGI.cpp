#include <CGI.hpp>

#define BUFFSIZE    4096
#define CGI_BIN     "php-cgi"
// #define CGI_PATH    "/usr/bin/php-cgi"

CGI::CGI(void) {}

CGI::~CGI() {}

CGIResponse* CGI::execute(const CGIRequest& request) const {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        std::cerr << "pipe() failed" << std::endl;
        return new CGIResponse(500);
    }

    pid_t p = fork();

    if (p < 0) {
        std::cerr << "fork() failed" << std::endl;
        return new CGIResponse(500);
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

        envp.setVariable("PATH_INFO", request.uri.substr(1));
        envp.setVariable("SCRIPT_NAME", CGI_PATH);
        envp.setVariable("REQUEST_URI", request.uri);
        envp.setVariable("REQUEST_METHOD", request.method);
        envp.setVariable("HTTP_ACCEPT", request.accept);
        envp.setVariable("HTTP_USER_AGENT", request.agent);

        envp.setVariable("DOCUMENT_ROOT", request.serverRoot);
        if (request.method == "POST") {
            envp.setVariable("CONTENT_LENGTH", request.contentLen);
            envp.setVariable("CONTENT_TYPE", request.contentType);
            envp.setVariable("SCRIPT_FILENAME", "testing/postFile.php");
        } else {
            /* Full path to script file */
            envp.setVariable("SCRIPT_FILENAME", request.serverRoot + request.uri);
        }

        if (request.querystring.size()) {
            envp.setVariable("QUERY_STRING", request.querystring);
        }
        char *args[2] = { (char*)CGI_BIN, NULL };

        /* CGI_PATH Being defined on Makefile */
        execve(CGI_PATH, args, envp.data());
        exit(1);
    }
    /* Parent process */
    close(pipe_fd[1]);

    std::string rawCGIResponse;
    char        buffer[BUFFSIZE];

    while (read(pipe_fd[0], buffer, BUFFSIZE) > 0) {
        rawCGIResponse.append(buffer);
    }
    waitpid(p, NULL, 0);
    return new CGIResponse(rawCGIResponse);
}
