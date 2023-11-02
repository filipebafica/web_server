cmake_minimum_required(VERSION 3.0)
project(your_project_name)

# Define the source files
set(SOURCE_FILES
        src/CGI/CGI.cpp
        src/CGI/CGIEnvironment.cpp
        src/CGI/CGIResponse.cpp
        src/main.cpp
        src/Monitor.cpp
        src/Request/HttpRequestHandler.cpp
        src/Response/HttpResponseHandler.cpp
        src/Setup/InitialParametersHandler.cpp
        src/Setup/Parser/CharacterReader.cpp
        src/Setup/Parser/Lexer.cpp
        src/Setup/Parser/Parser.cpp
        src/Setup/Parser/ServerConfig.cpp
        src/Setup/Parser/ServerLocation.cpp
        src/Signals.cpp
        src/Webserver.cpp
)

# Include directories
include_directories(src)

# Add executable
add_executable(your_program_name ${SOURCE_FILES})
