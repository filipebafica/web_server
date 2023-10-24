#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <cstring>
#include <fstream>
#include <list>
#include <sys/poll.h>
#include <unistd.h>
#include <vector>

#include "./Webserver.hpp"

using std::list;
using std::map;
using std::pair;
using std::vector;

using std::cerr;
using std::endl;

class Monitor {
 private:
    vector<Webserver*>* webservers;
    list<int> clientSockets;
    vector<struct pollfd> pollFds;
    map<int, Webserver*> fdToWebserverMap;
    int numberOfServers;
    volatile bool monitorRunning;

 public:
    Monitor(vector<Webserver*>* webservers);
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
};

#endif /* MONITOR_HPP_ */
