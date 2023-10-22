#include <vector>
#include <csignal>
#include "./Setup/InitialParametersHandler.cpp"
#include "./Request/HttpRequestHandler.cpp"
#include "./Response/HttpResponseHandler.cpp"
#include "./Monitor.cpp"
#include "./Webserver.cpp"

int main() {
    InitialParametersHandler initialParametersHandler1(0);
    HttpRequestHandler httpRequestHandler1;
    HttpResponseHandler httpResponseHandler1;

    InitialParametersHandler initialParametersHandler2(10);
    HttpRequestHandler httpRequestHandler2;
    HttpResponseHandler httpResponseHandler2;


    Webserver webserver1(
            &initialParametersHandler1,
            &httpRequestHandler1,
            &httpResponseHandler1
    );

    Webserver webserver2(
            &initialParametersHandler2,
            &httpRequestHandler2,
            &httpResponseHandler2
    );

    std::vector<Webserver*> webservers;

    webservers.push_back(&webserver1);
    webservers.push_back(&webserver2);

    Monitor monitor(&webservers);

    monitor.loop();

    return 0;
}
