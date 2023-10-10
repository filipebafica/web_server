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
#include "./Interfaces/IInitialParametersHandler.hpp"
#include "./Interfaces/IHttpRequestHandler.hpp"
#include "./Interfaces/IHttpResponseHandler.hpp"

class Webserver {
private:
    IInitialParametersHandler* initialParametersHandler;
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    std::vector<int> serverSockets;
    std::vector<struct sockaddr_in*> serverAddresses;
    bool allowResponse;

public:
    Webserver(
            IInitialParametersHandler* initialParametersHandler,
            IHttpRequestHandler* httpRequestHandler,
            IHttpResponseHandler* httpResponseHandler
    ) {
        // Inject dependencies
        this->initialParametersHandler = initialParametersHandler;
        this->httpRequestHandler = httpRequestHandler;
        this->httpResponseHandler = httpResponseHandler;

        // Setups server
        this->setupAddress();
        this->createSocket();
        this->bindServerSocket();
        this->startListening();

        // Initialize attributes
        this->allowResponse = false;
    }

    ~Webserver() {}
    void createSocket() {
        /*  Creates a socket using the IPv4 address family and TCP socket type.
            Returns the socket descriptor if the creation is successful.
            If an error occurs during socket creation, an error message is printed,
            and the program may exit with a non-zero status.

            - SOCKET:
              - A socket is a programming interface enabling communication between processes
              on different devices over networks. It functions as an endpoint for data exchange
              across networks, whether local or Internet-based

            - AF_INET:
              - AF stands for Address Family.
              - AF_INET corresponds to the IPv4 address family.

            - SOCK_STREAM:
              - SOCK stands for Socket Type.
              - SOCK_STREAM corresponds to a stream-oriented socket (guarantee the order).
        */
        std::vector<int> ports = this->initialParametersHandler->getListeningPorts();

        for (size_t i = 0; i < ports.size(); ++i) {
            int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
            if (socketDescriptor < 0) {
                std::cerr << "Error creating socket" << std::endl;
                exit(1);
            }
            this->serverSockets.push_back(socketDescriptor);
        }
    }

    void setupAddress() {
        /*  Sets up the server address structure.
            This function configures the server address struct for
            subsequent binding to the server socket.

            - SIN_FAMILY: Address Family.
              AF_INET corresponds to the IPv4 address family.

            - SIN_PORT: Port number in network byte order.
              htons() converts the port to network byte order.
                - Host Byte Order: This refers to the byte order used by the computer's architecture.
                For example, on x86-based systems, the host byte order is typically little-endian,
                meaning the least significant byte comes first.

                - Network Byte Order: This is a standardized byte order used in network communication.
                It's big-endian, meaning the most significant byte comes first.

            - SIN_ADDR.S_ADDR: IP address of the host.
              INADDR_ANY allows binding to all available interfaces.
        */
        std::vector<int> ports = this->initialParametersHandler->getListeningPorts();
        for (size_t i = 0; i < ports.size() ; ++i) {
            struct sockaddr_in* address = new struct sockaddr_in;

            address->sin_family = AF_INET;
            address->sin_port = htons(ports[i]);
            address->sin_addr.s_addr = INADDR_ANY;

            this->serverAddresses.push_back(address);
        }
    }

    void bindServerSocket() {
        /*  Binds the servers sockets to a specified address.
            This function associates the server socket with the preconfigured
            server address structure for communication.

            If binding fails, an error message is printed, and
            the program may exit with a non-zero status.
        */
        for (size_t i = 0; i < this->serverSockets.size(); ++i) {
            int bindToSocket = bind(
                    this->serverSockets[i],
                    (struct sockaddr*)this->serverAddresses[i],
                    sizeof(*this->serverAddresses[i])
            );

            if (bindToSocket < 0) {
                std::cerr << "Error binding socket" << std::endl;
                exit(1);
            }
        }
    }

    void startListening() const {
        /*  Marks the socket a passive socket, that is,
            as a socket that will be used to accept incoming connection requests using accept.
        */
        for (size_t i = 0; i < this->serverSockets.size(); ++i) {
            listen(this->serverSockets[i], 1024);
        }
    }

    std::vector<int> getServerSockets() const {
        return this->serverSockets;
    }

    const char* getResources(std::string& method, std::string& route) const {
        return this->initialParametersHandler->getResources(method, route);
    }

    const char* getUploadPath(std::string& route) const {
        return this->initialParametersHandler->getUploadPath(route);
    }

    const char* getErrorPage() const {
        return this->initialParametersHandler->getErrorPage();
    }

    void readRequest(int clientSocket) {
        this->httpRequestHandler->readRequest(clientSocket);
    }

    void parseRequest() {
        this->httpRequestHandler->parseRequest();
    }

    bool isResponseAllowed() const {
        return this->allowResponse;
    }

    void setAllowResponse(bool isResponseAllowed) {
        this->allowResponse = isResponseAllowed;
    }

    const std::map<std::string, std::string>& getHeaders() const {
        return this->httpRequestHandler->getHeaders();
    }

    void send(int clientSocket) {
        try {
            std::string method = this->httpRequestHandler->getHeader("Method");
            std::string route = this->httpRequestHandler->getHeader("Route");
            const char* resources = this->initialParametersHandler->getResources(
                    method,
                    route
            );

            if (std::string("GET") == method) {
                this->httpResponseHandler->send(
                        clientSocket,
                        200,
                        "",
                        this->getContent(resources).c_str()
                );

                return;
            }

            if (std::string("POST") == method) {
                this->uploadFile(
                        this->getContent(resources).c_str(),
                        this->getUploadPath(route)
                );

                this->httpResponseHandler->send(
                        clientSocket,
                        201,
                        "",
                        "file was uploaded successfully"
                );

                return;
            }

            if (std::string("DELETE") == method) {
                this->httpResponseHandler->send(
                        clientSocket,
                        200,
                        "",
                        "DELETE has been made"
                );

                return;
            }

        } catch (MethodNotAllowedException& exception) {
            this->httpResponseHandler->send(
                    clientSocket,
                    405,
                    "",
                    "Method not allowed\n"
            );

        } catch (RouteNotFoundException& exception) {
            this->httpResponseHandler->send(
                    clientSocket,
                    404,
                    "",
                    this->getContent(this->getErrorPage()).c_str()
            );
        }
    }

private:
    std::string getContent(const char* path) const {
        std::ifstream file;
        std::string content;
        std::string line;

        file.open(path, std::ios::binary);
        while(getline(file, line)) {
            content += line;
        }
        file.close();

        return content;
    }

    void uploadFile(const char* content, const char* fileName) const {
        FILE* destinationFile = fopen(fileName, "w+");
        fwrite(content, strlen(content), strlen(content), destinationFile);
        fclose(destinationFile);
    }

};

#endif //WEBSERVER_