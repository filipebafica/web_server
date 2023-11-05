#ifndef SRC_EXCEPTIONS_EXCEPTIONS_HPP_
#define SRC_EXCEPTIONS_EXCEPTIONS_HPP_

#include <exception>
#include <string>

class ServerResponseException: public std::exception {
 public:
    explicit ServerResponseException(int status);

    int getStatus(void) const;

 protected:
    int _status;
};

class PayloadTooLargeException: public ServerResponseException {
 public:
    explicit PayloadTooLargeException(int status = 413);
};

class MethodNotAllowedException: public ServerResponseException {
 public:
    explicit MethodNotAllowedException(int status = 405);
};

class RouteNotFoundException: public ServerResponseException {
 public:
    explicit RouteNotFoundException(int status = 404);
};

#endif /* SRC_EXCEPTIONS_EXCEPTIONS_HPP_ */
