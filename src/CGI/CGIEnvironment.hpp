#ifndef SRC_CGI_CGIENVIRONMENT_HPP_
#define SRC_CGI_CGIENVIRONMENT_HPP_

#include <vector>
#include <string>
#include <iostream>

class CGIEnvironment {
 public:
    CGIEnvironment();
    ~CGIEnvironment();

    void          setVariable(const std::string& key, const std::string& value);
    char *const * data(void);

 private:
    std::vector<std::string>  _variables;
    char *const *             _saveEnvPtr;
};

#endif /* SRC_CGI_CGIENVIRONMENT_HPP_ */
