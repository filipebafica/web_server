#ifndef WEBSERVER_
#define WEBSERVER_

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <IServerConfig.hpp>
#include <IHttpRequestHandler.hpp>
#include <IHttpResponseHandler.hpp>
#include <ICGI.hpp>
#include <CGIRequest.hpp>
#include <Exceptions.hpp>

class Webserver {
 private:
    std::vector<int> serverSockets;
    std::vector<struct sockaddr_in*> serverAddresses;
    bool allowResponse;
    int defaultBufferHeaderSize;
    int clientMaxBodySize;
    int bufferSize;
    char* buffer;

 public:
    IServerConfig* serverConfig;
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    ICGI* cgi;

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
    void setClientMaxBodySize(void);
    void setBufferSize(void);
    void setBuffer(void);
    std::vector<int> getServerSockets(void) const;
    Resources getResources(std::string& method, std::string& route) const;
    std::string getErrorPage(int statusCode) const;
    void readRequest(int clientSocket);
    void parseRequest(void);
    bool isResponseAllowed(void) const;
    void setAllowResponse(bool isResponseAllowed);
    const std::map<std::string, std::string>& getRequest(void) const;
    void responseWriter(int socket, int statusCode, const char* headers = "", const char* content = "");
    void send(int clientSocket);
    void handleGET(int clientSocket, std::string& method, std::string& route, std::string& contentType, const Resources& resources) const;
    void handlePOST(int clientSocket, std::string& method, std::string& route, std::string& contentType) const;
    void handleDELETE(int clientSocket);

 private:
    std::string getContent(std::string path) const;
    std::string getDirectoryFiles(std::string path) const;
};

#endif //WEBSERVER_
