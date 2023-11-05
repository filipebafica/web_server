#ifndef CHARACTER_READER_HPP
#define CHARACTER_READER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class CharacterReader
{
public:
    CharacterReader(const char *filePath);
    char peek();
    char peek(int idx);
    char consume();
    char consume(int idx);
    bool isEOF();
    ~CharacterReader();

private:
    std::ifstream _inputFile;
    std::string _buffer;
    size_t _position;
};

#endif // CHARACTER_READER_HPP
