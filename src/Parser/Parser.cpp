#include <Parser.hpp>

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
        this->_httpMethods.insert(allowedMethods[i]);
    }
}

std::vector<ServerConfig> Parser::parse()
{
    while (this->_lexer.peek().type != EOF_TOKEN)
    {
        this->_parseServerConfig();
    }
    return this->_serverConfigs;
}

void Parser::_parseServerConfig()
{
    this->_parseToken(std::string("server"));
    this->_parseToken(std::string("{"));
    this->_parseServerBlock();
    this->_parseToken(std::string("}"));
    this->_position++;
}

void Parser::_parseToken(std::string token)
{
    Token currToken = this->_lexer.consume();
    if (currToken.value != token)
    {
        throw std::runtime_error("invalid token token type");
    }

    if (currToken.value == std::string("server"))
    {
        this->_initServerConfig();
    }
}

void Parser::_parseServerBlock()
{ 
    Token token = this->_lexer.consume();
    while (token.type != RIGHT_BRACE && token.type != EOF_TOKEN)
    {
        if (token.type == KEYWORD && token.value == std::string("listen"))
        {
            this->_parseListenDirective();
        } else if (token.type == KEYWORD && token.value == std::string("server_name"))
        {
            this->_parseServerNameDirective();
        } else if (token.type == KEYWORD && token.value == std::string("location"))
        {
            this->_parseLocationDirective();
        } else if (token.type == KEYWORD && token.value == std::string("error_page"))
        {
            this->_parseErrorPageDirective();
        } else if (token.type == KEYWORD && token.value == std::string("client_max_body_size"))
        {
            this->_parseClientMaxBodySizeDirective();
        }
        token = this->_lexer.peek();
    }
}

void Parser::_initServerConfig()
{
    ServerConfig serverConfig;
    this->_serverConfigs.push_back(serverConfig);
}

void Parser::_parseListenDirective()
{
    Token token = this->_lexer.consume();
    if (token.type != IDENTIFIER || !this->_isValidPort(token.value))
    {
        throw std::runtime_error("invalid port with value x");
    }
    int port = std::atoi(token.value.c_str());
    this->_serverConfigs[this->_position].setPort(port);
    this->_occupiedPorts.insert(port);
    this->_lexer.consume();
}

void Parser::_parseServerNameDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN) {
        if (!_isValidServerName(token.value)) {
            throw std::runtime_error("invalid server name");
        }
        this->_serverConfigs[this->_position].setServerName(token.value);
        token = this->_lexer.consume();
    }
}

void Parser::_parseLocationDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();
    
    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("location parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !this->_isValidAddress(token.value))
    {
        throw std::runtime_error("invalid route parameter");
    }

    this->_initLocationBlock();
    this->_serverConfigs.back().getLocations().back().setRoute(token.value);
    this->_lexer.consume();

    token = this->_lexer.peek();
    if (token.type != LEFT_BRACE) {
        throw std::runtime_error("invalid directive pattern");
    }

    this->_parseLocationBlock();

    this->_lexer.consume();
}

bool Parser::_isValidPort(std::string &input)
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

    if (this->_lexer.peek().type != SEMICOLON)
    {
        return false;
    }

    int port = std::atoi(input.c_str());
    if (port < 1 || port > 65535)
    {
        return false;
    }

    if (this->_occupiedPorts.find(port) != this->_occupiedPorts.end()) {
        return false;
    }

    return true;
}

bool Parser::_isValidServerName(std::string& serverName)
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

bool Parser::_isValidPath(std::string& path)
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

bool Parser::_isValidErrorCode(int code)
{
    return (code >= 100 && code <= 599);
}

bool Parser::_isValidErrorPageDirective(Token token)
{
    size_t idx = 0;
    std::vector<std::string> errorCodes;

    //valida se error_page tem path
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        errorCodes.push_back(token.value);
        idx++;
        token = this->_lexer.peek(idx);
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
        if (!this->_isValidErrorCode(std::atoi(errorCodes[idx].c_str()))) {
            return false;
        }
    }

    return true;
}

