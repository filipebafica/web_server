#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include "../Interfaces/IServerSetupHandler.hpp"

class ServerSetupHandler : public IServerSetupHandler {
private:
    int serverSocket;
    struct sockaddr_in* serverAddress;

public:
    ServerSetupHandler() {
        this->serverAddress = new struct sockaddr_in;
    }

    ~ServerSetupHandler() {
        delete this->serverAddress;
        close(this->serverSocket);
    }

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

        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        this->serverSocket = socketDescriptor;
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

        this->serverAddress->sin_family = AF_INET;
        this->serverAddress->sin_port = htons(8088);
        this->serverAddress->sin_addr.s_addr = INADDR_ANY;
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
                (struct sockaddr*)this->serverAddress,
                sizeof(*this->serverAddress)
        );

        if (bindToSocket < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
        }
    }

    void startListening() const {
        /*  Marks the socket a passive socket, that is,
            as a socket that will be used to accept incoming connection requests using accept.
        */

        listen(this->serverSocket, 5);
        std::cout << "Server listening on port 8088" << std::endl;
    }

    int getServerSocket() const {
        return this->serverSocket;
    }

    struct sockaddr_in* getServerAddress() const {
        return this->serverAddress;
    }
};