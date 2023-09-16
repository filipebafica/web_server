#include <exception>
#include <string>

class MethodNotAllowedException: public std::exception {};
class PathNotFoundException: public std::exception {};

class IInitialParametersHandler {
public:
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    virtual const char* getResourcesPath(std::string& method, std::string& path) = 0;
    virtual const char* getErrorPage() const = 0;
    virtual const std::vector<int> getListeningPorts() const = 0;
    virtual ~IInitialParametersHandler() {};
};
