#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {
    _errorPages[404] = "./static/404-Page.html";
    _errorPages[405] = "./static/405-Page.html";
    _errorPages[500] = "./static/500-Page.html";
    _errorPages[502] = "./static/502-Page.html";
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

const std::string &ServerConfig::getClientMaxBodySize() const
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

ServerConfig &ServerConfig::setClientMaxBodySize(std::string clientMaxBodySize)
{
    this->_clientMaxBodySize = clientMaxBodySize;
    return *this;
}

void ServerConfig::setLocationBlock(ServerLocation location)
{
    this->_locations.push_back(location);
}

std::string ServerConfig::getResources(std::string method, std::string route)
{
    std::string resource("");
    int locationPosition = _selectLocationPosition(route);
    if (!_isMethodAllowed(method, locationPosition))
    {
        throw MethodNotAllowedException();
    }
    resource = _getResourcePathFile(locationPosition, route);
    return resource;
}

int ServerConfig::_selectLocationPosition(std::string route)
{
    std::vector<ServerLocation> locations = getLocations();
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
    std::vector<ServerLocation> locations = getLocations();
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

std::string ServerConfig::_getResourcePathFile(int locationPosition, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = getLocations();
    ServerLocation location = locations[locationPosition];
    std::string locationRoot = location.getRoot();

    if (_isRequestedRouteDirectory(requestedRoute))
    {
        return(_getResourcePathFromDirectory(locationPosition, locationRoot, requestedRoute));
    }
    return(_getResourcePathFromFile(locationPosition, locationRoot, requestedRoute));
}

bool ServerConfig::_isRequestedRouteDirectory(std::string requestedRoute)
{
    if (requestedRoute[requestedRoute.length() - 1] == '/')
    {
        return true;
    }

    return false;
}


std::string ServerConfig::_getResourcePathFromDirectory(int locationPosition, std::string locationRoot, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = getLocations();
    ServerLocation location = locations[locationPosition];

    std::string fullPath = locationRoot + requestedRoute;
    std::vector<std::string> fileNames = location.getIndexes();

    std::string filePath = _getIndexFilePath(fullPath, fileNames);
    return filePath;
}

std::string ServerConfig::_getIndexFilePath(std::string path, std::vector<std::string> fileNames)
{
    std::string fileName = "";
    for (size_t i = 0; i < fileNames.size(); i++)
    {
        fileName = path + fileNames[i];
        if (_fileExists(fileName))
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
    return true;
}

std::string ServerConfig::_getResourcePathFromFile(int locationPosition, std::string locationRoot, std::string requestedRoute)
{
    std::vector<ServerLocation> locations = getLocations();
    ServerLocation location = locations[locationPosition];

    std::string fileName = locationRoot + requestedRoute;
    if (!_fileExists(fileName))
    {
        throw RouteNotFoundException();
    }
    return fileName;
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
    std::vector<ServerLocation> locations = getLocations();
    int locationPosition = _selectLocationPosition(route);
    if (!_isMethodAllowed(method, locationPosition))
    {
        throw std::runtime_error("specified method not allowed");
    }
    root = locations[locationPosition].getRoot();
    return root;
}

std::string ServerConfig::getErrorPage(int statusCode)
{
    const std::map<int, std::string> errorPages = getErrorPages();
    struct stat buffer;
    if (errorPages.empty()) {
        if (stat(DEFAULT_ERROR_PAGE_PATH.c_str(), &buffer) == -1) {
            throw std::runtime_error("there is no default error page");
        }
        return DEFAULT_ERROR_PAGE_PATH;
    }

    std::map<int, std::string>::const_iterator it = errorPages.find(statusCode);

    if (it != errorPages.end()) {
        return it->second;
    } else {
        throw std::runtime_error("there is no error page for specified error");
    }
}
