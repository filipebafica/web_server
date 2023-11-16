#include "Resources.hpp"

Resources::Resources(
        const std::string path,
        const bool isDirectory,
        const std::string mimeType,
        const std::string redirection
): path(path), isDirectory(isDirectory), mimeType(mimeType), redirection(redirection) {}
