#ifndef SERVERLOCATION_HPP_
#define SERVERLOCATION_HPP_

#include <string>
#include <vector>
#include <map>

class ServerLocation
{
private:
    std::string _route;
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
    std::vector<std::string> _allowedMethods;

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
    std::string &getRoute();
    std::vector<std::string> &getAllowedMethods();

    void setRoot(std::string root);
    void setIndex(std::string index);
    void setAutoIndex(std::string autoIndex);
    void setProxyPass(std::string proxyPass);
    void setAlias(std::string alias);
    void setTryFiles(std::string tryFiles);
    void setReturn(int code, std::string msg);
    void setAllow(std::string address);
    void setDeny(std::string address);
    void setRoute(std::string route);
    void setAllowedMethods(std::string allowedMethod);
};

#endif // SERVERLOCATION_HPP_
