NAME = webserv
INCLUDES =  -I./src 				\
			-I./src/Interfaces 		\
			-I./src/CGI 			\
			-I./src/Setup/Parser 	\
			-I./src/Request 		\
			-I./src/Response 		\

SOURCE = 	main.cpp \
			Monitor.cpp \
			Webserver.cpp \
			HttpRequestHandler.cpp \
			CharacterReader.cpp \
			Lexer.cpp \
			Parser.cpp \
			ServerLocation.cpp \
			ServerConfig.cpp \
			CGI.cpp \
			CGIEnvironment.cpp \
			CGIResponse.cpp \
			CGIRequest.cpp \
			Signals.cpp \

OBJECTS_DIR = ./objects/
OBJECTS = $(SOURCE:%.cpp=$(OBJECTS_DIR)%.o)
CC = c++
FLAGS = -g -Wall -Wextra -Werror -std=c++98
VPATH = ./src/ ./src/CGI ./src/Request ./src/Response ./src/Setup ./src/Setup/Parser

PHP_CGI_PATH := $(shell which php-cgi)
FLAGS += -D CGI_PATH=\"$(PHP_CGI_PATH)\"

all: $(NAME)

$(NAME): $(OBJECTS_DIR) $(OBJECTS)
	@$(CC) $(FLAGS) $(OBJECTS) $(INCLUDES) -o $@

$(OBJECTS_DIR)%.o: %.cpp
	@$(CC) -c $(FLAGS) $< $(INCLUDES) -o $@

$(OBJECTS_DIR):
	@mkdir -p $(OBJECTS_DIR)

clean:
	@rm -rf $(OBJECTS_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all