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

void Parser::parseToken(std::string token)
{
    Token currToken = lexer.consume();
    if (currToken.value != token)
    {
        throw std::runtime_error("invalid token token type");
    }

    if (currToken.value == std::string("server"))
    {
        initServerConfig();
    }
}

void Parser::parseBlockDirective()
{ 
    Token token = lexer.consume();
    while (token.type != RIGHT_BRACE)
    {
        if (token.type == KEYWORD && token.value == std::string("listen"))
        {
            buildListenDirective();
        } else if (token.type == KEYWORD && token.value == std::string("server_name")) {
            std::runtime_error("");
            buildServerNameDirective();
        } else if (token.type == KEYWORD && token.value == std::string("location")) {
            buildLocationDirection();
        }
        token = lexer.peek();
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
    serverConfigs[position].setPort(port);
    lexer.consume();
}

void Parser::buildServerNameDirective()
{
    lexer.consume();
    lexer.consume();
    lexer.consume();
}

void Parser::buildLocationDirection()
{
    Token token = lexer.consume();
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

    if (lexer.peek().type != SEMICOLON)
    {
        return false;
    }

    // Convert the input string to an integer and check the range
    int port = std::stoi(input);
    return (port >= 1 && port <= 65535);
}
