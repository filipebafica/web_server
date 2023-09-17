#include "../Interfaces/IInitialParametersHandler.hpp"

class InitialParametersHandler : public IInitialParametersHandler {
public:
    int i;
    InitialParametersHandler(int i) {
        this->i = i;
    };
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    const char* getResourcesPath(std::string& method, std::string& path) const {
        if (path == std::string("/")) {
            return "./static/Hello-World-Page.html";
        }
        if (path == std::string("/up-the-irons")) {
            return "./static/Up-The-Irons-Page.html";
        }
        //        throw MethodNotAllowedException();
        throw PathNotFoundException();
    }

    const char* getErrorPage() const {
//        if (this->errorPage != NULL) {
//            return "path/to/user/error/page";
//        }

        return "./static/404-Page.html";
    }

    const std::vector<int> getListeningPorts() const {
        std::vector<int> ports;
        ports.push_back(8080 + this->i);
        ports.push_back(8081 + this->i);
        ports.push_back(8082 + this->i);
        return ports;
    }
};