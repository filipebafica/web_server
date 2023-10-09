#include "Lexer.hpp"

/**
 * @todo It should be implemented tests
 */
Lexer::Lexer(CharacterReader& characterReader) : reader(characterReader), position(0) {

    const char* allowedKeywords[] = {
        "server",
        "location",
        "listen",
        "error_page",
        "client_max_body_size",
        "autoindex",
        "\0",
    };

    for (size_t i = 0; allowedKeywords[i] != "\0"; i++) {
        keywords.push_back(allowedKeywords[i]);
    }
}


void Lexer::skipWhiteSpacesAndComments() {
    char ch = reader.peek();

    while (std::isspace(ch)) {
        if (ch == '#') {
            while (ch != '\n' && ch != EOF) {
                ch = reader.consume();
            }
        } else {
            ch = reader.consume();
        }
        ch = reader.peek();
    }
}

Token Lexer::peek() {
    skipWhiteSpacesAndComments();

    while (tokenBuffer.size() <= position) {
        char ch = reader.peek();
        Token token;

        if (ch == EOF) {
            token.type = EOF_TOKEN;
            token.value = "";
            tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';') {
            token.type = SEMICOLON;
            token.value = ";";
            reader.consume();
        } else if (ch == '{') {
            token.type = LEFT_BRACE;
            token.value = "{";
            reader.consume();
        } else if (ch == '}') {
            token.type = RIGHT_BRACE;
            token.value = "}";
            reader.consume();
        } else if (isalpha(ch)) {
            reader.consume();
            token.type = KEYWORD;
            token.value = "";

            while ((ch = reader.consume()) != '"' && ch != EOF) {
                token.value += ch;
            }
        } else {
            token.type = IDENTIFIER;
            token.value = "";

            while (!isspace(ch) && ch != ';' && ch != '{' && ch != '}' && ch != '#' && ch != EOF) {
                token.value += ch;
                reader.consume();
                ch = reader.peek();
            }
        }

        tokenBuffer.push_back(token);
    }

    return tokenBuffer[position];
}

Token Lexer::peek(int idx) {
    while (tokenBuffer.size() <= position + idx) {
        peek();
    }

    return tokenBuffer[position + idx];
}

Token Lexer::consume() {
    Token token = peek();
    position++;
    return token;
}

Token Lexer::consume(int idx) {
    Token token = peek();
    position += idx;
    return token;
}
