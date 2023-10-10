#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include "Lexer.hpp"
#include "ServerConfig.cpp"

class Parser
{
private:
    Lexer &lexer;
    std::vector<ServerConfig> serverConfigs;
    size_t position;

    void parseServerConfig();
    void parseToken(std::string keyword);
    void expectToken(std::string token);
    void parseBlockDirective();
    void initServerConfig();

    void buildListenDirective();
    bool isValidPort(std::string &port);

public:
    Parser(Lexer &lexer);

    std::vector<ServerConfig> parse();
};

#endif // PARSER_HPP_