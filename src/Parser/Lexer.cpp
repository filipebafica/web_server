#include <Lexer.hpp>

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
        this->_keywords.insert(allowedKeywords[i]);
    }
}

void Lexer::skipWhiteSpacesAndComments()
{
    char ch = this->_reader.peek();

    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '#')
    {
        if (ch == '#')
        {
            while (ch != '\n' && ch != EOF)
            {
                ch = this->_reader.consume();
            }
        }
        else
        {
            ch = this->_reader.consume();
        }
        ch = this->_reader.peek();
    }
}

Token Lexer::peek()
{
    this->skipWhiteSpacesAndComments();

    while (this->_tokenBuffer.size() <= position)
    {
        char ch = this->_reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = EOF_TOKEN;
            token.value = "";
            this->_tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';')
        {
            token.type = SEMICOLON;
            token.value = ";";
            this->_reader.consume();
        }
        else if (ch == '{')
        {
            token.type = LEFT_BRACE;
            token.value = "{";
            this->_reader.consume();
        }
        else if (ch == '}')
        {
            token.type = RIGHT_BRACE;
            token.value = "}";
            this->_reader.consume();
        }
        else if (ch == '"')
        {
            this->_reader.consume();
            token.value = "";

            while ((ch = this->_reader.consume()) != '"' && ch != EOF)
            {
                token.value += ch;
            }

            if (this->_keywords.find(token.value) == this->_keywords.end())
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
                this->_reader.consume();
                ch = this->_reader.peek();
            }

            if (this->_keywords.find(token.value) == this->_keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }

        this->_tokenBuffer.push_back(token);
    }

    return this->_tokenBuffer[position];
}

Token Lexer::peek(int idx)
{
    while (this->_tokenBuffer.size() <= position + idx)
    {
        this->skipWhiteSpacesAndComments();
        char ch = this->_reader.peek();
        Token token;

        if (ch == EOF)
        {
            token.type = EOF_TOKEN;
            token.value = "";
            this->_tokenBuffer.push_back(token);
            break;
        }

        if (ch == ';')
        {
            token.type = SEMICOLON;
            token.value = ";";
            this->_reader.consume();
        }
        else if (ch == '{')
        {
            token.type = LEFT_BRACE;
            token.value = "{";
            this->_reader.consume();
        }
        else if (ch == '}')
        {
            token.type = RIGHT_BRACE;
            token.value = "}";
            this->_reader.consume();
        }
        else if (ch == '"')
        {
            this->_reader.consume();
            token.value = "";

            while ((ch = this->_reader.consume()) != '"' && ch != EOF)
            {
                token.value += ch;
            }

            if (this->_keywords.find(token.value) == this->_keywords.end())
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
                this->_reader.consume();
                ch = this->_reader.peek();
            }

            if (this->_keywords.find(token.value) == this->_keywords.end())
            {
                token.type = IDENTIFIER;
            }
            else
            {
                token.type = KEYWORD;
            }
        }

        this->_tokenBuffer.push_back(token);
    }

    if (position + idx > this->_tokenBuffer.size()) {
        throw std::runtime_error("token index is out of bounds");
    }

    return this->_tokenBuffer[position + idx];
}

Token Lexer::consume()
{
    Token token = this->peek();
    position++;
    return token;
}

Token Lexer::consume(int idx)
{
    Token token = this->peek(idx);
    position += idx;
    return token;
}
