#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "./ServerConfig.cpp"

class FileParser
{
public:
    FileParser(std::string filePath)
    {
        this->_filePath = filePath;
        try
        {
            _loadFile();
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    std::vector<ServerConfig> parse()
    {
        // remove comments
        // remove white spaces
        // get directives
    }

private:
    void _loadFile()
    {
        std::ifstream configFile;
        std::string configContent;

        configFile.open(_filePath);
        if (!configFile.is_open())
        {
            throw std::runtime_error("couldn't load file");
        }
    }

    void _removeComments(std::string &fileContent)
    {
    }

    void _removeWhiteSpaces(std::string &fileContent)
    {
        
    }

    std::string _filePath;
    std::string _fileContent;
};