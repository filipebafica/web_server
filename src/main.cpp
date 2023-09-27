#include <vector>
#include "./Setup/InitialParametersHandler.cpp"
#include "./Request/HttpRequestHandler.cpp"
#include "./Response/HttpResponseHandler.cpp"
#include "./Monitor.cpp"

int main() {
    HttpRequestHandler httpRequestHandler;
    HttpResponseHandler httpResponseHandler;

    InitialParametersHandler initialParametersHandler1(0);
    InitialParametersHandler initialParametersHandler2(10);

    Webserver webserver1(&initialParametersHandler1);
    Webserver webserver2(&initialParametersHandler2);

    std::vector<Webserver*> webservers;

    webservers.push_back(&webserver1);
    webservers.push_back(&webserver2);

    Monitor monitor(
            &httpRequestHandler,
            &httpResponseHandler,
            &webservers
    );

    monitor.loop();

    return 0;
}
