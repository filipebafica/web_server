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
#include "./Interfaces/IServerConfig.hpp"
#include "./Interfaces/IHttpRequestHandler.hpp"
#include "./Interfaces/IHttpResponseHandler.hpp"
#include "./Interfaces/ICGI.hpp"

using std::string;
using std::vector;

class Webserver {
 private:
    IServerConfig* serverConfig;
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    ICGI* cgi;
    vector<int> serverSockets;
    vector<struct sockaddr_in*> serverAddresses;
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
    vector<int> getServerSockets(void) const;
    string getResources(string& method, string& route) const;
    string getErrorPage(int statusCode) const;
    void readRequest(int clientSocket);
    void parseRequest(void);
    bool isResponseAllowed(void) const;
    void setAllowResponse(bool isResponseAllowed);
    const std::map<string, string>& getRequest(void) const;
    void send(int clientSocket);

 private:
    string getContent(std::string path) const;
    void uploadFile(const char* content, const char* fileName) const;
};

#endif //WEBSERVER_
