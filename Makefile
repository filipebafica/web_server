NAME = webserv
INCLUDES = ./src/Interfaces
SOURCE = main.cpp Monitor.cpp Webserver.cpp InitialParametersHandler.cpp \
		 HttpRequestHandler.cpp HttpResponseHandler.cpp
OBJECTS_DIR = ./objects/
OBJECTS = $(SOURCE:%.cpp=$(OBJECTS_DIR)%.o)
CC = c++
#FLAGS = -g -Wall -Wextra -Werror -std=c++98
VPATH = ./src/ ./src/Setup ./src/Request ./src/Response


all: $(NAME)

$(NAME): $(OBJECTS_DIR) $(OBJECTS)
	@$(CC) $(FLAGS) $(OBJECTS) -o $@

$(OBJECTS_DIR)%.o: %.cpp
	@$(CC) -c $(FLAGS) $< -I$(INCLUDES) -o $@

$(OBJECTS_DIR):
	@mkdir -p $(OBJECTS_DIR)

clean:
	@rm -rf $(OBJECTS_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all