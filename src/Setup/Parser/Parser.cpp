// Implements the parser recognition

/**
 * @todo: Será necessário revisar todas as validações criando testes
 * 
 */

#include <cstdlib>
#include "Parser.hpp"

Parser::Parser(const char* filePath) : _lexer(filePath), _position(0)
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
        NULL,
    };

    for (size_t i = 0; allowedMethods[i] != NULL; i++)
    {
        _httpMethods.insert(allowedMethods[i]);
    }
}

std::vector<ServerConfig> Parser::parse()
{
    while (_lexer.peek().type != EOF_TOKEN)
    {
        parseServerConfig();
    }
    return _serverConfigs;
}

void Parser::parseServerConfig()
{
    parseToken(std::string("server"));
    parseToken(std::string("{"));
    parseServerBlock();
    parseToken(std::string("}"));
    _position++;
}

void Parser::parseToken(std::string token)
{
    Token currToken = _lexer.consume();
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
    Token token = _lexer.consume();
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
        token = _lexer.peek();
    }
}

void Parser::initServerConfig()
{
    ServerConfig serverConfig;
    _serverConfigs.push_back(serverConfig);
}

void Parser::parseListenDirective()
{
    Token token = _lexer.consume();
    if (token.type != IDENTIFIER || !isValidPort(token.value))
    {
        throw std::runtime_error("invalid port with value x");
    }
    int port = std::atoi(token.value.c_str());
    _serverConfigs[_position].setPort(port);
    _occupiedPorts.insert(port);
    _lexer.consume();
}

void Parser::parseServerNameDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN) {
        if (!isValidServerName(token.value)) {
            throw std::runtime_error("invalid server name");
        }
        _serverConfigs[_position].setServerName(token.value);
        token = _lexer.consume();
    }
}

void Parser::parseLocationDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();
    
    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("location parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid route parameter");
    }

    initLocationBlock();
    _serverConfigs.back().getLocations().back().setRoute(token.value);
    _lexer.consume();

    token = _lexer.peek();
    if (token.type != LEFT_BRACE) {
        throw std::runtime_error("invalid directive pattern");
    }

    parseLocationBlock();

    _lexer.consume();
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

    if (_lexer.peek().type != SEMICOLON)
    {
        return false;
    }

    int port = std::atoi(input.c_str());
    if (port < 1 || port > 65535)
    {
        return false;
    }

    if (_occupiedPorts.find(port) != _occupiedPorts.end()) {
        return false;
    }

    return true;
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
    size_t idx = 0;
    std::vector<std::string> errorCodes;

    //valida se error_page tem path
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        errorCodes.push_back(token.value);
        idx++;
        token = _lexer.peek(idx);
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
        if (!isValidErrorCode(std::atoi(errorCodes[idx].c_str()))) {
            return false;
        }
    }

    return true;
}

bool Parser::isValidAddress(std::string address)
{
    // TODO: Check the line below
    (void)address;
    return true;
}

void Parser::initLocationBlock()
{
    ServerLocation location;
    this->_serverConfigs.back().setLocationBlock(location);
}

void Parser::parseLocationBlock()
{
    _lexer.consume();
    Token token = _lexer.peek();
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
        token = _lexer.peek();
    }
}

//Talvez seja interessante validar a existência do path
void Parser::parseRootDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();
    if (token.type != IDENTIFIER || !isValidPath(token.value)) {
        throw std::runtime_error("root argument is not a valid identifier");
    }

    this->_serverConfigs.back().getLocations().back().setRoot(token.value);

    _lexer.consume();
    token = _lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("root shouldn't have more than one argument");
    }

    _lexer.consume();
}

void Parser::parseIndexDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER) {
            throw std::runtime_error("index argument it not valid identifier");
        }
        _serverConfigs.back().getLocations().back().setIndex(token.value);
        _lexer.consume();
        token = _lexer.peek();
    }
    _lexer.consume();
}

