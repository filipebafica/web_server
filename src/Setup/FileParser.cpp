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
        _initSupportedDirectives();
        _filePath = filePath;

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
        _removeComments();
        // _removeWhiteSpaces();
        _parseDirectives();
    }

private:
    void _initSupportedDirectives()
    {
        _supportedDirectives = {
            "listen",
            "server_name",
            "error_page",
            "client_max_body_size",
            "autoindex"};
    }

    void _loadFile()
    {
        std::ifstream configFile;
        std::stringstream streamContent;

        configFile.open(_filePath);
        if (!configFile.is_open())
        {
            throw std::runtime_error("couldn't load file");
        }
        else
        {
            streamContent << configFile.rdbuf();
            _fileContent = streamContent.str();
        }
        configFile.close();
    }

    std::string _removeSingleLineComments(const std::string &line)
    {
        std::string result;
        bool inQuotes = false;

        for (size_t i = 0; i < line.length(); ++i)
        {
            if (line[i] == '"' && (i == 0 || line[i - 1] != '\\'))
            {
                inQuotes = !inQuotes;
            }
            if (!inQuotes && line[i] == '#')
            {
                break;
            }
            result += line[i];
        }

        return result;
    }

    void _removeComments()
    {
        std::stringstream resultStream;
        std::string line;
        std::istringstream inputStream(_fileContent);

        while (std::getline(inputStream, line))
        {
            resultStream << _removeSingleLineComments(line) << '\n';
            _fileContent = resultStream.str();
        }
    }

    size_t _findStartServerBlock(size_t serverPos)
    {
        for (size_t i = serverPos; i < _fileContent.size(); i++)
        {
            if (!std::isspace(_fileContent[i]) && _fileContent[i] != '{')
            {
                throw std::runtime_error("invalid config file");
            }

            if (_fileContent[i] == '{')
            {
                return i;
            }
        }
        throw std::runtime_error("server block not closed");
    }

    size_t _findEndServerBlock(size_t start)
    {
        for (size_t i = start; i < _fileContent.size(); i++)
        {
            
        }
    }

    void _parseServerBlock(size_t serverPos)
    {
        size_t start = _findStartServerBlock(serverPos);
        size_t end = _findEndServerBlock(start);
    }

    void _parseDirectives()
    {
        size_t serverPos = 0;
        size_t offset = 0;
        while (serverPos = _fileContent.find("server", offset) != std::string::npos)
        {
            _parseServerBlock(serverPos);
            offset += serverPos + 1;
        }
    }

    std::string _filePath;
    std::string _fileContent;
    std::vector<std::string> _supportedDirectives;
};