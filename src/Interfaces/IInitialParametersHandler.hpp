#ifndef I_INITIAL_PARAMETERS_HANDLER_
#define I_INITIAL_PARAMETERS_HANDLER_

#include <exception>
#include <string>
#include <vector>

class MethodNotAllowedException: public std::exception {};
class RouteNotFoundException: public std::exception {};

class IInitialParametersHandler {
public:
    /*
     * throws RouteNotFoundException
     * throws MethodNotAllowedException
     */
    virtual const char* getResources(const std::string& method, const std::string& route) const = 0;
    virtual const char* getUploadPath(const std::string& route) const = 0;
    virtual const char* getErrorPage() const = 0;
    virtual const std::vector<int> getListeningPorts() const = 0;
    virtual ~IInitialParametersHandler() {};
};

#endif //I_INITIAL_PARAMETERS_HANDLER_