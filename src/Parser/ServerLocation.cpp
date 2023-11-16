#include <ServerLocation.hpp>

ServerLocation::ServerLocation() {
    this->_return = "";
}

void ServerLocation::setRoot(std::string root)
{
    this->_root = root;
}

void ServerLocation::setIndex(std::string index)
{
    this->_index.push_back(index);
}

std::vector<std::string> &ServerLocation::getIndexes()
{
    return this->_index;
}

std::string &ServerLocation::getRoot()
{
    return this->_root;
}

void ServerLocation::setAutoIndex(std::string autoindex)
{
    this->_autoindex = autoindex;
}

std::string &ServerLocation::getAutoIndex()
{
    return this->_autoindex;
}

void ServerLocation::setProxyPass(std::string proxyPass)
{
    this->_proxyPass = proxyPass;
}

std::string &ServerLocation::getProxyPass()
{
    return this->_proxyPass;
}

void ServerLocation::setAlias(std::string alias)
{
    this->_alias = alias;
}

std::string &ServerLocation::getAlias()
{
    return this->_alias;
}

std::vector<std::string> &ServerLocation::getTryFiles()
{
    return this->_tryFiles;
}

void ServerLocation::setTryFiles(std::string tryfile)
{
    this->_tryFiles.push_back(tryfile);
}

void ServerLocation::setReturn(std::string msg)
{
    this->_return = msg;
}

std::string &ServerLocation::getReturn()
{
    return this->_return;
}

std::vector<std::string> &ServerLocation::getAllow()
{
    return this->_allow;
}

void ServerLocation::setAllow(std::string address)
{
    this->_allow.push_back(address);
}

std::vector<std::string> &ServerLocation::getDeny()
{
    return this->_allow;
}

void ServerLocation::setDeny(std::string address)
{
    this->_allow.push_back(address);
}

std::string &ServerLocation::getRoute()
{
    return this->_route;
}

void ServerLocation::setRoute(std::string route)
{
    this->_route = route;
}

std::vector<std::string> &ServerLocation::getAllowedMethods()
{
    return this->_allowedMethods;
}

void ServerLocation::setAllowedMethods(std::string method)
{
    this->_allowedMethods.push_back(method);
}
