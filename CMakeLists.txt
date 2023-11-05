cmake_minimum_required(VERSION 3.0)
project(webserver)

# Defina o padrão C++ para C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adicione as flags de compilação desejadas
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

# Define a variável CGI_PATH
find_program(PHP_CGI_PATH php-cgi)
if (NOT PHP_CGI_PATH)
    message(FATAL_ERROR "php-cgi not found. Please install it.")
endif()

# Adicione a variável de compilação CGI_PATH
add_definitions(-DCGI_PATH="${PHP_CGI_PATH}")

# Define the source files
set(SOURCE_FILES
        src/CGI/CGI.cpp
        src/CGI/CGIEnvironment.cpp
        src/CGI/CGIResponse.cpp
        src/CGI/CGIRequest.cpp
        src/Exceptions/Exceptions.cpp
        src/main.cpp
        src/Monitor.cpp
        src/Request/HttpRequestHandler.cpp
        src/Response/HttpResponseHandler.cpp
        src/Parser/CharacterReader.cpp
        src/Parser/Lexer.cpp
        src/Parser/Parser.cpp
        src/Parser/Resources.cpp
        src/Parser/ServerConfig.cpp
        src/Parser/ServerLocation.cpp
        src/Signals.cpp
        src/Webserver.cpp
)

# Include directories
include_directories(
        src
        src/CGI
        src/Exceptions
        src/Interfaces
        src/Request
        src/Response
        src/Setup
        src/Parser
)

# Add executable
add_executable(webserver ${SOURCE_FILES})

target_compile_definitions(webserver PRIVATE CLI_ARGS="${CLI_ARGS}")
