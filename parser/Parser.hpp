#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include "Lexer.hpp"
#include "ServerConfig.hpp"

class Parser
{
private:
    Lexer &lexer;
    std::vector<ServerConfig> serverConfigs;
    size_t position;

    void initServerConfig();
    void initLocationBlock();

    bool isValidPort(std::string &port);
    bool isValidServerName(std::string &serverName);
    bool isValidRoot(std::string &rootPath);

    void parseServerConfig();
    void parseServerBlock();
    void parseListenDirective();
    void parseToken(std::string keyword);
    void parseServerNameDirective();

    void parseLocationDirective();
    void parseLocationBlock();
    void parseRootDirective();
    void parseIndexDirective();

public:
    Parser(Lexer &lexer);

    std::vector<ServerConfig> parse();
};

#endif // PARSER_HPP_
