#include <Monitor.hpp>
#include <fcntl.h>

Monitor::Monitor(std::vector<Webserver*>* webservers) {
    // Injects dependencies
    this->webservers = webservers;

    // Initialize attributes
    this->numberOfServers = 0;
    this->monitorRunning = true;
};

Monitor::~Monitor() {}

void Monitor::loop(void) {
    this->updatePollFdsVectorWithServerSockets();
    while (this->monitorRunning) {
        this->updatePollFdsVectorWithClientSockets();

        if (this->pollEvents() == -1) {
            std::cerr << "Error in poll" << std::endl;
            continue;
        }

        // Handles new clients connecting
        for (int i = 0; i < this->numberOfServers; ++i) {
            // Check if the server socket has events (new client connection)
            if (this->serverSocketHasEvents(this->pollFds[i])) {
                // Accept the new client connection
                this->updateClientSocketsVector(this->pollFds[i].fd);
            }
        }

        // Handles requests from existing clients
        for (size_t i = this->numberOfServers; i < this->pollFds.size(); ++i) {
            int clientSocket = this->pollFds[i].fd;
            Webserver* webserver = this->findWebserver(clientSocket);

            try {
                // Handles request side
                if (this->pollFds[i].revents & POLLIN) {
                    webserver->updateClientBuffers(clientSocket);
                    webserver->readRequest(clientSocket);
                    webserver->setAllowResponse(true);
                }
                // Handles response side
                else if ((this->pollFds[i].revents & POLLOUT) && webserver->isResponseAllowed()) {
                    webserver->parseRequest(clientSocket);
                    webserver->send(clientSocket);
                    webserver->setAllowResponse(false);
                    this->clean(clientSocket);
                }
            } catch (ServerResponseException& serverException) {
                std::vector<char> content = webserver->getContent(
                        webserver->getErrorPage(
                                serverException.getStatus()
                                )
                        );

                webserver->responseWriter(
                        clientSocket,
                        serverException.getStatus(),
                        serverException.getReasonPhrase(),
                        "content-type:text/html",
                        content.data(),
                        content.size()
                );
                webserver->setAllowResponse(false);
                this->clean(clientSocket);
            } catch (std::exception& exception) {
                webserver->responseWriter(
                        clientSocket,
                        500,
                        "Internal Server Error",
                        "content-type:text",
                        "Internal Server Error",
                        22
                );
                webserver->setAllowResponse(false);
                this->clean(clientSocket);
            }
        }
    }
}

void Monitor::stopLoop(void) {
    this->monitorRunning = false;
}

void Monitor::updatePollFdsVectorWithServerSockets(void) {
    /*  Updates the list of file descriptors to be polled.
        This function adds the server sockets to
        the list for monitoring  events (POLLIN/POLLOUT).

        - POLLIN: Is used to check if there is data available to be read
        from a file descriptor (such as a socket).

        - POLLOUT: is used to check if you can write data to a file descriptor
        without blocking.
    */
    // Reset number of servers
    this->numberOfServers = 0;

    for (size_t i = 0; i < this->webservers->size(); ++i) {
        Webserver* server = (*this->webservers)[i];
        std::vector<int> serverSockets = server->getServerSockets();

        for (size_t j = 0; j < serverSockets.size(); ++j) {
            struct pollfd serverPollFd;
            serverPollFd.fd = serverSockets[j];
            serverPollFd.events = POLLIN | POLLOUT;
            this->pollFds.push_back(serverPollFd);

            this->numberOfServers += 1;
            this->fdToWebserverMap.insert(
                    std::pair<int, Webserver*>(serverSockets[j], server)
            );
        }
    }
}

void Monitor::updatePollFdsVectorWithClientSockets(void) {
    /*  Add each client socket to the list to be polled.
        From the first iteration of the main loop
        there will be no client sockets to add to poll
        because there is no connection at this point yet
        */

    // Remove all the previous client sockets from the pollFds
    this->pollFds.erase(this->pollFds.begin() + this->numberOfServers, this->pollFds.end());

    for (std::list<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
        struct pollfd clientPollFd;
        clientPollFd.fd = *it;
        clientPollFd.events = POLLIN | POLLOUT;
        this->pollFds.push_back(clientPollFd);
    }
}

int Monitor::pollEvents(void) {
    // Poll for events on the file descriptors
    return poll(this->pollFds.data(), this->pollFds.size(), -1);
}

bool Monitor::serverSocketHasEvents(struct pollfd& fd) {
    // Check if the server socket has events to handle (new client connection)
    return fd.revents & POLLIN;
}

void Monitor::updateClientSocketsVector(int serverSocket) {
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
    this->fdToWebserverMap.insert(
            std::pair<int, Webserver*>(
                    clientSocket,
                    this->findWebserver(serverSocket)
            )
    );
}

Webserver* Monitor::findWebserver(int fileDescriptor) const {
    return this->fdToWebserverMap.find(fileDescriptor)->second;
}

void Monitor::clean(int clientSocket) {
    this->clientSockets.remove(clientSocket);
    this->fdToWebserverMap.erase(clientSocket);
    close(clientSocket);
}
