class IServerSetupHandler {
public:
    virtual void createSocket() = 0;
    virtual void setupAddress() = 0;
    virtual void bindServerSocket() = 0;
    virtual int getServerSocket() const = 0;
    virtual void startListening() const = 0;
    virtual struct sockaddr_in* getServerAddress() const = 0;
    virtual ~IServerSetupHandler() {};
};