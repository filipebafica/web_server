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
        std::vector<std::string> _allow;
        std::vector<std::string> _deny;
        std::string _autoindex;
        std::string _defaultType;
        std::map<int,std::string> _return;
        std::string _limitExcept;
        std::string _cgi;
        std::string _proxyPass;
        std::string _alias;
        std::vector<std::string> _tryFiles;

    public:
        std::vector<std::string> &getIndexes();
        std::string &getRoot();
        std::string &getAutoIndex();
        std::string &getProxyPass();
        std::string &getAlias();
        std::vector<std::string> &getTryFiles();
        std::map<int,std::string> &getReturn();
        std::vector<std::string> &getAllow();
        std::vector<std::string> &getDeny();

        void setRoot(std::string root);
        void setIndex(std::string index);
        void setAutoIndex(std::string autoIndex);
        void setProxyPass(std::string proxyPass);
        void setAlias(std::string alias);
        void setTryFiles(std::string tryFiles);
        void setReturn(int code, std::string msg);
        void setAllow(std::string address);
        void setDeny(std::string address);
    };

    const std::vector<int>          &getPort() const;
    const std::vector<std::string>  &getServerName() const;
    const std::string               &getHost() const;
    const std::map<int,std::string>  &getErrorPages() const;
    const std::string               &getClientMaxBodySize() const;
    std::vector<ServerConfig::Location> &getLocation();

    ServerConfig                    &setPort(int port);
    ServerConfig                    &setServerName(std::string serverName);
    ServerConfig                    &setHost(std::string host);
    ServerConfig                    &setErrorPages(int code, std::string page);
    ServerConfig                    &setClientMaxBodySize(std::string clientMaxBodySize);
    void                            setLocationBlock(Location location);


private:
    std::vector<int> _port;
    std::vector<std::string> _serverName;
    std::string _host;
    std::map<int, std::string> _errorPages;
    std::string _clientMaxBodySize;
    std::vector<ServerConfig::Location> _locations;
};

#endif // SERVERCONFIG_HPP_
