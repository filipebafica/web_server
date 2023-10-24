// Implements the parser recognition

/**
 * @todo: Será necessário revisar todas as validações criando testes
 * 
 */

#include "Parser.hpp"

Parser::Parser(Lexer &lexer) : lexer(lexer), position(0)
{
    const char *allowedMethods[] = {
        "GET",
        "POST",
        "PUT",
        "DELETE",
        "PATCH",
        "HEAD",
        "OPTIONS",
        "TRACE",
        "\0",
    };

    for (size_t i = 0; allowedMethods[i] != "\0"; i++)
    {
        _httpMethods.insert(allowedMethods[i]);
    }
}

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
            parseErrorPageDirective();
        } else if (token.type == KEYWORD && token.value == std::string("client_max_body_size"))
        {
            parseClientMaxBodySizeDirective();
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
    Token token = lexer.peek();
    
    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("location parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid route parameter");
    }

    initLocationBlock();
    serverConfigs.back().getLocations().back().setRoute(token.value);
    lexer.consume();

    token = lexer.peek();
    if (token.type != LEFT_BRACE) {
        throw std::runtime_error("invalid directive pattern");
    }

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

bool Parser::isValidPath(std::string& path)
{
    if (path.empty()) {
        return false;
    }

    if (path.find('\\') != std::string::npos) {
        return false;
    }

    if (path[0] != '/') {
        return false;
    }

    return true;
}

bool Parser::isValidErrorCode(int code)
{
    return (code >= 100 && code <= 599);
}

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

bool Parser::isValidAddress(std::string address)
{
    return true;
}

void Parser::initLocationBlock()
{
    ServerLocation location;
    this->serverConfigs.back().setLocationBlock(location);
}

void Parser::parseLocationBlock()
{
    lexer.consume();
    Token token = lexer.peek();
    while (token.type != RIGHT_BRACE && token.type != EOF_TOKEN)
    {
        if (token.type == KEYWORD && token.value == std::string("root"))
        {
            parseRootDirective();
        } else if (token.type == KEYWORD && token.value == std::string("index"))
        {
            parseIndexDirective();
        } else if (token.type == KEYWORD && token.value == std::string("autoindex"))
        {
            parseAutoIndexDirective();
        } else if (token.type == KEYWORD && token.value == std::string("proxy_pass"))
        {
            parseProxyPassDirective();
        } else if (token.type == KEYWORD && token.value == std::string("alias"))
        {
            parseAliasDirective();
        } else if (token.type == KEYWORD && token.value == std::string("try_files"))
        {
            parseTryFilesDirective();
        } else if (token.type == KEYWORD && token.value == std::string("return"))
        {
            parseReturnDirective();
        } else if (token.type == KEYWORD && token.value == std::string("allow"))
        {
            parseAllowDirective();
        } else if (token.type == KEYWORD && token.value == std::string("deny"))
        {
            parseDenyDirective();
        } else if (token.type == KEYWORD && token.value == std::string("allowed_methods"))
        {
            parseAllowedMethods();
        }
        token = lexer.peek();
    }
}

//Talvez seja interessante validar a existência do path
void Parser::parseRootDirective()
{
    lexer.consume();
    Token token = lexer.peek();
    if (token.type != IDENTIFIER || !isValidPath(token.value)) {
        throw std::runtime_error("root argument is not a valid identifier");
    }

    this->serverConfigs.back().getLocations().back().setRoot(token.value);

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
        serverConfigs.back().getLocations().back().setIndex(token.value);
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

void Parser::parseClientMaxBodySizeDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    char lastChar = token.value[token.value.size() - 1];
    if (lastChar != 'M' && lastChar != 'K' && lastChar != 'G')
    {
        throw std::runtime_error("client_max_body_size must be in K, M OR G");
    }

    for (int i = 0; i < token.value.size() - 1; i++)
    {
        if (!std::isdigit(token.value[i])) {
            throw std::runtime_error("client_max_body_size should be digits");
        }
    }
    serverConfigs.back().setClientMaxBodySize(token.value);
    lexer.consume();
    
    token = lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("client_max_body_size should have only one argument");
    }

    lexer.consume();
}

void Parser::parseAutoIndexDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.value != "on" && token.value != "off")
    {
        throw std::runtime_error("invalid use of autoindex directive");
    }
    serverConfigs.back().getLocations().back().setAutoIndex(token.value);
    lexer.consume();

    token = lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("autoindex should have only one argument");
    }

    lexer.consume();
}

void Parser::parseProxyPassDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.value.find("http://") != 0 && token.value.find("https://") != 0) {
        throw std::runtime_error("invalid proxy_pass directive");
    }
    serverConfigs.back().getLocations().back().setProxyPass(token.value);
    lexer.consume();

    token = lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("proxy_pass should have only one argument");
    }

    lexer.consume();
}

void Parser::parseAliasDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type != IDENTIFIER || !isValidPath(token.value)) {
        throw std::runtime_error("alias argument is not a valid identifier");
    }

    this->serverConfigs.back().getLocations().back().setAlias(token.value);

    lexer.consume();
    token = lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("alias shouldn't have more than one argument");
    }

    lexer.consume();
}

void Parser::parseTryFilesDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("tryfiles parameter should not be empty");
    }

    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER || (!isValidPath(token.value) && token.value[0] != '$'))
        {
            throw std::runtime_error("tryfiles argument is not a valid identifier");
        }
        serverConfigs.back().getLocations().back().setTryFiles(token.value);
        lexer.consume();
        token = lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("tryfiles parameter should end with semicolon");
    }

    lexer.consume();
}

void Parser::parseReturnDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("return parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidErrorCode(std::stoi(token.value)))
    {
        throw std::runtime_error("invalid return parameter");
    }
    int code = std::stoi(token.value);
    lexer.consume();
    token = lexer.peek();

    std::string msg;
    if (token.type == SEMICOLON) {
        msg = "";
    } else {
        msg = token.value;
        lexer.consume();
        token = lexer.peek();
    }
    serverConfigs.back().getLocations().back().setReturn(code, msg);

    if (token.type != SEMICOLON) {
        throw std::runtime_error("return directive should  be <error_code> <optional_msg>");
    }

    lexer.consume();
}

void Parser::parseAllowDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("allow parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    serverConfigs.back().getLocations().back().setAllow(token.value);
    lexer.consume();

    token = lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allow directive should be <allowed ip address>");
    }

    lexer.consume();
}

void Parser::parseDenyDirective()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("deny parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    serverConfigs.back().getLocations().back().setDeny(token.value);
    lexer.consume();

    token = lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("deny directive should be <allowed ip address>");
    }

    lexer.consume();    
}

void Parser::parseAllowedMethods()
{
    lexer.consume();
    Token token = lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("deny parameter should not be empty");
    }

    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER || !isValidHttpMethod(token.value))
        {
            throw std::runtime_error("invalid address parameter");
        }
        serverConfigs.back().getLocations().back().setAllowedMethods(token.value);
        lexer.consume();
        token = lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allowed_methods parameter should end with semicolon");
    }

    lexer.consume();
}

bool Parser::isValidHttpMethod(std::string allowedMethd)
{
    if (_httpMethods.find(allowedMethd) == _httpMethods.end())
    {
        return false;
    }

    return true;
}
