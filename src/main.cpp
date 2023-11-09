#include <vector>

#include <Signals.hpp>
#include <Monitor.hpp>
#include <CGI.hpp>
#include <Parser.hpp>
#include <HttpRequestHandler.hpp>
#include <HttpResponseHandler.hpp>

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cout << "You must pass a path to the configuration file" << std::endl;
        return 1;
    }
    std::vector<ServerConfig> *serverConfigs = NULL;

    try {
        Parser parser(argv[1]);
        serverConfigs = new std::vector<ServerConfig>(parser.parse());
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        delete serverConfigs;
        return 1;
    }

    std::vector<Webserver*> webservers;
    for (size_t i = 0; i < serverConfigs->size(); ++i) {
        webservers.push_back(
                new Webserver(
                        &((*serverConfigs)[i]),
                        new HttpRequestHandler,
                        new HttpResponseHandler,
                        new CGI
                )
        );
    }

    Monitor monitor(&webservers);
    Signals signals(&monitor);

    monitor.loop();

    for (size_t i = 0; i < webservers.size(); i++) {
        delete webservers[i]->httpRequestHandler;
        delete webservers[i]->httpResponseHandler;
        delete webservers[i]->cgi;
        delete webservers[i];
    }
    delete serverConfigs;
    return 0;
}
