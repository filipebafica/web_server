#include "ServerConfig.hpp"

const std::vector<int> &ServerConfig::getPort() const
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

const std::map<int,std::string> &ServerConfig::getErrorPages() const
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
    try {
        int locationPosition = selectLocationPosition(route);
        if (!_isMethodAllowed(method, locationPosition))
        {
            throw std::runtime_error("specified method not allowed");
        }
        resource = _getResourcePathFile(locationPosition, route);
        std::cout << "### DEBUG ###" << resource << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
    return resource;
}

int ServerConfig::selectLocationPosition(std::string route)
{
    std::vector<ServerLocation> locations = getLocations();
    size_t maxLength = 0;
    int locationPosition = -1;
    
    for (int i = 0; i < locations.size(); i++)
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
        return true;
    }

    for (int i = 0; i < allowedMethods.size(); i++)
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

    std::string pathFile = "";
    if (!_isRequestedRouteDirectory(requestedRoute))
    {
        std::string pathFile = _getResourcePathFromDirectory(locationPosition, locationRoot, requestedRoute);
    }
    return pathFile;
}

bool ServerConfig::_isRequestedRouteDirectory(std::string requestedRoute)
{
    if (*requestedRoute.end() == '/')
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

    //Refazer daqui pra baixo pois é para pegar apenas o index ou posso mudar para pega todo o path;
    std::string fileName = _getIndexFilePath(fullPath, fileNames);

    return fileName;
}

std::string ServerConfig::_getIndexFilePath(std::string path, std::vector<std::string> fileNames)
{
    std::string fileName = "";
    std::cout << fileNames.size() << std::endl;
    for (int i = 0; i < fileNames.size(); i++)
    {
        std::cout << fileNames[i] << std::endl;
        if (_fileExists(fileName))
        {
            return fileName;
        }
    }
    
    throw std::runtime_error("file doesn't exists");
}

bool ServerConfig::_fileExists(std::string& filename)
{
    std::ifstream file(filename.c_str());
    return file.good();
}
