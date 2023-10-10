#include "Lexer.hpp"

/**
 * @todo It should be implemented tests
 * @todo Write all functions implementation detail
 */
Lexer::Lexer(CharacterReader &characterReader) : reader(characterReader), position(0)
{

    const char *allowedKeywords[] = {
        "server",
        "location",
        "listen",
        "error_page",
        "client_max_body_size",
        "autoindex",
        "root",
        "index",
        "\0",
    };

    for (size_t i = 0; allowedKeywords[i] != "\0"; i++)
    {
        keywords.insert(allowedKeywords[i]);
    }
}

void Lexer::skipWhiteSpacesAndComments()
{
    char ch = reader.peek();

    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '#')
    {
        if (ch == '#')
        {
            while (ch != '\n' && ch != EOF)
            {
                ch = reader.consume();
            }
        }
        else
        {
            ch = reader.consume();
        }
        ch = reader.peek();
    }
}

Token Lexer::peek()
{
    skipWhiteSpacesAndComments();

    while (tokenBuffer.size() <= position)
    {
        char ch = reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = EOF_TOKEN;
            token.value = "";
            tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';')
        {
            token.type = SEMICOLON;
            token.value = ";";
            reader.consume();
        }
        else if (ch == '{')
        {
            token.type = LEFT_BRACE;
            token.value = "{";
            reader.consume();
        }
        else if (ch == '}')
        {
            token.type = RIGHT_BRACE;
            token.value = "}";
            reader.consume();
        }
        else if (ch == '"')
        {
            reader.consume();
            token.value = "";

            while ((ch = reader.consume()) != '"' && ch != EOF)
            {
                token.value += ch;
            }

            if (keywords.find(token.value) == keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }
        else
        {
            token.value = "";

            while (!isspace(ch) && ch != ';' && ch != '{' && ch != '}' && ch != '#' && ch != EOF)
            {
                token.value += ch;
                reader.consume();
                ch = reader.peek();
            }

            if (keywords.find(token.value) == keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }

        tokenBuffer.push_back(token);
    }

    return tokenBuffer[position];
}

Token Lexer::peek(int idx)
{
    while (tokenBuffer.size() <= position + idx)
    {
        char ch = reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = TokenType::EOF_TOKEN;
            token.value = "";
            tokenBuffer.push_back(token);
            break;
        }

        tokenBuffer.push_back(token);
    }

    return tokenBuffer[position + idx];
}

Token Lexer::consume()
{
    Token token = peek();
    position++;
    return token;
}

Token Lexer::consume(int idx)
{
    Token token = peek(idx);
    position += idx;
    return token;
}
