#ifndef I_INITIAL_PARAMETERS_HANDLER_
#define I_INITIAL_PARAMETERS_HANDLER_

#include <exception>
#include <string>
#include <vector>

class MethodNotAllowedException: public std::exception {};
class PathNotFoundException: public std::exception {};

class IInitialParametersHandler {
public:
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    virtual const char* getResources(std::string& method, std::string& path) const = 0;
    virtual const char* getErrorPage() const = 0;
    virtual const std::vector<int> getListeningPorts() const = 0;
    virtual ~IInitialParametersHandler() {};
};

#endif //I_INITIAL_PARAMETERS_HANDLER_