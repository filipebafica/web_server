#include <ServerConfig.hpp>

ServerConfig::ServerConfig() {
    this->_errorPages[400] = "./static/errorPages/400-Page.html";
    this->_errorPages[404] = "./static/errorPages/404-Page.html";
    this->_errorPages[405] = "./static/errorPages/405-Page.html";
    this->_errorPages[413] = "./static/errorPages/413-Page.html";
    this->_errorPages[500] = "./static/errorPages/500-Page.html";
    this->_errorPages[502] = "./static/errorPages/502-Page.html";

    this->_host = "";
    this->_clientMaxBodySize = 1000000;
}

ServerConfig::~ServerConfig() {}

std::vector<int> ServerConfig::getListeningPorts()
{
    return this->_port;
}

const std::vector<std::string> &ServerConfig::getServerName() const
{
    return this->_serverName;
}

const std::string &ServerConfig::getHost() const
{
    return this->_host;
}

const std::map<int,std::string> ServerConfig::getErrorPages() const
{
    return this->_errorPages;
}

int ServerConfig::getClientMaxBodySize()
{
    return this->_clientMaxBodySize;
}

std::vector<ServerLocation> &ServerConfig::getLocations()
{
    return this->_locations;
} 

ServerConfig &ServerConfig::setPort(int port)
{
    this->_port.push_back(port);
    return *this;
}

ServerConfig &ServerConfig::setServerName(std::string serverName)
{
    this->_serverName.push_back(serverName);
    return *this;
}

ServerConfig &ServerConfig::setHost(std::string host)
{
    this->_host = host;
    return *this;
}

ServerConfig &ServerConfig::setErrorPages(int code, std::string page)
{
    this->_errorPages[code] = page;
    return *this;
}

ServerConfig &ServerConfig::setClientMaxBodySize(int clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
    return *this;
}

void ServerConfig::setLocationBlock(ServerLocation location)
{
    this->_locations.push_back(location);
}

Resources ServerConfig::getResources(std::string method, std::string route)
{
    int locationPosition = this->_selectLocationPosition(route);
    if (!this->_isMethodAllowed(method, locationPosition))
    {
        throw MethodNotAllowedException();
    }
 
    return this->_getResourcePathFile(locationPosition, route);
}

int ServerConfig::_selectLocationPosition(std::string route)
{
    std::vector<ServerLocation> locations = this->getLocations();
    size_t maxLength = 0;
    int locationPosition = -1;
    
    for (size_t i = 0; i < locations.size(); i++)
    {
        if (route.find(locations[i].getRoute()) == 0 && locations[i].getRoute().length() > maxLength) {
            locationPosition = i;
            maxLength = locations[i].getRoute().length();
        }
    }

    if (locationPosition == -1) {
        throw std::runtime_error("could not select location for the given route");
    }

    return locationPosition;
}

bool ServerConfig::_isMethodAllowed(std::string method, int selectedLocation)
{
    std::vector<ServerLocation> locations = this->getLocations();
    std::vector<std::string> allowedMethods = locations[selectedLocation].getAllowedMethods();

    if (allowedMethods.empty())
    {
        return false;
    }

    for (size_t i = 0; i < allowedMethods.size(); i++)
    {
        if (allowedMethods[i] == method)
        {
            return true;
        }
    }

    return false;
}

Resources ServerConfig::_getResourcePathFile(int locationPosition, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = this->getLocations();
    ServerLocation location = locations[locationPosition];
    std::string locationRoot = location.getRoot();

    if (this->_isRequestedRouteDirectory(requestedRoute))
    {
        return(this->_getResourcePathFromDirectory(locationPosition, locationRoot, requestedRoute));
    }
    return(this->_getResourcePathFromFile(locationPosition, locationRoot, requestedRoute));
}

bool ServerConfig::_isRequestedRouteDirectory(std::string requestedRoute)
{
    if (requestedRoute[requestedRoute.length() - 1] == '/')
    {
        return true;
    }

    return false;
}