void Parser::parseErrorPageDirective()
{
    _lexer.consume();
    Token token  = _lexer.peek();
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
        errorCodes.push_back(std::atoi(token.value.c_str()));
        token = _lexer.consume();
    }

    for (size_t idx = 0; idx < errorCodes.size(); idx++)
    {
        _serverConfigs.back().setErrorPages(errorCodes[idx], errorPagePath);
    }
    _lexer.consume();
}

void Parser::parseClientMaxBodySizeDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    char lastChar = token.value[token.value.size() - 1];
    if (lastChar != 'M' && lastChar != 'K' && lastChar != 'G')
    {
        throw std::runtime_error("client_max_body_size must be in K, M OR G");
    }

    for (size_t i = 0; i < token.value.size() - 1; i++)
    {
        if (!std::isdigit(token.value[i])) {
            throw std::runtime_error("client_max_body_size should be digits");
        }
    }
    _serverConfigs.back().setClientMaxBodySize(token.value);
    _lexer.consume();
    
    token = _lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("client_max_body_size should have only one argument");
    }

    _lexer.consume();
}

void Parser::parseAutoIndexDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.value != "on" && token.value != "off")
    {
        throw std::runtime_error("invalid use of autoindex directive");
    }
    _serverConfigs.back().getLocations().back().setAutoIndex(token.value);
    _lexer.consume();

    token = _lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("autoindex should have only one argument");
    }

    _lexer.consume();
}

void Parser::parseProxyPassDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.value.find("http://") != 0 && token.value.find("https://") != 0) {
        throw std::runtime_error("invalid proxy_pass directive");
    }
    _serverConfigs.back().getLocations().back().setProxyPass(token.value);
    _lexer.consume();

    token = _lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("proxy_pass should have only one argument");
    }

    _lexer.consume();
}

void Parser::parseAliasDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.type != IDENTIFIER || !isValidPath(token.value)) {
        throw std::runtime_error("alias argument is not a valid identifier");
    }

    this->_serverConfigs.back().getLocations().back().setAlias(token.value);

    _lexer.consume();
    token = _lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("alias shouldn't have more than one argument");
    }

    _lexer.consume();
}

void Parser::parseTryFilesDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

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
        _serverConfigs.back().getLocations().back().setTryFiles(token.value);
        _lexer.consume();
        token = _lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("tryfiles parameter should end with semicolon");
    }

    _lexer.consume();
}

void Parser::parseReturnDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("return parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidErrorCode(std::atoi(token.value.c_str())))
    {
        throw std::runtime_error("invalid return parameter");
    }
    int code = std::atoi(token.value.c_str());
    _lexer.consume();
    token = _lexer.peek();

    std::string msg;
    if (token.type == SEMICOLON) {
        msg = "";
    } else {
        msg = token.value;
        _lexer.consume();
        token = _lexer.peek();
    }
    _serverConfigs.back().getLocations().back().setReturn(code, msg);

    if (token.type != SEMICOLON) {
        throw std::runtime_error("return directive should  be <error_code> <optional_msg>");
    }

    _lexer.consume();
}

void Parser::parseAllowDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("allow parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    _serverConfigs.back().getLocations().back().setAllow(token.value);
    _lexer.consume();

    token = _lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allow directive should be <allowed ip address>");
    }

    _lexer.consume();
}

void Parser::parseDenyDirective()
{
    _lexer.consume();
    Token token = _lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("deny parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    _serverConfigs.back().getLocations().back().setDeny(token.value);
    _lexer.consume();

    token = _lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("deny directive should be <allowed ip address>");
    }

    _lexer.consume();    
}

void Parser::parseAllowedMethods()
{
    _lexer.consume();
    Token token = _lexer.peek();

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
        _serverConfigs.back().getLocations().back().setAllowedMethods(token.value);
        _lexer.consume();
        token = _lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allowed_methods parameter should end with semicolon");
    }

    _lexer.consume();
}

bool Parser::isValidHttpMethod(std::string allowedMethd)
{
    if (_httpMethods.find(allowedMethd) == _httpMethods.end())
    {
        return false;
    }

    return true;
}
