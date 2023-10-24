#ifndef SERVERCONFIG_HPP_
#define SERVERCONFIG_HPP_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include "ServerLocation.hpp"
#include "../../Interfaces/IServerConfig.hpp"

class ServerConfig: public IServerConfig
{
public:
    const std::string DEFAULT_ERROR_PAGE_PATH;

    ServerConfig();
    ~ServerConfig();

    std::string               getResources(std::string method, std::string route);
    std::string               getRoot(std::string method, std::string route);
    std::string               getErrorPage(int statusCode);
    std::vector<int>          getListeningPorts();

    const std::vector<std::string>  &getServerName() const;
    const std::string               &getHost() const;
    const std::string               &getClientMaxBodySize() const;
    std::vector<ServerLocation>     &getLocations();
    const std::map<int,std::string> getErrorPages() const;

    ServerConfig                    &setPort(int port);
    ServerConfig                    &setServerName(std::string serverName);
    ServerConfig                    &setHost(std::string host);
    ServerConfig                    &setErrorPages(int code, std::string page);
    ServerConfig                    &setClientMaxBodySize(std::string clientMaxBodySize);
    void                            setLocationBlock(ServerLocation location);

private:
    std::vector<int>                _port;
    std::vector<std::string>        _serverName;
    std::string                     _host;
    std::map<int, std::string>      _errorPages;
    std::string                     _clientMaxBodySize;
    std::vector<ServerLocation>     _locations;

    std::string _getResourcePathFile(int locationPosition, std::string route);
    std::string _getResourcePathFromDirectory(int locationPosition, std::string locationRoot, std::string requestedRoute);
    std::string _getIndexFilePath(std::string path, std::vector<std::string> fileNames);
    std::string _getResourcePathFromFile(int locationPosition, std::string locationRoot, std::string requestedRoute);
    std::string _getFileFromRoute(std::string requestedRoute);

    int _selectLocationPosition(std::string route);

    bool _isRequestedRouteDirectory(std::string requestedRoute);
    bool _isMethodAllowed(std::string method, int selectedLocation);
    bool _fileExists(std::string& filename);
};

#endif // SERVERCONFIG_HPP_
