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
public:
    Lexer(const char* filePath);

    Token peek();
    Token peek(int idx);

    Token consume();
    Token consume(int idx);

private:
    CharacterReader _reader;
    std::vector<Token> _tokenBuffer;
    size_t position;

    // Aqui deveria ser usado unordered_set ao invés de set, mas esta dando erro
    std::set<std::string> _keywords;

    void skipWhiteSpacesAndComments();
};

#endif // LEXER_HPP
