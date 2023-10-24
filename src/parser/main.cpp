#include "Parser.hpp"

void iterateThroughMap(const std::map<int, std::string>& myMap) {
    std::cout << "ERROR PAGES: " << std::endl;

    for (std::map<int, std::string>::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
        int key = it->first;
        std::string value = it->second;
        std::cout << "\tKey: " << key << ", Value: " << value << std::endl;
    }
}

int main()
{
    // Create a CharacterReader with the nginx configuration
    CharacterReader reader("sample5.conf");
    // Create an NginxLexer with the CharacterReader
    Lexer lexer(reader);

    std::cout << "***** PRINTING PRIMARY METHODS *****" << std::endl;

    // Use the lexer to tokenize the input
    Token token;
    Parser parser(lexer);
    // while (true)
    // {
    //     token = lexer.consume();
    //     if (token.type == EOF_TOKEN) {
    //         break;
    //     }
    //     std::cout << "Token Type: " << token.type << ", Value: " << token.value << std::endl;
    // }
    std::vector<ServerConfig> serverConfig = parser.parse();

    for (int i = 0; i < serverConfig.size(); i++) {
        std::cout << "PRINTING SERVER: " << i << std::endl;

        std::vector<std::string> serverName = serverConfig[i].getServerName();
        std::cout << "SERVER NAME: " << serverName[0] << std::endl;

        std::string route = serverConfig[i].getLocations().back().getRoute();
        std::cout << "ROUTE: " << route << std::endl;

        std::string root = serverConfig[i].getLocations().back().getRoot();
        std::cout << "ROOT: " << root << std::endl;

        std::vector<std::string> index = serverConfig[i].getLocations().back().getIndexes();

        std::cout << "INDEX: " << index.back() << std::endl;

        std::map<int, std::string> errorPages = serverConfig[i].getErrorPages();
        iterateThroughMap(errorPages);

        std::cout << "MAX_BODY_SIZE: " << serverConfig[i].getClientMaxBodySize() << std::endl;

        std::string autoindex = serverConfig[i].getLocations().back().getAutoIndex();
        std::cout << "AUTOINDEX: " << autoindex << std::endl;

        std::string proxyPass = serverConfig[i].getLocations().back().getProxyPass();
        std::cout << "PROXY_PASS: " << proxyPass << std::endl;

        std::string alias = serverConfig[i].getLocations().back().getAlias();
        std::cout << "ALIAS: " << alias << std::endl;

        std::vector<std::string> tryfiles = serverConfig[i].getLocations().back().getTryFiles();
        std::cout << "TRY FILES: " << tryfiles[0] << std::endl;

        std::map<int, std::string> returnDirective = serverConfig[i].getLocations().back().getReturn();
        iterateThroughMap(returnDirective);

        std::vector<std::string> allow = serverConfig[i].getLocations().back().getAllow();
        std::cout << "ALLOW: " << allow[0] << std::endl;

        std::vector<std::string> deny = serverConfig[i].getLocations().back().getDeny();
        std::cout << "DENY: " << deny[0] << std::endl;

        std::vector<std::string> allowedMethods = serverConfig[i].getLocations().back().getAllowedMethods();
        std::cout << "ALLOWED_METHODS: " << allowedMethods[0] << std::endl;

        std::cout << std::endl;
    }
    {
        std::cout << "***** PRINTING FUNCTIONAL METHODS *****" << std::endl;

        std::string resource = serverConfig[0].getResources("POST", "/videos/");
        std::cout << "LOCATION POSITION: " << resource << std::endl;
    }


    return 0;
}
