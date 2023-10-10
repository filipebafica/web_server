#ifndef INPUT_READER_HPP
#define INPUT_READER_HPP

#include <iostream>
#include <fstream>
#include <string>

class InputReader
{
    private:
        std::ifstream _inputFile;
        std::string _buffer;
        size_t _position;

    public:
        InputReader(const std::string& filePath);
        InputReader(const std::string& inputString);
        char peek();
        char peek(int k);
        char consume();
        char consume(int k);
        bool isEOF();
        ~CharacterReader();
}

#endif // INPUT_READER_HPP