bool Parser::_isValidAddress(std::string address)
{
    // TODO: Check the line below
    (void)address;
    return true;
}

void Parser::_initLocationBlock()
{
    ServerLocation location;
    this->_serverConfigs.back().setLocationBlock(location);
}

void Parser::_parseLocationBlock()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();
    while (token.type != RIGHT_BRACE && token.type != EOF_TOKEN)
    {
        if (token.type == KEYWORD && token.value == std::string("root"))
        {
            this->_parseRootDirective();
        } else if (token.type == KEYWORD && token.value == std::string("index"))
        {
            this->_parseIndexDirective();
        } else if (token.type == KEYWORD && token.value == std::string("autoindex"))
        {
            this->_parseAutoIndexDirective();
        } else if (token.type == KEYWORD && token.value == std::string("proxy_pass"))
        {
            this->_parseProxyPassDirective();
        } else if (token.type == KEYWORD && token.value == std::string("alias"))
        {
            this->_parseAliasDirective();
        } else if (token.type == KEYWORD && token.value == std::string("try_files"))
        {
            this->_parseTryFilesDirective();
        } else if (token.type == KEYWORD && token.value == std::string("return"))
        {
            this->_parseReturnDirective();
        } else if (token.type == KEYWORD && token.value == std::string("allow"))
        {
            this->_parseAllowDirective();
        } else if (token.type == KEYWORD && token.value == std::string("deny"))
        {
            this->_parseDenyDirective();
        } else if (token.type == KEYWORD && token.value == std::string("allowed_methods"))
        {
            this->_parseAllowedMethods();
        }
        token = this->_lexer.peek();
    }
}

//Talvez seja interessante validar a existência do path
void Parser::_parseRootDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();
    if (token.type != IDENTIFIER || !_isValidPath(token.value)) {
        throw std::runtime_error("root argument is not a valid identifier");
    }

    this->_serverConfigs.back().getLocations().back().setRoot(token.value);

    this->_lexer.consume();
    token = this->_lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("root shouldn't have more than one argument");
    }

    this->_lexer.consume();
}

void Parser::_parseIndexDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();
    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER) {
            throw std::runtime_error("index argument it not valid identifier");
        }
        this->_serverConfigs.back().getLocations().back().setIndex(token.value);
        this->_lexer.consume();
        token = this->_lexer.peek();
    }
    this->_lexer.consume();
}

void Parser::_parseErrorPageDirective()
{
    this->_lexer.consume();
    Token token  = this->_lexer.peek();
    if (token.type != IDENTIFIER || !this->_isValidErrorPageDirective(token))
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
        token = this->_lexer.consume();
    }

    for (size_t idx = 0; idx < errorCodes.size(); idx++)
    {
        this->_serverConfigs.back().setErrorPages(errorCodes[idx], errorPagePath);
    }
    this->_lexer.consume();
}

void Parser::_parseClientMaxBodySizeDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    for (size_t i = 0; i < token.value.size(); i++)
    {
        if (!std::isdigit(token.value[i])) {
            throw std::runtime_error("client_max_body_size should be digits");
        }
    }
    
    if (!this->_isConvertibleToInt(token.value)) {
        throw std::runtime_error("client_max_body_size should be less than or equal MAX_INT");
    }

    this->_serverConfigs.back().setClientMaxBodySize(std::atoi(token.value.c_str()));
    this->_lexer.consume();
    
    token = this->_lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("client_max_body_size should have only one argument");
    }

    this->_lexer.consume();
}

void Parser::_parseAutoIndexDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.value != "on" && token.value != "off")
    {
        throw std::runtime_error("invalid use of autoindex directive");
    }
    this->_serverConfigs.back().getLocations().back().setAutoIndex(token.value);
    this->_lexer.consume();

    token = this->_lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("autoindex should have only one argument");
    }

    this->_lexer.consume();
}

