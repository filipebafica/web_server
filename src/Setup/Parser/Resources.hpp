#include <string>

class Resources {
 public:
    Resources(
            const std::string& path,
            const bool isDirectory
    ) : path(path), isDirectory(isDirectory)
    {
    }
    const std::string path;
    const bool isDirectory;
};
