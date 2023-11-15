#include <Webserver.hpp>

#define LIST_DIR_SCRIPT "./static/listDir.php"

Webserver::Webserver(
        IServerConfig* serverConfig,
        IHttpRequestHandler* httpRequestHandler,
        IHttpResponseHandler* httpResponseHandler,
        ICGI* cgi
) {
    // Injects dependencies
    this->serverConfig = serverConfig;
    this->httpRequestHandler = httpRequestHandler;
    this->httpResponseHandler = httpResponseHandler;
    this->cgi = cgi;

    // Initializes attributes
    this->allowResponse = false;

    // Setups server
    this->setupAddress();
    this->createSocket();
    this->bindServerSocket();
    this->startListening();
    this->setBufferHeaderSize();
    this->setClientMaxBodySize();
}

Webserver::~Webserver() {
    for (size_t i = 0; i < this->serverAddresses.size(); i++) {
        delete this->serverAddresses[i];
    }
    /* Closing socket descriptors */
    for (size_t i = 0; i < this->serverSockets.size(); i++) {
        close(this->serverSockets[i]);
    }
}

void Webserver::createSocket(void) {
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
    std::vector<int> ports = this->serverConfig->getListeningPorts();
    int option = 1;

    for (size_t i = 0; i < ports.size(); ++i) {
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

        /** SO_REUSEADDR socket option avoids 'address already in use'
         * error when binding the socket to the server address. */
        setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (socketDescriptor < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
        this->serverSockets.push_back(socketDescriptor);
    }
}

void Webserver::setupAddress(void) {
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
    std::vector<int> ports = this->serverConfig->getListeningPorts();
    for (size_t i = 0; i < ports.size() ; ++i) {
        struct sockaddr_in* address = new struct sockaddr_in;

        address->sin_family = AF_INET;
        address->sin_port = htons(ports[i]);
        address->sin_addr.s_addr = INADDR_ANY;

        this->serverAddresses.push_back(address);
    }
}

void Webserver::bindServerSocket(void) {
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

void Webserver::startListening(void) const {
    /*  Marks the socket a passive socket, that is,
        as a socket that will be used to accept incoming connection requests using accept.
    */
    for (size_t i = 0; i < this->serverSockets.size(); ++i) {
        listen(this->serverSockets[i], 1024);
    }
}

void Webserver::setBufferHeaderSize(void) {
    this->defaultBufferHeaderSize = 1024;
}

void Webserver::setClientMaxBodySize(void) {
    this->clientMaxBodySize = this->serverConfig->getClientMaxBodySize();
}

std::vector<int> Webserver::getServerSockets(void) const {
    return this->serverSockets;
}

Resources Webserver::getResources(std::string& method, std::string& route) const {
    return this->serverConfig->getResources(method, route);
}

std::string Webserver::getErrorPage(int statusCode) const {
    return this->serverConfig->getErrorPage(statusCode);
}

void Webserver::readRequest(int clientSocket) {
    this->httpRequestHandler->readRequest(
            clientSocket,
            this->clientBuffers[clientSocket]
    );
}

void Webserver::parseRequest(int clientSocket) {
    this->httpRequestHandler->parseRequest(this->clientBuffers[clientSocket], this->defaultBufferHeaderSize, this->clientMaxBodySize);
}

bool Webserver::isResponseAllowed(void) const {
    return this->allowResponse;
}

void Webserver::setAllowResponse(bool isResponseAllowed) {
    this->allowResponse = isResponseAllowed;
}

const std::map<std::string, std::string>& Webserver::getRequest() const {
    return this->httpRequestHandler->getRequest();
}

void Webserver::responseWriter(
        int socket,
        int statusCode,
        const char* reasonPhrase,
        const char *headers,
        const char* content,
        size_t contentLength
) {
    this->httpResponseHandler->send(
            socket,
            statusCode,
            reasonPhrase,
            headers,
            content,
            contentLength
    );

    this->clientBuffers.erase(socket);
}

void Webserver::updateClientBuffers(int clientSocket) {
    if (this->clientBuffers.find(clientSocket) != this->clientBuffers.end()) {
        return;
    }

    this->clientBuffers.insert(std::pair<int, std::vector<char> >(clientSocket, std::vector<char>()));
}

void Webserver::send(int clientSocket) {
    std::string method = this->httpRequestHandler->getHeader("Method");
    std::string route = this->httpRequestHandler->getHeader("Route");
    std::string contentType = this->httpRequestHandler->getHeader("Content-Type");
    Resources resources = this->serverConfig->getResources(method, route);

    if (std::string("GET") == method) {
        this->handleGET(clientSocket, method, route, contentType, resources);

        return;
    }

    if (std::string("POST") == method) {
        this->handlePOST(clientSocket, method, route, contentType);

        return;
    }

    if (std::string("DELETE") == method) {
        this->handleDELETE(clientSocket, resources);
    }
}

void Webserver::handleGET(
        int clientSocket,
        std::string& method,
        std::string& route,
        std::string& contentType,
        const Resources& resources
    ) {
    if (resources.isDirectory) {
        CGIRequest cgiReq = CGIRequest(
                method,
                this->httpRequestHandler->getHeader("Accept"),
                this->httpRequestHandler->getHeader("User-Agent"),
                this->serverConfig->getRoot(method, route),
                this->httpRequestHandler->getHeader("DecodedURI"),
                this->httpRequestHandler->getHeader("QueryString"),
                this->httpRequestHandler->getHeader("Content-Length"),
                contentType,
                this->httpRequestHandler->getBody(),
                LIST_DIR_SCRIPT
        );

        CGIResponse* cgiResponse = this->cgi->execute(cgiReq);

        this->responseWriter(
                clientSocket,
                403,
                "", // TODO: Parsear reasonPhrase do CGI
                cgiResponse->getCGIHeaders(),
                cgiResponse->getCGIBody(),
                strlen(cgiResponse->getCGIBody())
        );
        delete cgiResponse;
        return;
    }

    if (this->httpRequestHandler->getHeader("isPHP") == std::string("Yes")) {
        CGIRequest cgiReq = CGIRequest(
                method,
                this->httpRequestHandler->getHeader("Accept"),
                this->httpRequestHandler->getHeader("User-Agent"),
                this->serverConfig->getRoot(method, route),
                this->httpRequestHandler->getHeader("DecodedURI"),
                this->httpRequestHandler->getHeader("QueryString"),
                this->httpRequestHandler->getHeader("Content-Length"),
                contentType,
                this->httpRequestHandler->getBody()
        );

        CGIResponse* cgiResponse = this->cgi->execute(cgiReq);

        this->responseWriter(
                clientSocket,
                cgiResponse->getCGIStatus(),
                "", // TODO: Parsear reasonPhrase do CGI
                cgiResponse->getCGIHeaders(),
                cgiResponse->getCGIBody(),
                strlen(cgiResponse->getCGIBody())
        );
        delete cgiResponse;

        return;
    }

    std::vector<char> content = this->getContent(resources.path);

    std::string responseType = "Content-Type: ";
    responseType += resources.mimeType;

    this->responseWriter(
            clientSocket,
            200,
            "OK",
            responseType.c_str(),
            content.data(),
            content.size()
    );
}

void Webserver::handlePOST(
        int clientSocket,
        std::string& method,
        std::string& route,
        std::string& contentType
    ) {

    if (contentType.find("multipart/form-data") == std::string::npos) {
        std::string content = "Success";

        this->responseWriter(
                clientSocket,
                200,
                "OK",
                "Content-Type:text/plain",
                content.c_str(),
                content.size()
        );
        return;
    }

    CGIRequest cgiReq = CGIRequest(
            method,
            this->httpRequestHandler->getHeader("Accept"),
            this->httpRequestHandler->getHeader("User-Agent"),
            this->serverConfig->getRoot(method, route),
            this->httpRequestHandler->getHeader("DecodedURI"),
            this->httpRequestHandler->getHeader("QueryString"),
            this->httpRequestHandler->getHeader("Content-Length"),
            contentType,
            this->httpRequestHandler->getBody()
    );

    CGIResponse* cgiResponse = this->cgi->execute(cgiReq);

    this->responseWriter(
            clientSocket,
            cgiResponse->getCGIStatus(),
            "", // TODO: Parsear reasonPhrase do CGI
            cgiResponse->getCGIHeaders(),
            cgiResponse->getCGIBody(),
            strlen(cgiResponse->getCGIBody())
    );
    delete cgiResponse;
}

void Webserver::handleDELETE(int clientSocket, const Resources& resources) {
    if (resources.isDirectory) {
        throw BadRequestException();
    }
    if (remove(resources.path.c_str()) == 0) {
        std::string content = "DELETE has been made";

        this->responseWriter(
                clientSocket,
                200,
                "OK",
                "Content-Type:text/plain",
                content.c_str(),
                content.size()
        );
    } else {
        this->responseWriter(
                clientSocket,
                500,
                "Internal Server Error",
                "Content-Type:image/plain",
                "",
                0
        );
    }
}

std::vector<char> Webserver::getContent(std::string path) const {
    std::ifstream file;
    std::vector<char> content;
    char byte;

    file.open(path.c_str(), std::ios::binary);
    while (file.get(byte)) {
        content.push_back(byte);
    }
    file.close();

    return content;
}

std::string Webserver::getDirectoryFiles(std::string path) const {
    DIR* directory = opendir(path.c_str());

    if (!directory) {
        throw std::runtime_error("Something went wrong while opening a directory");
    }

    struct dirent* entry;
    std::string entries;

    while ((entry = readdir(directory)) != NULL) {
        // Filter out current directory and parent directory entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            entries += entry->d_name;
            entries += std::string("\n");
        }
    }

    closedir(directory);

    return entries;
}
