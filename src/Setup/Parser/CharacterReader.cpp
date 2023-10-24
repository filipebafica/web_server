#include "CharacterReader.hpp"

/**
 * @todo It should be implemented tests
 * @todo Write all functionsd implementation detail
 */

CharacterReader::CharacterReader(const char *filePath) : _position(0)
{
    _inputFile.open(filePath);
    if (!_inputFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        throw std::runtime_error("Error: Could not open file");
    }

    std::stringstream bufferStream;
    bufferStream << _inputFile.rdbuf();
    _buffer = bufferStream.str();
}

char CharacterReader::peek()
{
    if (_position < _buffer.size())
    {
        return _buffer[_position];
    }
    else if (_inputFile.is_open())
    {
        return _inputFile.peek();
    }
    else
    {
        return EOF;
    }
}

char CharacterReader::peek(int idx)
{
    if (_position + idx < _buffer.size())
    {
        return _buffer[_position + idx];
    }
    else if (_inputFile.is_open())
    {
        _inputFile.seekg(_position + idx);
        return _inputFile.peek();
    }
    else
    {
        return EOF;
    }
}

char CharacterReader::consume()
{
    char currentChar = peek();
    if (currentChar != EOF)
    {
        _position++;
    }
    else if (_inputFile.is_open() && !_inputFile.eof())
    {
        _inputFile.get(currentChar);
    }
    return currentChar;
}

char CharacterReader::consume(int idx)
{
    char currentChar = EOF;
    for (int i = 0; i < idx; i++)
    {
        char nextChar = consume();
        if (nextChar == EOF)
        {
            break;
        }
        currentChar = nextChar;
    }
    return currentChar;
}

bool CharacterReader::isEOF()
{
    return peek() == EOF;
}

CharacterReader::~CharacterReader()
{
    if (_inputFile.is_open())
    {
        _inputFile.close();
    }
}
