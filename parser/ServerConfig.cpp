#include <string>
#include <vector>

class ServerConfig
{
public:
    ServerConfig()
    {
    }

    const int getPort() const
    {
        return this->_port;
    }

    const std::string getServerName() const
    {
        return this->_serverName;
    }

    const std::string getHost() const
    {
        return this->_host;
    }

    const std::vector<std::string> getErrorPages() const
    {
        return this->_errorPages;
    }

    const std::string getClientMaxBodySize() const
    {
        return this->_clientMaxBodySize;
    }

    ServerConfig &setPort(int port)
    {
        this->_port = port;
        return *this;
    }

    ServerConfig &setServerName(std::string serverName)
    {
        this->_serverName = serverName;
        return *this;
    }

    ServerConfig &setHost(std::string host)
    {
        this->_host = host;
        return *this;
    }

    ServerConfig &setErrorPages(std::vector<std::string> errorPages)
    {
        this->_errorPages = errorPages;
        return *this;
    }

    ServerConfig &setClientMaxBodySize(std::string clientMaxBodySize)
    {
        this->_clientMaxBodySize = clientMaxBodySize;
        return *this;
    }

    class LocationConfig
    {
    public:
        void setRoot(std::string root)
        {
            this->_root = root;
        }
        void setIndex(std::string index)
        {
            this->_index = index;
        }

    private:
        std::string _root;
        std::string _index;
        std::string _allow;
        std::string _deny;
        std::string _autoindex;
        std::string _defaultType;
        std::string _return;
        std::string _limitExcept;
        std::string _cgi;
        std::string _proxyPass;
    };

private:
    int _port;
    std::string _serverName;
    std::string _host;
    std::vector<std::string> _errorPages;
    std::string _clientMaxBodySize;
};