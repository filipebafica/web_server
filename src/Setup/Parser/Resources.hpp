#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include <string>

class Resources
{
public:
    Resources(const std::string path, const bool isDirectory);

    const std::string   path;
    const bool          isDirectory;
};

#endif // // RESOURCES_HPP_
