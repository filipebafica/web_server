#include "Parser.hpp"

void iterateThroughMap(const std::map<int, std::string>& myMap) {
    for (std::map<int, std::string>::const_iterator it = myMap.begin(); it != myMap.end(); ++it) {
        int key = it->first;
        std::string value = it->second;
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
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

    return 0;
}
