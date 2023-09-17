#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include "./Webserver.cpp"
#include "./Interfaces/IHttpRequestHandler.hpp"
#include "./Interfaces/IHttpResponseHandler.hpp"

class Monitor {
private:
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    std::vector<Webserver*>* webservers;
    std::vector<int> clientSockets;
    std::vector<struct pollfd> pollFds;
    std::map<int, Webserver*> fdToWebServerMap;
    size_t numberOfServerFds;
    char buffer[1024];

public:
    Monitor(
            IHttpRequestHandler* httpRequestHandler,
            IHttpResponseHandler* httpResponseHandler,
            std::vector<Webserver*>* webservers
    ) {
        // Injects dependencies
        this->httpRequestHandler = httpRequestHandler;
        this->httpResponseHandler = httpResponseHandler;
        this->webservers = webservers;
    };
    
    ~Monitor() {
        // Close all client sockets
        for (size_t i = 0; i < this->clientSockets.size(); ++i) {
            close(this->clientSockets[i]);
        }
    }

    void loop() {
        while (true) {
            this->updatePollFdsWithServerSockets();
            this->updatePollFdsWithClientSockets();

            if (this->pollEvents() == -1) {
                perror("Error in poll");
                continue;
            }

            for (size_t i = 0; i < this->pollFds.size(); ++i) {
                // Check if the server socket has events (new client connection)
                if (this->serverSocketHasEvents(this->pollFds[i])) {
                    // Accept the new client connection
                    this->handleNewConnection(this->pollFds[i].fd);
                }
            }

            // Handle requests from existing clients
            this->handleRequests();
        }
    }

private:
    void updatePollFdsWithServerSockets() {
        /*  Updates the list of file descriptors to be polled.
            This function adds the server sockets to
            the list for monitoring  events (POLLIN/POLLOUT).

            - POLLIN: Is used to check if there is data available to be read
            from a file descriptor (such as a socket).

            - POLLOUT: is used to check if you can write data to a file descriptor
            without blocking.
        */

        // Clear all the fds in the pollFds vector
        this->pollFds.clear();


        for (size_t i = 0; i < this->webservers->size(); ++i) {
            Webserver* server = (*this->webservers)[i];
            std::vector<int> serverSockets = server->getServerSockets();

            for (size_t j = 0; j < serverSockets.size(); ++j) {
                struct pollfd serverPollFd;
                serverPollFd.fd = serverSockets[j];
                serverPollFd.events = POLLIN | POLLOUT;
                this->pollFds.push_back(serverPollFd);

                this->numberOfServerFds += 1;
                this->fdToWebServerMap.insert(
                        std::pair<int, Webserver*>(serverSockets[j], server)
                );
            }
        }
    }

    void updatePollFdsWithClientSockets() {
        /*  Add each client socket to the list to be polled.
            From the first iteration of the main loop
            there will be no client sockets to add to poll
            because there is no connection at this point yet
         */
        for (size_t i = 0; i < this->clientSockets.size(); ++i) {
            struct pollfd clientPollFd;
            clientPollFd.fd = this->clientSockets[i];
            clientPollFd.events = POLLIN | POLLOUT;
            this->pollFds.push_back(clientPollFd);
        }
    }

    int pollEvents() {
        // Poll for events on the file descriptors
        return poll(this->pollFds.data(), this->pollFds.size(), -1);
    }

    void handleRequests() {
        // Handle requests from existing client sockets
        for (size_t i = this->numberOfServerFds; i < this->pollFds.size(); ++i) {
            if (this->pollFds[i].revents & POLLIN) {

                this->readRequest(this->pollFds[i].fd);
                this->processRequest(this->pollFds[i].fd);
                close(this->pollFds[i].fd);

                // Remove the client socket after handling
                this->clientSockets.erase(clientSockets.begin() + i - this->numberOfServerFds);
            }
        }
    }

    void readRequest(int clientSocket) {
        // Clear the buffer and read data from the client socket
        std::memset(this->buffer, 0, sizeof(this->buffer));
        int bytesRead = read(clientSocket, this->buffer, sizeof(this->buffer));

        if (bytesRead < 0) {
            std::cerr << "Error reading request" << std::endl;
        } else if (bytesRead == 0) {
            std::cerr << "Client disconnected" << std::endl;
        }
    }

    void processRequest(int clientSocket) {
        // Parses received request
        this->httpRequestHandler->parse(this->buffer);
        std::map<std::string, std::string> headers = this->httpRequestHandler->getHeaders();

        try {
            const char* resourcesPath = this->fdToWebServerMap.find(clientSocket)->second
                    ->getInitialParametersHandler()
                    ->getResourcesPath(
                        headers["Method"],
                        headers["Path"]
                    );

            this->httpResponseHandler->responseWriter(
                    clientSocket,
                    200,
                    "",
                    this->getContent(resourcesPath).c_str()
            );

        } catch (MethodNotAllowedException& exception) {
            this->httpResponseHandler->responseWriter(
                    clientSocket,
                    405,
                    "",
                    "Method not allowed\n"
            );

        } catch (PathNotFoundException& exception) {
            this->httpResponseHandler->responseWriter(
                    clientSocket,
                    404,
                    "",
                    this->getContent(
                            this->fdToWebServerMap.find(clientSocket)->second
                            ->getInitialParametersHandler()
                            ->getErrorPage()
                    ).c_str()
            );
        }

        // Displays the received request
        std::cout << "Received request:\n" << this->buffer << std::endl;
    }

    std::string getContent(const char* path) {
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

    bool serverSocketHasEvents(struct pollfd& fd) {
        // Check if the server socket has events to handle (new client connection)
        return fd.revents & POLLIN;
    }

    void handleNewConnection(int serverSocket) {
        /*  Handles a new incoming client connection.
            This function accepts a client connection on the server socket.
            It adds the client socket descriptor to the list of client sockets.
            If an error occurs during connection acceptance, an error message is printed,
            and the function returns.
        */

        socklen_t clientAddressSize = sizeof(struct sockaddr*);
        int clientSocket = accept(
                serverSocket,
                NULL,
                &clientAddressSize
        );

        if (clientSocket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            return;
        }

        // Add the new client socket to the list
        this->clientSockets.push_back(clientSocket);
        this->fdToWebServerMap.insert(
                std::pair<int, Webserver*>(
                        clientSocket,
                        this->fdToWebServerMap.find(serverSocket)->second
                )
        );
    }
};