#include <CGIEnvironment.hpp>

CGIEnvironment::CGIEnvironment(void) : _saveEnvPtr(NULL) {}

CGIEnvironment::~CGIEnvironment() {
    delete [] this->_saveEnvPtr;
}

/**
 * Adds a key-value pair to a list of variables in the CGI environment.
 */
void CGIEnvironment::setVariable(const std::string& key,
                                 const std::string& value) {
    this->_variables.push_back(key + "=" + value);
}

/**
 * Returns a pointer to an array of strings
 * with all stored CGI environment variables.
 */
char* const* CGIEnvironment::data(void) {
    try {
        char* const* envp = new char *[this->_variables.size() + 1];
        size_t i;

        this->_saveEnvPtr = envp;

        for (i = 0; i < this->_variables.size(); ++i) {
            const_cast<const char **>(envp)[i] = this->_variables[i].c_str();
        }
        const_cast<const char **>(envp)[i] = NULL;

        return this->_saveEnvPtr;
    } catch (const std::bad_alloc& e) {
        return NULL;
    }
}
