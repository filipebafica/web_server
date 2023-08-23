#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <sys/poll.h>
#include <vector>

class Webserver {
private:
    int serverSocket;
    struct sockaddr_in serverAddress;
    char buffer[1024];
    std::vector<int> clientSockets;
    std::vector<struct pollfd> pollFds; // Stores the file descriptors for polling

public:
    Webserver() {
        // Create the server socket
        this->serverSocket = this->createSocket();

        // Setup server address
        this->setupServerAddress();

        // Bind server socket to the specified address
        this->bindServerSocket();

        // Set up a signal handler for graceful termination
        std::signal(SIGINT, Webserver::signalHandler);
    }

    ~Webserver() {
        // Close the server socket
        close(this->serverSocket);

        // Close all client sockets
        for (int clientSocket : clientSockets) {
            close(clientSocket);
        }
    }

    void startListening() {
        // Listen for incoming connections on the server socket
        listen(this->serverSocket, 5);
        std::cout << "Server listening on port 8088" << std::endl;

        while (true) {
            // Update the list of file descriptors to poll
            this->updatePollFds();

            // Poll for events on the file descriptors
            if (this->pollEvents() == -1) {
                perror("Error in poll");
                continue;
            }

            // Check if the server socket has events (new client connection)
            if (this->serverSocketHasEvents()) {
                // Accept the new client connection
                this->handleNewConnection();
            }

            // Handle requests from existing clients
            this->handleClientRequests();
        }
    }

private:
    int createSocket() {
        // Create a socket using the IPv4 address family and TCP socket type
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        return socketDescriptor;
    }

    void setupServerAddress() {
        // Set up the server address struct
        this->serverAddress.sin_family = AF_INET;
        this->serverAddress.sin_port = htons(8088);
        this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    }

    void bindServerSocket() {
        // Bind the server socket to the specified address
        if (bind(this->serverSocket, (struct sockaddr*)&this->serverAddress, sizeof(this->serverAddress)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
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

    void processRequest() {
        // Process and display the received request
        std::cout << "Received request:\n" << this->buffer << std::endl;
    }

    void sendResponse(int clientSocket) {
        // Prepare and send a response to the client
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\n\r\nHello, World!\n";
        write(clientSocket, response, std::strlen(response));
    }

    static void signalHandler(int signalNumber) {
        // Handle signals (e.g., SIGINT) for graceful termination
        exit(signalNumber);
    }

    void updatePollFds() {
        // Update the list of file descriptors to poll
        this->pollFds = this->createPollFds();
    }

    int pollEvents() {
        // Poll for events on the file descriptors
        return poll(this->pollFds.data(), this->pollFds.size(), -1);
    }

    bool serverSocketHasEvents() {
        // Check if the server socket has events to handle (new client connection)
        return this->pollFds[0].revents & POLLIN;
    }

    void handleNewConnection() {
        // Accept a new client connection
        socklen_t clientAddressSize = sizeof(this->serverAddress);
        int clientSocket = accept(this->serverSocket, (struct sockaddr*)&this->serverAddress, &clientAddressSize);
        if (clientSocket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
        } else {
            // Add the new client socket to the list
            this->clientSockets.push_back(clientSocket);
        }
    }

    void handleClientRequests() {
        // Handle requests from existing client sockets
        for (size_t i = 1; i < this->pollFds.size(); ++i) {
            if (this->pollFds[i].revents & POLLIN) {
                // Check if the client socket has events to read data
                int clientSocket = this->pollFds[i].fd;
                this->readRequest(clientSocket);
                this->processRequest();
                this->sendResponse(clientSocket);
                close(clientSocket);
                // Remove the client socket after handling
                this->clientSockets.erase(clientSockets.begin() + i - 1);
            }
        }
    }

    std::vector<struct pollfd> createPollFds() {
        // Create a list of pollfd structures for polling
        std::vector<struct pollfd> pollFds;

        // Add the server socket to the list of file descriptors to poll
        struct pollfd serverPollFd;
        serverPollFd.fd = this->serverSocket;
        serverPollFd.events = POLLIN;
        pollFds.push_back(serverPollFd);

        // Add each client socket to the list
        for (std::vector<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
            struct pollfd clientPollFd;
            clientPollFd.fd = *it;
            clientPollFd.events = POLLIN;
            pollFds.push_back(clientPollFd);
        }

        return pollFds;
    }
};

int main() {
    // Create an instance of the Webserver class and start listening
    Webserver webserver;
    webserver.startListening();
    return 0;
}
