#include "../Interfaces/IConfigHandler.hpp"

class ConfigHandler : public IConfigHandler {
public:
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    void routeValidate(std::string& path, std::string& method) {
        throw MethodNotAllowedException();
//        throw PathNotFoundException();
    }
};