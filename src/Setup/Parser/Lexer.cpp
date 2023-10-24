#include "Lexer.hpp"

/**
 * @todo It should be implemented tests
 * @todo Write all functions implementation detail
 */
Lexer::Lexer(const char* filePath) : _reader(filePath), position(0)
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
        "server_name",
        "proxy_pass",
        "alias",
        "try_files",
        "return",
        "allow",
        "deny",
        "allowed_methods",
        NULL,
    };

    for (size_t i = 0; allowedKeywords[i] != NULL; i++)
    {
        _keywords.insert(allowedKeywords[i]);
    }
}

void Lexer::skipWhiteSpacesAndComments()
{
    char ch = _reader.peek();

    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '#')
    {
        if (ch == '#')
        {
            while (ch != '\n' && ch != EOF)
            {
                ch = _reader.consume();
            }
        }
        else
        {
            ch = _reader.consume();
        }
        ch = _reader.peek();
    }
}

Token Lexer::peek()
{
    skipWhiteSpacesAndComments();

    while (_tokenBuffer.size() <= position)
    {
        char ch = _reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = EOF_TOKEN;
            token.value = "";
            _tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';')
        {
            token.type = SEMICOLON;
            token.value = ";";
            _reader.consume();
        }
        else if (ch == '{')
        {
            token.type = LEFT_BRACE;
            token.value = "{";
            _reader.consume();
        }
        else if (ch == '}')
        {
            token.type = RIGHT_BRACE;
            token.value = "}";
            _reader.consume();
        }
        else if (ch == '"')
        {
            _reader.consume();
            token.value = "";

            while ((ch = _reader.consume()) != '"' && ch != EOF)
            {
                token.value += ch;
            }

            if (_keywords.find(token.value) == _keywords.end())
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
                _reader.consume();
                ch = _reader.peek();
            }

            if (_keywords.find(token.value) == _keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }

        _tokenBuffer.push_back(token);
    }

    return _tokenBuffer[position];
}

Token Lexer::peek(int idx)
{
    while (_tokenBuffer.size() <= position + idx)
    {
        skipWhiteSpacesAndComments();
        char ch = _reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = EOF_TOKEN;
            token.value = "";
            _tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';')
        {
            token.type = SEMICOLON;
            token.value = ";";
            _reader.consume();
        }
        else if (ch == '{')
        {
            token.type = LEFT_BRACE;
            token.value = "{";
            _reader.consume();
        }
        else if (ch == '}')
        {
            token.type = RIGHT_BRACE;
            token.value = "}";
            _reader.consume();
        }
        else if (ch == '"')
        {
            _reader.consume();
            token.value = "";

            while ((ch = _reader.consume()) != '"' && ch != EOF)
            {
                token.value += ch;
            }

            if (_keywords.find(token.value) == _keywords.end())
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
                _reader.consume();
                ch = _reader.peek();
            }

            if (_keywords.find(token.value) == _keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }

        _tokenBuffer.push_back(token);
    }

    if (position + idx > _tokenBuffer.size()) {
        throw std::runtime_error("token index is out of bounds");
    }

    return _tokenBuffer[position + idx];
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
