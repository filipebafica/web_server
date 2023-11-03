#ifndef WEBSERVER_
#define WEBSERVER_

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <netinet/in.h>
#include <sys/socket.h>

#include <IServerConfig.hpp>
#include <IHttpRequestHandler.hpp>
#include <IHttpResponseHandler.hpp>
#include <ICGI.hpp>
#include <CGIRequest.hpp>

class Webserver {
 private:
    IServerConfig* serverConfig;
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    ICGI* cgi;
    std::vector<int> serverSockets;
    std::vector<struct sockaddr_in*> serverAddresses;
    bool allowResponse;

 public:
    Webserver(
            IServerConfig* serverConfig,
            IHttpRequestHandler* httpRequestHandler,
            IHttpResponseHandler* httpResponseHandler,
            ICGI* cgi
    );

    ~Webserver();

    void createSocket(void);
    void setupAddress(void);
    void bindServerSocket(void);
    void startListening(void) const;
    std::vector<int> getServerSockets(void) const;
    std::string getResources(std::string& method, std::string& route) const;
    std::string getErrorPage(int statusCode) const;
    void readRequest(int clientSocket);
    void parseRequest(void);
    bool isResponseAllowed(void) const;
    void setAllowResponse(bool isResponseAllowed);
    const std::map<std::string, std::string>& getRequest(void) const;
    void send(int clientSocket);

 private:
    std::string getContent(std::string path) const;
    void uploadFile(const char* content, const char* fileName) const;
};

#endif //WEBSERVER_
