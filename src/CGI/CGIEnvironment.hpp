#include <vector>
#include <string>
#include <iostream>

using std::string;
using std::vector;

class CGIEnvironment {
 public:
    CGIEnvironment();
    ~CGIEnvironment();

    void          setVariable(const string& key, const string& value);
    char *const * data(void);

 private:
    vector<string>   _variables;
    char *const *    _saveEnvPtr;
};
