#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <cstdlib>
#include <string>
#include <Lexer.hpp>
#include <ServerConfig.hpp>


class Parser
{
public:
    Parser(const char *filePath);
    std::vector<ServerConfig> parse();

private:
    std::set<std::string>       _httpMethods;
    Lexer                       _lexer;
    std::vector<ServerConfig>   _serverConfigs;
    size_t                      _position;
    std::set<int>               _occupiedPorts;

    void _initServerConfig();
    void _initLocationBlock();

    bool _isValidPort(std::string &port);
    bool _isValidServerName(std::string &serverName);
    bool _isValidPath(std::string &rootPath);
    bool _isValidErrorPageDirective(Token token);
    bool _isValidErrorCode(int code);
    bool _isValidAddress(std::string address);
    bool _isValidHttpMethod(std::string method);
    bool _isConvertibleToInt(const std::string& str);

    void _parseServerConfig();
    void _parseServerBlock();
    void _parseListenDirective();
    void _parseToken(std::string keyword);
    void _parseServerNameDirective();
    void _parseLocationDirective();
    void _parseLocationBlock();
    void _parseRootDirective();
    void _parseIndexDirective();
    void _parseErrorPageDirective();
    void _parseClientMaxBodySizeDirective();
    void _parseAutoIndexDirective();
    void _parseProxyPassDirective();
    void _parseAliasDirective();
    void _parseTryFilesDirective();
    void _parseReturnDirective();
    void _parseAllowDirective();
    void _parseDenyDirective();
    void _parseAllowedMethods();
};

#endif // PARSER_HPP_
