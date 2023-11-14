#include <HttpRequestHandler.hpp>

#include <sys/socket.h>

#define SPACE_DELIMITER " "
#define COLONS_DELIMITER ": "
#define END_OF_LINE_DELIMITER "\r\n"
#define HEADER_AND_BODY_DELIMITER "\r\n\r\n"
#define REQUEST_HEADER_START_INDEX 1
#define REQUEST_HEADER_PAIR_SIZE 2
#define EXPECTED_NUMBER_OF_TOKENS_IN_A_REQUEST_LINE 3
#define HEADER_AND_BODY_DELIMITER_LEN 4

void HttpRequestHandler::readRequest(int clientSocket, std::vector<char>& clientBuffer) {
    ssize_t bytesRead = read(clientSocket, this->defaultBuffer, sizeof(this->defaultBuffer) - 1);
    if (bytesRead < 0) {
        std::cerr << "Error reading request" << std::endl;
        throw std::runtime_error("Could not read from file descriptor");
    } else if (bytesRead == 0) {
        std::cerr << "Client disconnected" << std::endl;
        throw std::runtime_error("Could not read from file descriptor");
    } else {
        this->defaultBuffer[bytesRead] = 0;
        clientBuffer.insert(clientBuffer.end(), this->defaultBuffer, this->defaultBuffer + bytesRead);

        std::cout << "********** REQUEST **********" << std::endl;
        std::cout << "bytes read: " << bytesRead << std::endl;
        std::cout << this->defaultBuffer << std::endl;
    }
}

void HttpRequestHandler::parseRequest(std::vector<char>& clientBuffer, int defaultBufferHeaderSize, int clientMaxBodySize) {
    // Delimits the end of the request
    clientBuffer.push_back(0);
    this->requestLen = clientBuffer.size();

    // Validates Request size
    this->validateRequest(clientBuffer.data(), defaultBufferHeaderSize, clientMaxBodySize);

    // Parses Request-Line (first line)
    this->parseRequestLine(clientBuffer.data());

    // Parses Request-Header
    this->parseRequestHeader(clientBuffer.data());

    // Decodes URI, get 
    this->parseURI();

    // Parses the Request-Body
    this->parseRequestBody(clientBuffer.data());

}

const std::map<std::string, std::string>& HttpRequestHandler::getRequest() const {
    return this->request;
}

std::string HttpRequestHandler::getHeader(const std::string& key) const {
    if (this->request.find(key) == this->request.end()) {
        return std::string("");
    }
    return this->request.find(key)->second;
}

const std::vector<char>& HttpRequestHandler::getBody(void) const {
    return this->body;
}

void HttpRequestHandler::validateRequest(char *buffer, int defaultBufferHeaderSize, size_t clientMaxBodySize) {
    // Splits the request blocks (header and body)
    std::vector<std::string> blocks = this->tokenize(buffer, HEADER_AND_BODY_DELIMITER);

    /* Validates if request was completed read */
    if (blocks.empty()) {
        throw BadRequestException();
    }

    if ((int)blocks[0].size() > defaultBufferHeaderSize) {
        std::cerr << "Header too large" << std::endl;
        throw RequestHeaderFieldsTooLargeException();
    }

    // Validates if there is a body
    if (blocks.size() < 2) {
        return;
    }

    size_t bodyBegin = blocks[0].length() + HEADER_AND_BODY_DELIMITER_LEN;
    this->bodyLen = this->requestLen - bodyBegin;

    if (this->bodyLen > clientMaxBodySize) {
        std::cerr << "Body too large" << std::endl;
        throw PayloadTooLargeException();
    }
}

void HttpRequestHandler::parseRequestLine(char* buffer) {
    /* Parses the request line (e.g., "GET /path/to/resource HTTP/1.1") */

    // Splits the request into lines
    std::vector<std::string> lines = this->tokenize(
            buffer,
            END_OF_LINE_DELIMITER
    );

    if (lines.empty()) {
        return;
    }

    // Splits the first line (request-line) into tokens
    std::vector<std::string> tokens = this->tokenize((char*)lines[0].c_str(), SPACE_DELIMITER);

    if (tokens.empty() || tokens.size() != EXPECTED_NUMBER_OF_TOKENS_IN_A_REQUEST_LINE) {
        return;
    }

    this->request["Method"] = tokens[0];
    this->request["Route"] = tokens[1];
    this->request["Version"] = tokens[2];
}

void HttpRequestHandler::parseRequestHeader(char* buffer) {
    /* Parses the request headers (e.g., "
        Host: www.example.com
        Content-Type: application/octet-stream
        Accept-Language: en-US,en;q=0.5
        ") */


    // Splits the request blocks (header and body)
    std::vector<std::string> blocks = this->tokenize(buffer, HEADER_AND_BODY_DELIMITER);

    if (blocks.empty()) {
        return;
    }

    // Splits the request into lines
    std::vector<std::string> lines = this->tokenize(blocks[0].c_str(), END_OF_LINE_DELIMITER);

    if (lines.empty()) {
        return;
    }

    // Adds to the request vector the remaining headers (request-header)
    for (size_t i = REQUEST_HEADER_START_INDEX; i < lines.size(); ++i) {
        std::string line = lines[i];
        std::vector<std::string> tokens = this->tokenize((char*)line.c_str(), COLONS_DELIMITER);
        if (tokens.size() == REQUEST_HEADER_PAIR_SIZE) {
            this->request[tokens[0]] = tokens[1];
//            std::cout << this->request[tokens[0]] << ": " << tokens[1] << std::endl;
        }
    }
}