Resources ServerConfig::_getResourcePathFromDirectory(int locationPosition, std::string locationRoot, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = this->getLocations();
    ServerLocation location = locations[locationPosition];

    std::string fullPath = locationRoot + requestedRoute;
    std::vector<std::string> fileNames = location.getIndexes();

    try {
        std::string filePath = this->_getIndexFilePath(fullPath, fileNames);
        std::string mimeType = this->_parseMimeType(filePath);
        Resources resources(filePath, false, mimeType);
        return resources;
    } catch (RouteNotFoundException &exception) {
        if (!this->_isAutoIndexOn(locationPosition)) {
            throw RouteNotFoundException();
        }
    }
    Resources resources(fullPath, true, "");
    return resources;
}

std::string ServerConfig::_getIndexFilePath(std::string path, std::vector<std::string> fileNames)
{
    std::string fileName = "";
    for (size_t i = 0; i < fileNames.size(); i++)
    {
        fileName = path + fileNames[i];
        if (this->_fileExists(fileName))
        {
            return fileName;
        }
    }
    throw RouteNotFoundException();
}

bool ServerConfig::_fileExists(std::string& fileName)
{
    struct stat buffer;
    if (stat(fileName.c_str(), &buffer) == -1)
    {
        return false;
    }
    return S_ISREG(buffer.st_mode);
}

Resources ServerConfig::_getResourcePathFromFile(int locationPosition, std::string locationRoot, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = this->getLocations();
    ServerLocation location = locations[locationPosition];

    std::string fileName = locationRoot + requestedRoute;
    if (!this->_fileExists(fileName))
    {
        throw RouteNotFoundException();
    }

    std::string mimeType = this->_parseMimeType(fileName);
    Resources resources(fileName, false, mimeType);
    return resources;
}

std::string ServerConfig::_getFileFromRoute(std::string requestedRoute)
{
    size_t lastSlashPos = requestedRoute.find_last_of('/');
    if (lastSlashPos != std::string::npos)
    {
        std::string file = requestedRoute.substr(lastSlashPos + 1);
        return file;
    }
    return requestedRoute;
}

std::string ServerConfig::getRoot(std::string method, std::string route)
{
    std::string root("");
    std::vector<ServerLocation> locations = this->getLocations();
    int locationPosition = this->_selectLocationPosition(route);
    if (!this->_isMethodAllowed(method, locationPosition))
    {
        throw MethodNotAllowedException();
    }
    root = locations[locationPosition].getRoot();
    return root;
}

std::string ServerConfig::getErrorPage(int statusCode)
{
    const std::map<int, std::string> errorPages = this->getErrorPages();

    std::map<int, std::string>::const_iterator it = errorPages.find(statusCode);

    if (it != errorPages.end()) {
        return it->second;
    } else {
        return "";
    }
}

bool ServerConfig::_isAutoIndexOn(int locationPosition)
{
    std::vector<ServerLocation> locations = this->getLocations();
    ServerLocation location = locations[locationPosition];

    if (location.getAutoIndex() == "on") {
        return true;
    } else {
        return false;
    }
}

std::string ServerConfig::_parseMimeType(std::string fileName)
{
    std::string mimeType;

    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".gif"]   = "image/gif";
    mimeTypes[".htm"]   = "text/html";
    mimeTypes[".html"]  = "text/html";
    mimeTypes[".jpeg"]  = "image/jpeg";
    mimeTypes[".jpg"]   = "image/jpeg";
    mimeTypes[".mp3"]   = "audio/mpeg";
    mimeTypes[".mpeg"]  = "video/mpeg";
    mimeTypes[".mp4"]   = "video/mp4";
    mimeTypes[".pdf"]   = "application/pdf";
    mimeTypes[".png"]   = "image/png";
    mimeTypes[".php"]   = "application/x-httpd-php";
    mimeTypes[".rar"]   = "application/vnd.rar";
    mimeTypes[".svg"]   = "image/svg+xml";
    mimeTypes[".tar"]   = "application/x-tar";
    mimeTypes[".txt"]   = "text/plain";
    mimeTypes[".wav"]   = "audio/wav";
    mimeTypes[".xhtml"] = "application/xhtml+xml";
    mimeTypes[".xml"]   = "application/xml";
    mimeTypes[".zip"]   = "application/zip";

    size_t dotIndex = fileName.find_last_of('.');
    if (dotIndex == std::string::npos) {
        return mimeTypes[".txt"];
    }
    std::string extension = fileName.substr(dotIndex);

    std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
    if (it == mimeTypes.end()) {
        return mimeTypes[".txt"];
    }

    return mimeTypes[it->first];
}
