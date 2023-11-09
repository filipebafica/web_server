#ifndef SERVERCONFIG_HPP_
#define SERVERCONFIG_HPP_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <sys/stat.h>

#include <IServerConfig.hpp>
#include <ServerLocation.hpp>
#include <Resources.hpp>

class ServerConfig: public IServerConfig
{
public:
    ServerConfig();
    ~ServerConfig();

    Resources           getResources(std::string method, std::string route);
    std::string         getRoot(std::string method, std::string route);
    std::string         getErrorPage(int statusCode);
    std::vector<int>    getListeningPorts();
    int                 getClientMaxBodySize();

    const std::vector<std::string>  &getServerName() const;
    const std::string               &getHost() const;
    std::vector<ServerLocation>     &getLocations();
    const std::map<int,std::string> getErrorPages() const;

    ServerConfig                    &setPort(int port);
    ServerConfig                    &setServerName(std::string serverName);
    ServerConfig                    &setHost(std::string host);
    ServerConfig                    &setErrorPages(int code, std::string page);
    ServerConfig                    &setClientMaxBodySize(int clientMaxBodySize);
    void                            setLocationBlock(ServerLocation location);

private:
    std::vector<int>                _port;
    std::vector<std::string>        _serverName;
    std::string                     _host;
    std::map<int, std::string>      _errorPages;
    int                             _clientMaxBodySize;
    std::vector<ServerLocation>     _locations;

    Resources   _getResourcePathFile(int locationPosition, std::string route);
    Resources   _getResourcePathFromDirectory(int locationPosition, std::string locationRoot, std::string requestedRoute);
    std::string _getIndexFilePath(std::string path, std::vector<std::string> fileNames);
    Resources   _getResourcePathFromFile(int locationPosition, std::string locationRoot, std::string requestedRoute);
    std::string _getFileFromRoute(std::string requestedRoute);
    std::string _parseMimeType(std::string fileName);

    int _selectLocationPosition(std::string route);

    bool _isRequestedRouteDirectory(std::string requestedRoute);
    bool _isMethodAllowed(std::string method, int selectedLocation);
    bool _fileExists(std::string& filename);
    bool _isAutoIndexOn(int locationPosition);
};

#endif // SERVERCONFIG_HPP_
