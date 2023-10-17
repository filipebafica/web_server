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
    std::string root = serverConfig.back().getLocation().back().getRoot();

    std::cout << "ROOT: " << root << std::endl;

    std::vector<std::string> index = serverConfig.back().getLocation().back().getIndexes();

    std::cout << "INDEX: " << index.back() << std::endl;

    std::map<int, std::string> errorPages = serverConfig.back().getErrorPages();
    iterateThroughMap(errorPages);

    std::cout << "MAX_BODY_SIZE: " << serverConfig.back().getClientMaxBodySize() << std::endl;

    std::string autoindex = serverConfig.back().getLocation().back().getAutoIndex();
    std::cout << "AUTOINDEX: " << autoindex << std::endl;

    std::string proxyPass = serverConfig.back().getLocation().back().getProxyPass();
    std::cout << "PROXY_PASS: " << proxyPass << std::endl;

    std::string alias = serverConfig.back().getLocation().back().getAlias();
    std::cout << "ALIAS: " << alias << std::endl;

    std::vector<std::string> tryfiles = serverConfig.back().getLocation().back().getTryFiles();
    std::cout << "TRY FILES: " << tryfiles[0] << std::endl;

    std::map<int, std::string> returnDirective = serverConfig.back().getLocation().back().getReturn();
    iterateThroughMap(returnDirective);

    return 0;
}
