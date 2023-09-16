#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <sys/poll.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "./Setup/ServerSetupHandler.cpp"
#include "./Setup/InitialParametersHandler.cpp"
#include "./Request/HttpRequestHandler.cpp"
#include "./Response/HttpResponseHandler.cpp"

//class Webserver {
//private:
//    IServerSetupHandler* serverSetupHandler;
//    IInitialParametersHandler* initialParametersHandler;
//    IHttpRequestHandler* httpRequestHandler;
//    IHttpResponseHandler* httpResponseHandler;
//    char buffer[1024];
//    std::vector<int> clientSockets;
//    std::vector<struct pollfd> pollFds; // Stores the file descriptors for polling
//
//
//public:
//    Webserver(
//            IInitialParametersHandler* initialParametersHandler,
//            IServerSetupHandler* serverSetupHandler,
//            IHttpRequestHandler* httpRequestHandler,
//            IHttpResponseHandler* httpResponseHandler
//    ) {
//        // Inject dependencies
//        this->initialParametersHandler = initialParametersHandler;
//        this->serverSetupHandler = serverSetupHandler;
//        this->httpRequestHandler = httpRequestHandler;
//        this->httpResponseHandler = httpResponseHandler;
//
//        // Setups server
//        this->serverSetupHandler->createSocket();
//        this->serverSetupHandler->setupAddress();
//        this->serverSetupHandler->bindServerSocket();
//
//        // Set up a signal handler for graceful termination
//        std::signal(SIGINT, Webserver::signalHandler);
//    }
//
//    ~Webserver() {
//        // Close all client sockets
//        for (std::vector<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
//            close(*it);
//        }
//    }
//
//    void startListening() {
//        // Prepare server socket to accept calls
//        this->serverSetupHandler->startListening();
//
//        while (true) {
//            // Update the list of file descriptors to poll
//            this->updatePollFds();
//
//            // Poll for events on the file descriptors
//            if (this->pollEvents() == -1) {
//                perror("Error in poll");
//                continue;
//            }
//
//            // Check if the server socket has events (new client connection)
//            if (this->serverSocketHasEvents()) {
//                // Accept the new client connection
//                this->handleNewConnection();
//            }
//
//            // Handle requests from existing clients
//            this->handleRequests();
//        }
//    }
//
//private:
//    void readRequest(int clientSocket) {
//        // Clear the buffer and read data from the client socket
//        std::memset(this->buffer, 0, sizeof(this->buffer));
//        int bytesRead = read(clientSocket, this->buffer, sizeof(this->buffer));
//
//        if (bytesRead < 0) {
//            std::cerr << "Error reading request" << std::endl;
//        } else if (bytesRead == 0) {
//            std::cerr << "Client disconnected" << std::endl;
//        }
//    }
//
//    void processRequest(int clientSocket) {
//        // Parses received request
//        this->httpRequestHandler->parse(this->buffer);
//        std::map<std::string, std::string> headers = this->httpRequestHandler->getHeaders();
//
//        try {
//            const char* resourcesPath = this->initialParametersHandler->getResourcesPath(
//                    headers["Method"],
//                    headers["Path"]
//            );
//
//            this->httpResponseHandler->responseWriter(
//                    clientSocket,
//                    200,
//                    "",
//                    this->getContent(resourcesPath).c_str()
//            );
//
//        } catch (MethodNotAllowedException& exception) {
//            this->httpResponseHandler->responseWriter(
//                    clientSocket,
//                    405,
//                    "",
//                    "Method not allowed\n"
//            );
//
//        } catch (PathNotFoundException& exception) {
//            this->httpResponseHandler->responseWriter(
//                    clientSocket,
//                    404,
//                    "",
//                    this->getContent(this->initialParametersHandler->getErrorPage()).c_str()
//            );
//        }
//
//        // Displays the received request
//        std::cout << "Received request:\n" << this->buffer << std::endl;
//    }
//
//    std::string getContent(const char* path) {
//        std::ifstream file;
//        std::string content;
//        std::string line;
//
//        file.open(path, std::ios::binary);
//        while(getline(file, line)) {
//            content += line;
//        }
//        file.close();
//
//        return content;
//    }
//
//    static void signalHandler(int signalNumber) {
//        // Handle signals (e.g., SIGINT) for graceful termination
//        exit(signalNumber);
//    }
//
//    void updatePollFds() {
//        /*  Updates the list of file descriptors to be polled.
//            This function adds both the server socket and client sockets to
//            the list for monitoring input events (POLLIN/POLLOUT).
//
//            - POLLIN: Is used to check if there is data available to be read
//            from a file descriptor (such as a socket).
//
//            - POLLOUT: is used to check if you can write data to a file descriptor without blocking.
//        */
//
//        // Clear all the fds in the pollFds vector
//        this->pollFds.clear();
//
//        // Add the server socket to the list of file descriptors to poll
//        struct pollfd serverPollFd;
//        serverPollFd.fd = this->serverSetupHandler->getServerSocket();
//        serverPollFd.events = POLLIN | POLLOUT;
//        this->pollFds.push_back(serverPollFd);
//
//        // Add each client socket to the list
//        // From the first iteration of the main loop
//        // there will be no client sockets to add to poll
//        // because there is no connection at this point yet
//        for (std::vector<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
//            struct pollfd clientPollFd;
//            clientPollFd.fd = *it;
//            clientPollFd.events = POLLIN | POLLOUT;
//            this->pollFds.push_back(clientPollFd);
//        }
//    }
//
//    int pollEvents() {
//        // Poll for events on the file descriptors
//        return poll(this->pollFds.data(), this->pollFds.size(), -1);
//    }
//
//    bool serverSocketHasEvents() {
//        // Check if the server socket has events to handle (new client connection)
//        return this->pollFds[0].revents & POLLIN;
//    }
//
//    void handleNewConnection() {
//        /*  Handles a new incoming client connection.
//            This function accepts a client connection on the server socket.
//            It adds the client socket descriptor to the list of client sockets.
//            If an error occurs during connection acceptance, an error message is printed,
//            and the function returns.
//        */
//
//        socklen_t clientAddressSize = sizeof(*this->serverSetupHandler->getServerAddress());
//        int clientSocket = accept(
//                this->serverSetupHandler->getServerSocket(),
//                (struct sockaddr*)this->serverSetupHandler->getServerAddress(),
//                &clientAddressSize
//        );
//
//        if (clientSocket < 0) {
//            std::cerr << "Error accepting connection" << std::endl;
//            return;
//        }
//
//        // Add the new client socket to the list
//        this->clientSockets.push_back(clientSocket);
//    }
//
//    void handleRequests() {
//        // Handle requests from existing client sockets
//        //starts from 1 because the index 0 is reserved for the server
//        for (size_t i = 1; i < this->pollFds.size(); ++i) {
//            if (this->pollFds[i].revents & POLLIN) {
//
//                this->readRequest(this->pollFds[i].fd);
//                this->processRequest(this->pollFds[i].fd);
//                close(this->pollFds[i].fd);
//
//                // Remove the client socket after handling
//                this->clientSockets.erase(clientSockets.begin() + i - 1);
//            }
//        }
//    }
//};
//
//int main() {
//    // Dependencies
//    InitialParametersHandler initialParametersHandler;
//    ServerSetupHandler serverSetupHandler;
//    HttpRequestHandler httpRequestHandler;
//    HttpResponseHandler httpResponseHandler;
//
//    // Create an instance of the Webserver class and start listening
//    Webserver webserver(
//            &initialParametersHandler,
//            &serverSetupHandler,
//            &httpRequestHandler,
//            &httpResponseHandler
//    );
//    webserver.startListening();
//    return 0;
//}
