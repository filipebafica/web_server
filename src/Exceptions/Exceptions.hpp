#ifndef SRC_EXCEPTIONS_EXCEPTIONS_HPP_
#define SRC_EXCEPTIONS_EXCEPTIONS_HPP_

#include <exception>
#include <string>

class ServerResponseException: public std::exception {
 public:
    ServerResponseException(int status, const char* reasonPhrase);
    ~ServerResponseException() throw();

    int getStatus(void) const;
    const char* getReasonPhrase(void) const;

 protected:
    int _status;
    const char* _reasonPhrase;
};

class PayloadTooLargeException : public ServerResponseException {
public:
    explicit PayloadTooLargeException(int status = 413, const char* reasonPhrase = "Request Entity Too Large");
};

class MethodNotAllowedException : public ServerResponseException {
public:
    explicit MethodNotAllowedException(int status = 405, const char* reasonPhrase = "Method Not Allowed");
};

class RouteNotFoundException : public ServerResponseException {
public:
    explicit RouteNotFoundException(int status = 404, const char* reasonPhrase = "Not Found");
};

class BadRequestException : public ServerResponseException {
public:
    explicit BadRequestException(int status = 400, const char* reasonPhrase = "Bad Request");
};

#endif /* SRC_EXCEPTIONS_EXCEPTIONS_HPP_ */
