#include <exception>
#include <string>

class MethodNotAllowedException: public std::exception {};
class PathNotFoundException: public std::exception {};

class IConfigHandler {
public:
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    virtual void routeValidate(std::string& path, std::string& method) = 0;
    virtual ~IConfigHandler() {};
};
