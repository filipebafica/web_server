#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::string str = "This is a sample string";
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, ' ')) {
        std::cout << token << std::endl;
    }

    return 0;
}
