#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <vector>
#include <set>
#include "CharacterReader.hpp"

enum TokenType
{
    KEYWORD,
    IDENTIFIER,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    EOF_TOKEN
};

struct Token
{
    TokenType type;
    std::string value;
};

class Lexer
{
private:
    CharacterReader &reader;
    std::vector<Token> tokenBuffer;
    size_t position;

    // Aqui deveria ser usado unordered_set ao invés de set, mas esta dando erro
    std::set<std::string> keywords;

    void skipWhiteSpacesAndComments();

public:
    Lexer(CharacterReader &characterReader);

    Token peek();
    Token peek(int idx);

    Token consume();
    Token consume(int idx);
};

#endif // LEXER_HPP
