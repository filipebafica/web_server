#include <csignal>
#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <list>
#include "./Webserver.cpp"

class Monitor {
private:
    std::vector<Webserver*>* webservers;
    std::list<int> clientSockets;
    std::vector<struct pollfd> pollFds;
    std::map<int, Webserver*> fdToWebserverMap;
    int numberOfServers;

    static bool monitorRunning;

public:
    Monitor(
            std::vector<Webserver*>* webservers
    ) {
        // Injects dependencies
        this->webservers = webservers;

        // Initialize attributes
        this->numberOfServers = 0;
        this->monitorRunning = true;
    };
    
    ~Monitor() {
    }

    static void handleSignals(int signal) {
        Monitor::monitorRunning = false;
    }

    void loop() {
        signal(SIGINT, Monitor::handleSignals);
        signal(SIGTERM, Monitor::handleSignals);

        this->updatePollFdsVectorWithServerSockets();
        while (this->monitorRunning) {
            this->updatePollFdsVectorWithClientSockets();

            if (this->pollEvents() == -1) {
                perror("Error in poll");
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
            for (int i = this->numberOfServers; i < this->pollFds.size(); ++i) {
                int clientSocket = this->pollFds[i].fd;
                Webserver* webserver = this->findWebserver(clientSocket);

                // Handles request side
                if (this->pollFds[i].revents & POLLIN) {
                    webserver->readRequest(clientSocket);
                    webserver->parseRequest();
                    webserver->setAllowResponse(true);
                }

                // Handles response side
                if ((this->pollFds[i].revents & POLLOUT) && webserver->isResponseAllowed()) {
                    webserver->send(clientSocket);
                    webserver->setAllowResponse(false);
                    this->clientSockets.remove(clientSocket);
                    this->fdToWebserverMap.erase(clientSocket);
                    close(clientSocket);
                }
            }
        }
    }

private:
    void updatePollFdsVectorWithServerSockets() {
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

        // Clear all the fds in the pollFds vector
//        this->pollFds.clear();

        for (int i = 0; i < this->webservers->size(); ++i) {
            Webserver* server = (*this->webservers)[i];
            std::vector<int> serverSockets = server->getServerSockets();

            for (int j = 0; j < serverSockets.size(); ++j) {
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

    void updatePollFdsVectorWithClientSockets() {
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

    int pollEvents() {
        // Poll for events on the file descriptors
        return poll(this->pollFds.data(), this->pollFds.size(), -1);
    }

    bool serverSocketHasEvents(struct pollfd& fd) {
        // Check if the server socket has events to handle (new client connection)
        return fd.revents & POLLIN;
    }

    void updateClientSocketsVector(int serverSocket) {
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

    Webserver* findWebserver(int fileDescriptor) const {
        return this->fdToWebserverMap.find(fileDescriptor)->second;
    }
};

bool Monitor::monitorRunning = true;
