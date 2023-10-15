#ifndef SERVERCONFIG_HPP_
#define SERVERCONFIG_HPP_

#include <string>
#include <vector>
#include <map>

class ServerConfig
{
public:
    class Location
    {
    private:
        std::string _modifier;
        std::string _pattern;
        std::string _root;
        std::vector<std::string> _index;
        std::string _allow;
        std::string _deny;
        std::string _autoindex;
        std::string _defaultType;
        std::string _return;
        std::string _limitExcept;
        std::string _cgi;
        std::string _proxyPass;

    public:
        std::vector<std::string> &getIndexes();
        std::string &getRoot();

        void setRoot(std::string root);
        void setIndex(std::string index);
    };

    const std::vector<int>          &getPort() const;
    const std::vector<std::string>  &getServerName() const;
    const std::string               &getHost() const;
    const std::vector<std::string>  &getErrorPages() const;
    const std::string               &getClientMaxBodySize() const;
    std::vector<ServerConfig::Location> &getLocation();

    ServerConfig                    &setPort(int port);
    ServerConfig                    &setServerName(std::string serverName);
    ServerConfig                    &setHost(std::string host);
    ServerConfig                    &setErrorPages(std::vector<std::string> errorPages);
    ServerConfig                    &setClientMaxBodySize(std::string clientMaxBodySize);
    void                            setLocationBlock(Location location);


private:
    std::vector<int> _port;
    std::vector<std::string> _serverName;
    std::string _host;
    std::vector<std::string> _errorPages;
    std::string _clientMaxBodySize;
    std::vector<ServerConfig::Location> _locations;
};

#endif // SERVERCONFIG_HPP_
