#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <cstring>
#include <fstream>
#include <list>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

#include <Webserver.hpp>

class Monitor {
 private:
    std::vector<Webserver*>* webservers;
    std::list<int> clientSockets;
    std::vector<struct pollfd> pollFds;
    std::map<int, Webserver*> fdToWebserverMap;

    int numberOfServers;
    volatile bool monitorRunning;

 public:
    Monitor(std::vector<Webserver*>* webservers);
    ~Monitor();

    void loop(void);
    void stopLoop(void);

 private:
    void    updatePollFdsVectorWithServerSockets(void);
    void    updatePollFdsVectorWithClientSockets(void);
    int     pollEvents(void);
    bool    serverSocketHasEvents(struct pollfd& fd);
    void    updateClientSocketsVector(int serverSocket);
    Webserver* findWebserver(int fileDescriptor) const;
    void    clean(int clientSocket);
};

#endif /* MONITOR_HPP_ */
