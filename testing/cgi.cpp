#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "./CGIEnvironment.hpp"
#include "./CGIResponse.hpp"

int main(void) {
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe() failed");
        return 1;
    }

    pid_t p = fork();

    if (p < 0) {
        std::cerr << "fork() failed" << std::endl;
        return 1;
    }

    if (!p) {
        /* Child process */
        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        char *args[] = { (char *)"php-cgi", NULL };

        CGIEnvironment envp;

        envp.setVariable("REDIRECT_STATUS", "200");
        envp.setVariable("GATEWAY_INTERFACE", "CGI/1.1");
        envp.setVariable("REQUEST_SCHEME", "http");
        envp.setVariable("SERVER_PROTOCOL", "HTTP/1.1");
        envp.setVariable("SCRIPT_FILENAME", "/home/vlima-nu/42/web_server/cgi/index.php"); /* Only works with this env variable set */
        envp.setVariable("DOCUMENT_ROOT", "/home/vlima-nu/42/web_server");
        envp.setVariable("PATH_INFO", "/cgi/index.php");
        envp.setVariable("SCRIPT_NAME", "cgi/index.php");
        envp.setVariable("REQUEST_URI", "/cgi/index.php");
        envp.setVariable("REQUEST_METHOD", "GET");
        envp.setVariable("HTTP_ACCEPT", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
        envp.setVariable("HTTP_USER_AGENT", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/118.0.0.0 Safari/537.36 Edg/118.0.2088.57");

        execve("/usr/bin/php-cgi", args, envp.data());
        perror("execve");
        exit(1);
    } else {
        /* Parent process */
        close(pipe_fd[1]);

        std::string response;
        char        buffer[4096];

        while (read(pipe_fd[0], buffer, sizeof(buffer)) > 0) {
            response.append(buffer);
        }

        waitpid(p, NULL, 0);

        CGIResponse cgiResp(response);

        std::cout << "Status: "     << cgiResp.getCGIStatus() << std::endl;
        std::cout << "Headers: "    << cgiResp.getCGIHeaders() << std::endl;
        std::cout << "Body: "       << cgiResp.getCGIBody() << std::endl;
    }

    return 0;
}
