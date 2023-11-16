#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include <string>

class Resources
{
public:
    Resources(const std::string path, const bool isDirectory, const std::string mimeType);

    const std::string   path;
    const bool          isDirectory;
    const std::string   mimeType;
    std::string         redirection;
};

#endif // // RESOURCES_HPP_
