#include "../Interfaces/IInitialParametersHandler.hpp"

class InitialParametersHandler : public IInitialParametersHandler {
public:
    int i;
    std::vector<std::string> allowedMethods;

    InitialParametersHandler(int i) {
        this->i = i;
        this->allowedMethods.push_back(std::string("POST"));
        this->allowedMethods.push_back(std::string("GET"));
        this->allowedMethods.push_back(std::string("DELETE"));
    };
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    const char* getResources(std::string& method, std::string& path) const {
        if (! this->validateMethod(method)) {
            throw MethodNotAllowedException();
        }

        if (path == std::string("/")) {
            return "./static/Hello-World-Page.html";
        }

        if (path == std::string("/up-the-irons")) {
            return "./static/Up-The-Irons-Page.html";
        }

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

private:
    bool validateMethod(std::string& method) const {
        for (int j = 0; j < this->allowedMethods.size(); j++) {
            if (this->allowedMethods[j] == method) {
                return true;
            }
        }

        return false;
    }
};