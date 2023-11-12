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

    void dumpEnv(void) {
      for (size_t i = 0; i < this->_variables.size(); i++) {
         std::cout << this->_variables[i] << std::endl;
      }
      std::cout << std::endl;
    }

 private:
    std::vector<std::string>  _variables;
    char *const *             _saveEnvPtr;
};

#endif /* SRC_CGI_CGIENVIRONMENT_HPP_ */
