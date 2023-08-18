#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

class Webserver {
private:
    int serverSocket;
    struct sockaddr_in serverAddress;
    char buffer[1024];

public:
    Webserver() {
        this->serverSocket = this->createSocket();
        this->setupServerAddress();
        this->bindServerSocket();
    }

    ~Webserver() {
        close(this->serverSocket);
    }

    void startListening() {
        listen(this->serverSocket, 5);
        std::cout << "Server listening on port 8088" << std::endl;

        while (true) {
            socklen_t clientAddressSize = sizeof(this->serverAddress);
            int clientSocket = accept(this->serverSocket, (struct sockaddr*)&this->serverAddress, &clientAddressSize);

            if (clientSocket < 0) {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }

            this->readRequest(clientSocket);
            this->processRequest();
            this->sendResponse(clientSocket);

            close(clientSocket);
        }
    }

private:
    int createSocket() {
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        return socketDescriptor;
    }

    void setupServerAddress() {
        this->serverAddress.sin_family = AF_INET;
        this->serverAddress.sin_port = htons(8088);
        this->serverAddress.sin_addr.s_addr = INADDR_ANY;
    }

    void bindServerSocket() {
        if (bind(this->serverSocket, (struct sockaddr*)&this->serverAddress, sizeof(this->serverAddress)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
        }
    }

    void readRequest(int clientSocket) {
        std::memset(this->buffer, 0, sizeof(this->buffer));
        int bytesRead = read(clientSocket, this->buffer, sizeof(this->buffer));
        if (bytesRead < 0) {
            std::cerr << "Error reading request" << std::endl;
        } else if (bytesRead == 0) {
            std::cerr << "Client disconnected" << std::endl;
        }
    }

    void processRequest() {
        std::cout << "Received request:\n" << this->buffer << std::endl;
    }

    void sendResponse(int clientSocket) {
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\n\r\nHello, World!\n";
        write(clientSocket, response, std::strlen(response));
    }
};

int main() {
    Webserver webserver;
    webserver.startListening();
    return 0;
}
