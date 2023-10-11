#include "Lexer.hpp"

int main(void)
{
    // {
    //     std::cout << "TEST ONE" << std::endl;
    //     // Create a CharacterReader with the nginx configuration
    //     CharacterReader reader("sample1.conf");
    //     // Create an NginxLexer with the CharacterReader
    //     Lexer lexer(reader);

    //     // Use the lexer to tokenize the input
    //     Token token;
    //     while (true)
    //     {
    //         token = lexer.consume();
    //         if (token.type == EOF_TOKEN)
    //         {
    //             break;
    //         }
    //         std::cout << "Token Type: " << token.type << ", Value: " << token.value << std::endl;
    //     }
    // }

    std::cout << std::endl << "TEST TWO" << std::endl;
    {
        CharacterReader reader("sample1.conf");
        Lexer lexer(reader);
        Token token;

        token = lexer.peek(5);
        std::cout << "Token Type: " << token.type << ", Value: " << token.value << std::endl;
    }

    return (0);
}