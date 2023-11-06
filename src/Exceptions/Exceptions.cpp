#include <Exceptions.hpp>

ServerResponseException::ServerResponseException(int status) 
    : _status(status) {}

int ServerResponseException::getStatus(void) const {
    return this->_status;
}

PayloadTooLargeException::PayloadTooLargeException(int status)
    : ServerResponseException(status) {}

MethodNotAllowedException::MethodNotAllowedException(int status)
    : ServerResponseException(status) {}

RouteNotFoundException::RouteNotFoundException(int status)
    : ServerResponseException(status) {}

BadRequestException::BadRequestException(int status)
    : ServerResponseException(status){}
