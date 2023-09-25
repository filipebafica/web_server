#include "../Interfaces/IInitialParametersHandler.hpp"

class InitialParametersHandler : public IInitialParametersHandler
{
public:
    /*
     * throws PathNotFoundException
     * throws MethodNotAllowedException
     */
    const char *getResourcesPath(std::string &method, std::string &path)
    {
        if (path == std::string("/"))
        {
            return "./static/Hello-World-Page.html";
        }
        if (path == std::string("/up-the-irons"))
        {
            return "./static/Up-The-Irons-Page.html";
        }
        //        throw MethodNotAllowedException();
        throw PathNotFoundException();
    }

    const char *getErrorPage() const
    {
        //        if (this->errorPage != NULL) {
        //            return "path/to/user/error/page";
        //        }

        return "./static/404-Page.html";
    }
};