void Parser::_parseProxyPassDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.value.find("http://") != 0 && token.value.find("https://") != 0) {
        throw std::runtime_error("invalid proxy_pass directive");
    }
    this->_serverConfigs.back().getLocations().back().setProxyPass(token.value);
    this->_lexer.consume();

    token = this->_lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("proxy_pass should have only one argument");
    }

    this->_lexer.consume();
}

void Parser::_parseAliasDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type != IDENTIFIER || !this->_isValidPath(token.value)) {
        throw std::runtime_error("alias argument is not a valid identifier");
    }

    this->_serverConfigs.back().getLocations().back().setAlias(token.value);

    this->_lexer.consume();
    token = this->_lexer.peek();
    if (token.type != SEMICOLON) {
        throw std::runtime_error("alias shouldn't have more than one argument");
    }

    this->_lexer.consume();
}

void Parser::_parseTryFilesDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("tryfiles parameter should not be empty");
    }

    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER || (!this->_isValidPath(token.value) && token.value[0] != '$'))
        {
            throw std::runtime_error("tryfiles argument is not a valid identifier");
        }
        this->_serverConfigs.back().getLocations().back().setTryFiles(token.value);
        this->_lexer.consume();
        token = this->_lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("tryfiles parameter should end with semicolon");
    }

    this->_lexer.consume();
}

void Parser::_parseReturnDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("return parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !this->_isValidErrorCode(std::atoi(token.value.c_str())))
    {
        throw std::runtime_error("invalid return parameter");
    }
    int code = std::atoi(token.value.c_str());
    this->_lexer.consume();
    token = this->_lexer.peek();

    std::string msg;
    if (token.type == SEMICOLON) {
        msg = "";
    } else {
        msg = token.value;
        this->_lexer.consume();
        token = this->_lexer.peek();
    }
    this->_serverConfigs.back().getLocations().back().setReturn(code, msg);

    if (token.type != SEMICOLON) {
        throw std::runtime_error("return directive should  be <error_code> <optional_msg>");
    }

    this->_lexer.consume();
}

void Parser::_parseAllowDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("allow parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !this->_isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    this->_serverConfigs.back().getLocations().back().setAllow(token.value);
    this->_lexer.consume();

    token = this->_lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allow directive should be <allowed ip address>");
    }

    this->_lexer.consume();
}

void Parser::_parseDenyDirective()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("deny parameter should not be empty");
    }

    if (token.type != IDENTIFIER || !this->_isValidAddress(token.value))
    {
        throw std::runtime_error("invalid address parameter");
    }
    this->_serverConfigs.back().getLocations().back().setDeny(token.value);
    this->_lexer.consume();

    token = this->_lexer.peek();
    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("deny directive should be <allowed ip address>");
    }

    this->_lexer.consume();    
}

void Parser::_parseAllowedMethods()
{
    this->_lexer.consume();
    Token token = this->_lexer.peek();

    if (token.type == SEMICOLON)
    {
        throw std::runtime_error("deny parameter should not be empty");
    }

    while (token.type != SEMICOLON && token.type != EOF_TOKEN)
    {
        if (token.type != IDENTIFIER || !this->_isValidHttpMethod(token.value))
        {
            throw std::runtime_error("invalid address parameter");
        }
        this->_serverConfigs.back().getLocations().back().setAllowedMethods(token.value);
        this->_lexer.consume();
        token = this->_lexer.peek();
    }

    if (token.type != SEMICOLON)
    {
        throw std::runtime_error("allowed_methods parameter should end with semicolon");
    }

    this->_lexer.consume();
}

bool Parser::_isValidHttpMethod(std::string allowedMethd)
{
    if (this->_httpMethods.find(allowedMethd) == this->_httpMethods.end())
    {
        return false;
    }

    return true;
}

bool Parser::_isConvertibleToInt(const std::string& str) {
    if (str.empty())
        return false;

    for (size_t i = 0; i < str.length(); i++) {
        if (!isdigit(str[i]))
            return false;
    }

    if (str.length() > 10)
        return false;

    return true;
}
