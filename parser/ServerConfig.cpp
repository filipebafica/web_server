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

std::vector<ServerConfig::Location> &ServerConfig::getLocation()
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

void ServerConfig::setLocationBlock(Location location)
{
    this->_locations.push_back(location);
}

void ServerConfig::Location::setRoot(std::string root)
{
    this->_root = root;
}

void ServerConfig::Location::setIndex(std::string index)
{
    this->_index.push_back(index);
}

std::vector<std::string> &ServerConfig::Location::getIndexes()
{
    return this->_index;
}

std::string &ServerConfig::Location::getRoot()
{
    return this->_root;
}
