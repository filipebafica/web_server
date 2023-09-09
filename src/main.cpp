#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <sys/poll.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "./InitialConfig/InitialConfigHandler.cpp"
#include "./Request/HttpRequestHandler.cpp"
#include "./Response/HttpResponseHandler.cpp"

class Webserver {
private:
    IInitialConfigHandler* initialConfigHandler;
    IHttpRequestHandler* httpRequestHandler;
    IHttpResponseHandler* httpResponseHandler;
    int serverSocket;
    struct sockaddr_in serverAddress;
    char buffer[1024];
    std::vector<int> clientSockets;
    std::vector<struct pollfd> pollFds; // Stores the file descriptors for polling


public:
    Webserver(
            IInitialConfigHandler* initialConfigHandler,
            IHttpRequestHandler* httpRequestHandler,
            IHttpResponseHandler* httpResponseHandler
    ) {
        // Inject dependencies
        this->initialConfigHandler = initialConfigHandler;
        this->httpRequestHandler = httpRequestHandler;
        this->httpResponseHandler = httpResponseHandler;

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
        for (std::vector<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
            close(*it);
        }
    }

    void startListening() {
        // Prepare server socket to accept calls
        this->makeServerSocketListen();

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
            this->handleRequests();
        }
    }

private:
    int createSocket() {
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

        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        return socketDescriptor;
    }

    void setupServerAddress() {
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

        this->serverAddress.sin_family = AF_INET;
        this->serverAddress.sin_port = htons(8088);
        this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    }

    void bindServerSocket() {
        /*  Binds the server socket to a specified address.
            This function associates the server socket with the preconfigured
            server address structure for communication.

            If binding fails, an error message is printed, and
            the program may exit with a non-zero status.
        */

        int bindToSocket = bind(
                this->serverSocket,
                (struct sockaddr*)&this->serverAddress,
                sizeof(this->serverAddress)
            );

        if (bindToSocket < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
        }
    }

    void makeServerSocketListen() {
        /*  Marks the socket referred to by sockfd as a passive socket, that is,
            as a socket that will be used to accept incoming connection requests using accept.
        */

        listen(this->serverSocket, 5);
        std::cout << "Server listening on port 8088" << std::endl;
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
            const char* resourcesPath = this->initialConfigHandler->getResourcesPath(
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
                    401,
                    "",
                    "Method not allowed\n"
            );

        } catch (PathNotFoundException& exception) {
            this->httpResponseHandler->responseWriter(
                    clientSocket,
                    404,
                    "",
                    this->getContent(this->initialConfigHandler->getErrorPage()).c_str()
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

    static void signalHandler(int signalNumber) {
        // Handle signals (e.g., SIGINT) for graceful termination
        exit(signalNumber);
    }

    void updatePollFds() {
        /*  Updates the list of file descriptors to be polled.
            This function adds both the server socket and client sockets to
            the list for monitoring input events (POLLIN/POLLOUT).

            - POLLIN: Is used to check if there is data available to be read
            from a file descriptor (such as a socket).

            - POLLOUT: is used to check if you can write data to a file descriptor without blocking.
        */

        // Clear all the fds in the pollFds vector
        this->pollFds.clear();

        // Add the server socket to the list of file descriptors to poll
        struct pollfd serverPollFd;
        serverPollFd.fd = this->serverSocket;
        serverPollFd.events = POLLIN | POLLOUT;
        this->pollFds.push_back(serverPollFd);

        // Add each client socket to the list
        for (std::vector<int>::iterator it = this->clientSockets.begin(); it != this->clientSockets.end(); ++it) {
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

    bool serverSocketHasEvents() {
        // Check if the server socket has events to handle (new client connection)
        return this->pollFds[0].revents & POLLIN;
    }

    void handleNewConnection() {
        /*  Handles a new incoming client connection.
            This function accepts a client connection on the server socket.
            It adds the client socket descriptor to the list of client sockets.
            If an error occurs during connection acceptance, an error message is printed,
            and the function returns.
        */

        socklen_t clientAddressSize = sizeof(this->serverAddress);
        int clientSocket = accept(
                this->serverSocket,
                (struct sockaddr*)&this->serverAddress,
                &clientAddressSize
            );

        if (clientSocket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            return;
        }

        // Add the new client socket to the list
        this->clientSockets.push_back(clientSocket);
    }

    void handleRequests() {
        // Handle requests from existing client sockets
        //starts from 1 because the index 0 is reserved for the server
        for (size_t i = 1; i < this->pollFds.size(); ++i) {
            if (this->pollFds[i].revents & POLLIN) {

                this->readRequest(this->pollFds[i].fd);
                this->processRequest(this->pollFds[i].fd);
                close(this->pollFds[i].fd);

                // Remove the client socket after handling
                this->clientSockets.erase(clientSockets.begin() + i - 1);
            }
        }
    }
};

int main() {
    // Dependencies
    InitialConfigHandler initialConfigHandler;
    HttpRequestHandler httpRequestHandler;
    HttpResponseHandler httpResponseHandler;

    // Create an instance of the Webserver class and start listening
    Webserver webserver(
            &initialConfigHandler,
            &httpRequestHandler,
            &httpResponseHandler
    );
    webserver.startListening();
    return 0;
}
