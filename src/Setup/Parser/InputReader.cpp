#include "InputReader.hpp"

InputReader::InputReader(const std::string& filePath) : _inputFile(filePath), _position(0) {
    if(!inputFile.is_open()) {
        std::cerr << "Error: could not open file " << filePath << std::endl;
    }
}

InputReader::InputReader(const std::string& inputString) : _buffer(inputString), _position(0) {}

char InputReader::peek() {
    if (this->_position < buffer.size()) {
        return buffer[this->_position];
    } else if (inputFile.is_open()) {
        return inputFile.peek();
    } else {
        return EOF;
    }
}