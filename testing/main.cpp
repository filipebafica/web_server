#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

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

    std::vector<std::vector<std::string>> parse()
    {
        _removeComments();
        _removeWhiteSpaces();
        return _parseDirectives();
    }

    std::string getFileContent() const
    {
        return _fileContent;
    }

private:
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

    void _removeWhiteSpaces()
    {
        std::string result;
        for (std::size_t i = 0; i < _fileContent.size(); ++i) {
            if (!std::isspace(_fileContent[i])) {
                result += _fileContent[i];
            }
        }
        _fileContent = result;
    }

    std::vector<std::string> parseBlockDirectives(const std::string& serverBlock)
    {
        std::vector<std::string> blockDirectives;
        std::vector<std::string> lines = splitString(serverBlock, '\n');
        bool insideBlock = false;

        for (const std::string& line : lines) {
            if (line.find('{') != std::string::npos) {
                insideBlock = true;
            }
            if (insideBlock) {
                blockDirectives.push_back(line);
            }
            if (insideBlock && line.find('}') != std::string::npos) {
                insideBlock = false;
            }
        }

        return blockDirectives;
    }

    std::vector<std::string> splitString(const std::string& input, char delimiter) 
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(input);

        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }


    std::vector<std::vector<std::string>> _parseDirectives()
    {
        std::vector<std::vector<std::string>> directives;
        std::vector<std::string> serverBlocks = splitString(_fileContent, '}');

        for (const std::string& serverBlock : serverBlocks) {
            if (!serverBlock.empty()) {
                std::vector<std::string> blockDirectives = parseBlockDirectives(serverBlock + '}');
                directives.push_back(blockDirectives);
            }
        }

        return directives;
    }

    std::string _filePath;
    std::string _fileContent;
};

int main()
{
    std::string filePath("testing.conf");

    FileParser parser(filePath);
    std::vector<std::vector<std::string>> parsedDirectives = parser.parse();
    std::cout << parser.getFileContent() << std::endl << std::endl;

    for (const auto& serverDirectives : parsedDirectives) {
        for (const auto& blockDirective : serverDirectives) {
            std::cout << "Block Directive:\n" << blockDirective << "\n";
            std::cout << "--------------------\n";
        }
    }

    return (0);
}
