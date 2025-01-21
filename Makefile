CPPC = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror
NAME = webserv
HEADERS_DIR = ./includes/
HEADER_FILES = Global.hpp Parser.hpp Server.hpp File.hpp WebServer.hpp ServerRoutes.hpp
HEADERS = $(addprefix $(HEADERS_DIR), $(HEADER_FILES))

SRC_DIR = ./src/
SRC_FILES = main.cpp
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_DIR = ./obj/
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CPPC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CPPC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re