#include "Lexer.hpp"

int main() {
    // Create a CharacterReader with the nginx configuration
    CharacterReader reader("nginx.conf");
    // Create an NginxLexer with the CharacterReader
    Lexer lexer(reader);

    // Use the lexer to tokenize the input
    Token token;
    while (true) {
        token = lexer.consume();
        if (token.type == EOF_TOKEN) {
            break;
        }
        std::cout << "Token Type: " << token.type << ", Value: " << token.value << std::endl;
    }

    return 0;
}
