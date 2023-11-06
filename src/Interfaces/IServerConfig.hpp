#ifndef I_SERVER_CONFIG_
#define I_SERVER_CONFIG_

#include <string>
#include <vector>

#include <Exceptions.hpp>
#include <Resources.hpp>

class IServerConfig {
public:
    /*
     * throws RouteNotFoundException
     * throws MethodNotAllowedException
     */
    virtual Resources getResources(std::string method, std::string route) = 0;
    virtual std::string getErrorPage(int statusCode) = 0;
    virtual std::string getRoot(std::string method, std::string route) = 0;
    virtual std::vector<int> getListeningPorts() = 0;
    virtual int getClientMaxBodySize() = 0;
    virtual ~IServerConfig() {};
};

#endif //I_SERVER_CONFIG_