void HttpRequestHandler::parseRequestBody(char* buffer) {
    /* Parses and stores the message-body */

    this->body.clear();
    // Splits the request blocks (header and body)

    std::vector<std::string> tokens = this->tokenize(buffer, HEADER_AND_BODY_DELIMITER);

    // Validates if there is a body
    if (tokens.empty() || tokens.size() < 2) {
        return;
    }

    size_t bodyBegin = tokens[0].length() + HEADER_AND_BODY_DELIMITER_LEN;
    char *_body = buffer + bodyBegin;

    if (this->getHeader("Transfer-Encoding") == std::string("chunked")) {
        this->setChunkedRequest(_body);
        return;
    }

//    this->body = std::vector<char>(_body, buffer + this->requestLen + 1);
    this->body = std::vector<char>(_body, _body + this->bodyLen);
    std::cout << "********** BODY **********" << std::endl;
//    std::cout << this->body.data() << std::endl;
}

void HttpRequestHandler::setChunkedRequest(char* chunkedBody) {
    size_t pos = 0;
    char* endPtr;

    this->body.clear();
    while (pos < this->bodyLen) {
        // Finds the position of the CRLF (END_OF_LINE_DELIMITER)
        size_t crlfPos = this->_find(chunkedBody, pos, this->bodyLen, END_OF_LINE_DELIMITER);
        if (crlfPos == std::size_t(-1)) {
            // Invalid format
            break;
        }

        // Extract the chunk size // 16 is used to define the base of the chunk length
        long chunkSize = strtol(chunkedBody + pos, &endPtr, 16);

        if (chunkSize == 0) {
            // End of chunked data
            break;
        }

        // Move the position past the current CRLF
        pos = crlfPos + strlen(END_OF_LINE_DELIMITER);

        // Append the chunk data to the result
        this->body.insert(this->body.end(), endPtr, chunkedBody + pos + chunkSize);


        // Move the position past the chunk data and CRLF
        pos += chunkSize + strlen(END_OF_LINE_DELIMITER);
    }

    this->body.push_back(0);
    std::cout << "********** BODY **********" << std::endl;
//    std::cout << this->body.data() << std::endl;
}

std::vector<std::string> HttpRequestHandler::tokenize(const char* s, const char* delim) {
    std::vector<std::string> tokens;
    std::string text = s;
    std::string delimiter = delim;

    size_t pos = 0;
    while ((pos = text.find(delimiter)) != std::string::npos) {
        std::string token = text.substr(0, pos);
        tokens.push_back(token);
        text.erase(0, pos + delimiter.length());
    }

    if (!text.empty() && !tokens.empty()) {
        tokens.push_back(text);
    }

    return tokens;
}

void HttpRequestHandler::parseURI(void) {
    const std::string& requestURI = this->request["Route"];
    size_t queryStart = requestURI.find('?');

    const std::string& uriWithoutQuery  = requestURI.substr(0, queryStart);
    const std::string& decodedURI       = _decodeURI(uriWithoutQuery);

    this->request["DecodedURI"] = decodedURI;

    // std::size_t lastURIBar  = decodedURI.find_last_of('/');

    // this->request["Route"]  = decodedURI.substr(0, lastURIBar);
    // this->request["File"]   = decodedURI.substr(lastURIBar + 1);

    this->request["isPHP"]  = _checkCGIExtension(decodedURI);

    if (queryStart != std::string::npos) {
        this->request["Querystring"] = requestURI.substr(queryStart + 1);
    } else {
        this->request["Querystring"] = std::string();
    }
}

/**
 * The function decodes a URI-encoded string by replacing
 * percent-encoded characters with their corresponding ASCII characters and
 * replacing the plus sign with a space.
 * 
 * @param requestURI It's a string that represents a URI that
 * has been percent-encoded.
 * 
 * @return a string that represents the _decoded URI.
 */
std::string HttpRequestHandler::_decodeURI(const std::string& requestURI) {
    std::string decoded;
    decoded.reserve(requestURI.length());

    for (const char *s = requestURI.c_str(); *s; s++) {
        if (*s == '%' && *(s + 1) && *(s + 2)) {
            decoded += _decodePercentEncoded(s + 1);
            s += 2;
        } else if (*s == '+') {
            decoded += ' ';
        } else {
            decoded += *s;
        }
    }

    return decoded;
}

std::string HttpRequestHandler::_checkCGIExtension(const std::string& decodedURI) {
    size_t extensionPos = decodedURI.rfind(".php");

    if (extensionPos != std::string::npos &&
        extensionPos == (decodedURI.length() - 4)) {
        return "Yes";
    }
    return "No";
}

/**
 * Decodes a percent-encoded character represented by two hexadecimal
 * digits.
 * 
 * @param hexDigits The parameter is a pointer to a character array
 * that represents a pair of hexadecimal digits.
 * 
 * @return a character value.
 */
char HttpRequestHandler::_decodePercentEncoded(const char* hexDigits) {
    int hex = _hex2int(hexDigits[0]) * 16 + _hex2int(hexDigits[1]);
    return static_cast<char>(hex);
}

int HttpRequestHandler::_hex2int(char c) {
    return isdigit(c) ? (c - '0') : (tolower(c) - 'a' + 10);
}

std::size_t HttpRequestHandler::_find(const char *buffer, std::size_t pos, std::size_t bufferLen, const char *delim) {
    while (pos < bufferLen) {
        if (buffer[pos] == delim[0]) {
            std::size_t i = 0;

            while (pos + i < bufferLen && delim[i] != '\0' && buffer[pos + i] == delim[i]) {
                ++i;
            }
            if (delim[i] == '\0') {
                return pos;
            }
        }
        ++pos;
    }
    return std::size_t(-1);
}









