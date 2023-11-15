#include <vector>

#include <Signals.hpp>
#include <Monitor.hpp>
#include <CGI.hpp>
#include <Parser.hpp>
#include <HttpRequestHandler.hpp>
#include <HttpResponseHandler.hpp>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "You must pass a path to the configuration file" << std::endl;
        return 1;
    }
    std::vector<ServerConfig> serverConfigs;

    try {
        Parser parser(argv[1]);
        serverConfigs = std::vector<ServerConfig>(parser.parse());
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::vector<Webserver*> webservers;
    HttpResponseHandler httpResponseHandler;
    HttpRequestHandler httpRequestHandler;
    CGI cgi;

    try {
        for (size_t i = 0; i < serverConfigs.size(); ++i) {
            webservers.push_back(
                new Webserver(
                        &serverConfigs[i],
                        &httpRequestHandler,
                        &httpResponseHandler,
                        &cgi
                )
            );
        }

        Monitor monitor(&webservers);
        Signals signals(&monitor);

        monitor.loop();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    for (size_t i = 0; i < webservers.size(); i++) {
        delete webservers[i];
    }
    return 0;
}
