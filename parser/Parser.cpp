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
    parseToken(std::string("server"));
    parseToken(std::string("{"));
    std::cout << "parse server block" << std::endl;
    parseServerBlock();
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

void Parser::parseServerBlock()
{ 
    Token token = lexer.consume();
    while (token.type != RIGHT_BRACE && token.type != EOF_TOKEN)
    {
        if (token.type == KEYWORD && token.value == std::string("listen"))
        {
            parseListenDirective();
        } else if (token.type == KEYWORD && token.value == std::string("server_name"))
        {
            parseServerNameDirective();
        } else if (token.type == KEYWORD && token.value == std::string("location"))
        {
            parseLocationDirective();
        } else if (token.type == KEYWORD && token.value == std::string("error_page"))
        {
            std::cout << "parse error page directive" << std::endl;
            parseErrorPageDirective();
        }
        token = lexer.peek();
    }
}

void Parser::initServerConfig()
{
    ServerConfig serverConfig;
    serverConfigs.push_back(serverConfig);
}

void Parser::parseListenDirective()
{
    Token token = lexer.consume();
    if (token.type != IDENTIFIER || !isValidPort(token.value))
    {
        throw std::runtime_error("invalid port with value x");
    }
    int port = std::stoi(token.value);
    serverConfigs[position].setPort(port);
    lexer.consume();
}

void Parser::parseServerNameDirective()
{
    lexer.consume();
    Token token = lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN) {
        if (!isValidServerName(token.value)) {
            throw std::runtime_error("invalid server name");
        }
        serverConfigs[position].setServerName(token.value);
        token = lexer.consume();
    }
}

void Parser::parseLocationDirective()
{
    lexer.consume();
    if (lexer.peek(1).type != LEFT_BRACE && lexer.peek(2).type != LEFT_BRACE) {
        throw std::runtime_error("invalid directive pattern");
    }
    lexer.consume();
    parseLocationBlock();
    lexer.consume();
}

bool Parser::isValidPort(std::string &input)
{
    if (input.empty())
    {
        return false;
    }

    for (size_t i = 0; i < input.length(); ++i)
    {
        if (!isdigit(input[i]))
        {
            return false;
        }
    }

    if (lexer.peek().type != SEMICOLON)
    {
        return false;
    }

    int port = std::stoi(input);
    return port >= 1 && port <= 65535;
}

bool Parser::isValidServerName(std::string& serverName)
{
    if (serverName.empty()) {
        return false;
    }

    if (serverName.length() > 255) {
        return false;
    }

    if (!isalnum(serverName[0]) || !isalnum(serverName[serverName.length() - 1])) {
        return false;
    }

    for (size_t i = 0; i < serverName.length(); ++i) {
        char c = serverName[i];
        if (!(isalnum(c) || c == '-' || c == '.')) {
            return false;
        }
    }

    return true;
}

bool Parser::isValidRoot(std::string& rootPath)
{
    if (rootPath.empty()) {
        std::cout << "Error: The root rootPath cannot be empty." << std::endl;
        return false;
    }

    if (rootPath.find('\\') != std::string::npos) {
        std::cout << "Error: Use Unix-style rootPath separators '/' in the root directive." << std::endl;
        return false;
    }

    if (rootPath[0] != '/') {
        std::cout << "Error: The root rootPath must be an absolute rootPath." << std::endl;
        return false;
    }

    return true;
}

bool Parser::isValidErrorCode(int code)
{
    return (code >= 100 && code <= 599);
}

/**
 * @todo: Melhorar esta validação ou limitar argumentos, pois nginx
 * lida com essa diretiva de varias maneiras. Todos os códigos de erro
 * definidos antes do caminho da página são associados aquele path
 * códigos de erro definidos depois utilizam o default, se eu definir
 * um path mais de uma vez ai os códigos de erro usarão o ultimo path.
 */
bool Parser::isValidErrorPageDirective(Token token)
{
    int idx = 0;
    std::vector<std::string> errorCodes;

    //valida se error_page tem path
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        errorCodes.push_back(token.value);
        idx++;
        token = lexer.peek(idx);
    }

    std::string pagePath = errorCodes.back();
    if (pagePath[0] != '/') {
        return false;
    }
    errorCodes.pop_back();

    if (!errorCodes.size()) {
        return false;
    }

    for (idx = 0; idx < errorCodes.size(); idx++) {
        if (!isValidErrorCode(std::stoi(errorCodes[idx]))) {
            return false;
        }
    }

    return true;
}

void Parser::initLocationBlock()
{
    ServerConfig::Location location;
    this->serverConfigs.back().setLocationBlock(location);
}

void Parser::parseLocationBlock()
{
    initLocationBlock();
    lexer.consume();
    Token token = lexer.peek();
    while (token.type != RIGHT_BRACE && token.type != EOF_TOKEN)
    {
        if (token.type == KEYWORD && token.value == std::string("root")) {
            parseRootDirective();
        } else if (token.type == KEYWORD && token.value == std::string("index")) {
            parseIndexDirective();
        }
        token = lexer.peek();
    }
}

//Talvez seja interessante validar a existência do path
void Parser::parseRootDirective()
{
    lexer.consume();
    Token token = lexer.peek();
    if (token.type != IDENTIFIER || !isValidRoot(token.value)) {
        throw std::runtime_error("root argument is not a valid identifier");
    }

    this->serverConfigs.back().getLocation().back().setRoot(token.value);

    lexer.consume();
    token = lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("root shouldn't have more than one argument");
    }
    lexer.consume();
}

void Parser::parseIndexDirective()
{
    lexer.consume();
    Token token = lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER) {
            throw std::runtime_error("index argument it not valid identifier");
        }
        serverConfigs.back().getLocation().back().setIndex(token.value);
        lexer.consume();
        token = lexer.peek();
    }
    lexer.consume();
}

void Parser::parseErrorPageDirective()
{
    lexer.consume();
    Token token  = lexer.peek();
    if (token.type != IDENTIFIER || !isValidErrorPageDirective(token))
    {
        throw std::runtime_error("error_page directive should be: <error_codes1 error_code2 ...> <error_page_path>");
    }

    std::vector<int> errorCodes;
    std::string errorPagePath;
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.value[0] == '/') {
            errorPagePath = token.value;
            break;
        }
        errorCodes.push_back(std::stoi(token.value));
        token = lexer.consume();
    }

    for (int idx = 0; idx < errorCodes.size(); idx++)
    {
        serverConfigs.back().setErrorPages(errorCodes[idx], errorPagePath);
    }
    lexer.consume();
}
