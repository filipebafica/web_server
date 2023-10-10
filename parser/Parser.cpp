// Implements the parser recognition

#include "Parser.hpp"

Parser::Parser(Lexer &lexer) : lexer(lexer), position(0) {}

std::vector<ServerConfig> Parser::parse()
{
    while (lexer.peek().type != EOF_TOKEN)
    {
        parseServerConfig();
    }
    return serverConfigs;
}

void Parser::parseServerConfig()
{
    std::cout << "parse server token" << std::endl;
    parseToken(std::string("server"));
    std::cout << "parse open bracket" << std::endl;
    parseToken(std::string("{"));
    std::cout << "parse block directive" << std::endl;
    parseBlockDirective();
    std::cout << "parse closed bracket" << std::endl;
    parseToken(std::string("}"));
    position++;
}

void Parser::parseToken(std::string keyword)
{
    Token token = lexer.consume();
    if (token.value != keyword)
    {
        throw std::runtime_error("invalid keyword token type");
    }

    if (token.value == std::string("server"))
    {
        initServerConfig();
    }
}

void Parser::parseBlockDirective()
{
    Token token = lexer.consume();
    while (token.type != EOF_TOKEN)
    {
        std::cout << token.value << std::endl;
        if (token.type == KEYWORD && token.value == std::string("listen"))
        {
            buildListenDirective();
        }
    }
}

void Parser::initServerConfig()
{
    ServerConfig serverConfig;
    serverConfigs.push_back(serverConfig);
}

void Parser::buildListenDirective()
{
    Token token = lexer.consume();
    if (token.type != IDENTIFIER || !isValidPort(token.value))
    {
        // Colocar o valor aqui
        throw std::runtime_error("invalid port with value x");
    }
    int port = std::stoi(token.value);
    serverConfigs.at(position).setPort(port);
    lexer.consume();
}

bool Parser::isValidPort(std::string &input)
{
    if (input.empty())
    {
        return false; // Empty string is not a valid port
    }

    // Check if all characters in the input string are digits
    for (size_t i = 0; i < input.length(); ++i)
    {
        if (!isdigit(input[i]))
        {
            return false; // Non-numeric character found
        }
    }

    if (lexer.peek(1).type != SEMICOLON)
    {
        std::cout << lexer.peek(1).value << std::endl;
        return false;
    }

    // Convert the input string to an integer and check the range
    int port = std::stoi(input);
    return (port >= 1 && port <= 65535);
}