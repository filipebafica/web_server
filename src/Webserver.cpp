#include <Webserver.hpp>

Webserver::Webserver(
        IServerConfig* serverConfig,
        IHttpRequestHandler* httpRequestHandler,
        IHttpResponseHandler* httpResponseHandler,
        ICGI* cgi
) {
    // Inject dependencies
    this->serverConfig = serverConfig;
    this->httpRequestHandler = httpRequestHandler;
    this->httpResponseHandler = httpResponseHandler;
    this->cgi = cgi;

    // Setups server
    this->setupAddress();
    this->createSocket();
    this->bindServerSocket();
    this->startListening();

    // Initialize attributes
    this->allowResponse = false;
}

Webserver::~Webserver() {
    delete this->httpRequestHandler;
    delete this->httpResponseHandler;
    delete this->cgi;

    for (size_t i = 0; i < this->serverAddresses.size(); i++) {
        delete this->serverAddresses[i];
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

    for (size_t i = 0; i < ports.size(); ++i) {
        int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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
    this->httpRequestHandler->readRequest(clientSocket);
}

void Webserver::parseRequest(void) {
    this->httpRequestHandler->parseRequest();
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

void Webserver::send(int clientSocket) {
    try {
        std::string method = this->httpRequestHandler->getHeader("Method");
        std::string route = this->httpRequestHandler->getHeader("Route");
        std::string contentType = this->httpRequestHandler->getHeader("Content-Type");

        if (std::string("GET") == method) {
            this->handleGET(clientSocket, method, route, contentType);

            return;
        }

        if (std::string("POST") == method) {
            this->handlePOST(clientSocket, method, route, contentType);

            return;
        }

        if (std::string("DELETE") == method) {
            this->handleDELETE(clientSocket);
        }

    } catch (MethodNotAllowedException& exception) {
        this->httpResponseHandler->send(
                clientSocket,
                405,
                "",
                this->getContent(this->getErrorPage(405)).c_str()
        );

    } catch (RouteNotFoundException& exception) {
        this->httpResponseHandler->send(
                clientSocket,
                404,
                "",
                this->getContent(this->getErrorPage(404)).c_str()
        );

    } catch (std::exception& exception) {
        this->httpResponseHandler->send(
                clientSocket,
                500,
                "",
                "Internal Server Error"
        );
    }
}

void Webserver::handleGET(
        int clientSocket,
        std::string& method,
        std::string& route,
        std::string& contentType
    ) const {
    Resources resources = this->serverConfig->getResources(method, route);

    if (resources.isDirectory) {
        this->httpResponseHandler->send(
                clientSocket,
                403,
                "",
                this->getDirectoryFiles(resources.path).c_str()
        );

        return;
    }

    if (this->httpRequestHandler->getHeader("isPHP") == std::string("YES")) {
        CGIRequest cgiReq = CGIRequest(
                method,
                this->httpRequestHandler->getHeader("Accept"),
                this->httpRequestHandler->getHeader("Agent"),
                this->serverConfig->getRoot(method, route),
                this->httpRequestHandler->getHeader("DecodedURI"),
                this->httpRequestHandler->getHeader("QueryString"),
                this->httpRequestHandler->getHeader("Content-Length"),
                contentType,
                this->httpRequestHandler->getHeader("Body")
        );

        CGIResponse* cgiResponse = this->cgi->execute(cgiReq);

        this->httpResponseHandler->send(
                clientSocket,
                cgiResponse->getCGIStatus(),
                cgiResponse->getCGIHeaders(),
                cgiResponse->getCGIBody()
        );
    }

    this->httpResponseHandler->send(
            clientSocket,
            200,
            "",
            this->getContent(resources.path).c_str()
    );
}

void Webserver::handlePOST(
        int clientSocket,
        std::string& method,
        std::string& route,
        std::string& contentType
    ) const {

    if (contentType.find("multipart/form-data") == std::string::npos) {
        this->httpResponseHandler->send(
                clientSocket,
                200,
                "",
                "success"
        );
    }

    CGIRequest cgiReq = CGIRequest(
            method,
            this->httpRequestHandler->getHeader("Accept"),
            this->httpRequestHandler->getHeader("Agent"),
            this->serverConfig->getRoot(method, route),
            this->httpRequestHandler->getHeader("DecodedURI"),
            this->httpRequestHandler->getHeader("QueryString"),
            this->httpRequestHandler->getHeader("Content-Length"),
            contentType,
            this->httpRequestHandler->getHeader("Body")
    );

    CGIResponse* cgiResponse = this->cgi->execute(cgiReq);

    this->httpResponseHandler->send(
            clientSocket,
            cgiResponse->getCGIStatus(),
            cgiResponse->getCGIHeaders(),
            "file was uploaded successfully"
    );
}

void Webserver::handleDELETE(int clientSocket) {
    this->httpResponseHandler->send(
            clientSocket,
            200,
            "",
            "DELETE has been made"
    );
}

std::string Webserver::getContent(std::string path) const {
    std::ifstream file;
    std::string content;
    std::string line;

    file.open(path.c_str(), std::ios::binary);
    while (std::getline(file, line)) {
        content += line;
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


