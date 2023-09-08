//
// Created by filipebafica on 08/09/23.
//

class IParser {
public:
    virtual void parse(char* s) = 0;
    virtual const std::map<std::string, std::string>& getHeaders() const = 0;
    ~IParser() {};
};
