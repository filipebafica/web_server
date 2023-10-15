#include "Parser.hpp"

int main()
{
    // Create a CharacterReader with the nginx configuration
    CharacterReader reader("sample1.conf");
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

    return 0;
}
