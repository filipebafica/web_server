#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <vector>
#include "CharacterReader.hpp"

enum TokenType {
    IDENTIFIER,
    SEMICOLON,
    LEFT_BRACE,
    RIGHT_BRACE,
    KEYWORD,
    EOF_TOKEN
};

struct Token {
    TokenType   type;
    std::string value;
};

class Lexer {
    private:
        CharacterReader&            reader;
        std::vector<Token>          tokenBuffer;
        size_t                      position;
        std::vector<std::string>    keywords;

        void skipWhiteSpacesAndComments();

    public:
        Lexer(CharacterReader& characterReader);
        
        Token peek();
        Token peek(int idx);

        Token consume();
        Token consume(int idx);
};

#endif // LEXER_HPP
