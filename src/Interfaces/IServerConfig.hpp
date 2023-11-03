#ifndef I_SERVER_CONFIG_
#define I_SERVER_CONFIG_

#include <exception>
#include <string>
#include <vector>

class MethodNotAllowedException: public std::exception {};
class RouteNotFoundException: public std::exception {};

class IServerConfig {
public:
    /*
     * throws RouteNotFoundException
     * throws MethodNotAllowedException
     */
    virtual std::string getResources(std::string method, std::string route) = 0;
    virtual std::string getErrorPage(int statusCode) = 0;
    virtual std::string getRoot(std::string method, std::string route) = 0;
    virtual std::vector<int> getListeningPorts() = 0;
    virtual ~IServerConfig() {};
};

#endif //I_SERVER_CONFIG_
