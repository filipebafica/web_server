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

    void parse()
    {
        _removeComments();
        // remove white spaces
        // get directives
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

    void _removeWhiteSpaces(std::string &fileContent)
    {
    }

    std::string _filePath;
    std::string _fileContent;
};

int main()
{
    std::string filePath("testing.conf");

    FileParser parser(filePath);
    parser.parse();
    std::cout << parser.getFileContent() << std::endl;

    return (0);
}
