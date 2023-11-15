#include <Exceptions.hpp>


ServerResponseException::ServerResponseException(int status, const char* reasonPhrase)
    : _status(status), _reasonPhrase(reasonPhrase) {}

ServerResponseException::~ServerResponseException() throw() {}

int ServerResponseException::getStatus(void) const {
    return this->_status;
}

const char* ServerResponseException::getReasonPhrase(void) const {
    return this->_reasonPhrase;
}

PayloadTooLargeException::PayloadTooLargeException(int status, const char* reasonPhrase)
    : ServerResponseException(status, reasonPhrase) {}

MethodNotAllowedException::MethodNotAllowedException(int status, const char* reasonPhrase)
    : ServerResponseException(status, reasonPhrase) {}

RouteNotFoundException::RouteNotFoundException(int status, const char* reasonPhrase)
    : ServerResponseException(status, reasonPhrase) {}

BadRequestException::BadRequestException(int status, const char* reasonPhrase)
    : ServerResponseException(status, reasonPhrase) {}

RequestHeaderFieldsTooLargeException::RequestHeaderFieldsTooLargeException(int status, const char* reasonPhrase)
    : ServerResponseException(status, reasonPhrase) {}
