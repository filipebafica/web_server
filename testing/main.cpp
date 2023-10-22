#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
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

    void parse()
    {
        _removeComments();
        // _removeWhiteSpaces();
        _parseDirectives();
    }

    std::string getFileContent()
    {
        return _fileContent;
    }

private:
    void _initSupportedDirectives()
    {
        _supportedServerDirectives = {
            "listen",
            "server_name",
            "error_page",
            "client_max_body_size",
            "autoindex",
            "location",
            "default_server",
        };
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

    void _parseDirectives()
    {
        std::string directive;
        for (size_t i = 0; _fileContent[i]; i++)
        {
            if (!std::isspace(_fileContent[i]))
            {
                directive = _readDirective(&_fileContent[i]);
                if (!isValidDirective(directive))
                {
                    throw std::runtime_error("not a valid root directive");
                }
                i = _parseServerBlock(i);
            }
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
        throw std::runtime_error("server block not opened");
    }

    size_t _findEndServerBlock(size_t start)
    {
        size_t bracketsCount = 0;

        for (size_t i = start + 1; _fileContent[i]; i++)
        {
            if (_fileContent[i] == '{')
            {
                bracketsCount++;
            }

            if (_fileContent[i] == '}')
            {
                if (!bracketsCount)
                    return i;
                bracketsCount--;
            }
        }
        throw std::runtime_error("server block not closed");
    }

    ServerConfig makeServerConfig(std::string serverBlock)
    {
        ServerConfig serverConfig;
        std::map<std::string, std::vector<std::string>> data;

        std::istringstream blockStream(serverBlock);
        std::string currentLine;
        while (std::getline(blockStream, currentLine))
        {
            std::istringstream lineStream(currentLine);
            std::string key, token;

            /**
             * Essa função é ótima para pegar diretrizes que tem apenas uma linha
             * mas não funciona com diretrizes em bloco, então é necessário redefinir
             * a maneira de acordo com a diretriz em bloco chamada location.
             * 
             * Vou precisar usar classes ServerConfig e Location dentro da ServerConfig para conseguir
             * setar os atributos aqui a partir dos tokens.
             */
            if (lineStream >> key)
            {
                if (key == std::string("location"))
                {
                    int bracketsCount = 0;
                    while (lineStream >> token)
                    {
                        if (token == std::string("{"))
                        {
                            
                        }
                        data[key].push_back(token);
                    }
                }
                else
                {
                    while (lineStream >> token)
                    {
                        data[key].push_back(token);
                    }
                }
            }
        }

        return serverConfig;
    }

    size_t _parseServerBlock(size_t serverPos)
    {
        size_t start = _findStartServerBlock(serverPos + std::strlen("server"));
        size_t end = _findEndServerBlock(start);
        std::string serverBlock = _fileContent.substr(start, end + 1 - start);
        ServerConfig serverConfig = makeServerConfig(serverBlock);
        return (end);
    }

    int isValidDirective(std::string directive)
    {
        std::string rootDirective("server");
        return (directive == rootDirective);
    }

    std::string _readDirective(char *fileContent)
    {
        size_t i = 0;
        std::string directive;
        while (!std::isspace(fileContent[i]) && fileContent[i] != '\0')
        {
            directive += fileContent[i];
            i++;
        }
        return (directive);
    }

    std::string _filePath;
    std::string _fileContent;
    std::vector<std::string> _supportedServerDirectives;
};

int main()
{
    std::string filePath("testing.conf");

    FileParser parser(filePath);
    // std::cout << parser.getFileContent() << std::endl
    //           << std::endl;

    parser.parse();

    return (0);
}
