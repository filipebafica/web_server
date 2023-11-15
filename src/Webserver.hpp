#ifndef WEBSERVER_
#define WEBSERVER_

#include <iostream>
#include <sstream>
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
    std::map<int, std::vector<char> > clientBuffers;

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
    void setBufferHeaderSize(void);
    void setClientMaxBodySize(void);
    std::vector<int> getServerSockets(void) const;
    Resources getResources(std::string& method, std::string& route) const;
    std::string getErrorPage(int statusCode) const;
    void readRequest(int clientSocket);
    void parseRequest(int clientSocket);
    bool isResponseAllowed(void) const;
    void setAllowResponse(bool isResponseAllowed);
    const std::map<std::string, std::string>& getRequest(void) const;
    void responseWriter(int socket, int statusCode, const char* reasonPhrase, const char* headers, const char* content, size_t contentLength);
    void updateClientBuffers(int clientSocket);
    void send(int clientSocket);
    void handleGET(int clientSocket, std::string& method, std::string& route, std::string& contentType, const Resources& resources);
    void handlePOST(int clientSocket, std::string& method, std::string& route, std::string& contentType);
    void handleDELETE(int clientSocket, const Resources& resources);
    std::vector<char> getContent(std::string path) const;
    std::string getDirectoryFiles(std::string path) const;
};

#endif //WEBSERVER_
