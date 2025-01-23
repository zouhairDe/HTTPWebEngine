CPPC = c++
CFLAGS = -std=c++98 #-Wall -Wextra -Werror
NAME = webserv

# Directories
HEADERS_DIR = includes/
INTERFACE_DIR = $(HEADERS_DIR)interfaces/
COLORS_DIR = $(INTERFACE_DIR)Colors/
FUNCTIONS_DIR = functions/
SRC_DIR = src/
OBJ_DIR = obj/

# Header files
HEADER_FILES = Global.hpp Parser.hpp Server.hpp File.hpp WebServer.hpp ServerRoutes.hpp functions.hpp cpp11.hpp
HEADERS = $(addprefix $(HEADERS_DIR), $(HEADER_FILES))

# Interface files (colors)
INTERFACE_FILES = hh.hpp
COLORS_FILES = IColor.hpp BleuColor.hpp GreenColor.hpp RedColor.hpp DefaultColor.hpp BoldFont.hpp
COLORS = $(addprefix $(COLORS_DIR), $(COLORS_FILES))

# Files
SRC_FILES = main.cpp Global.cpp
FUNCTIONS_FILES = string_manipulation.cpp helper_function.cpp

# Full paths
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
FUNCTIONS = $(addprefix $(FUNCTIONS_DIR), $(FUNCTIONS_FILES))

# Object files
SRC_OBJ = $(SRC_FILES:%.cpp=$(OBJ_DIR)%.o)
FUNCTIONS_OBJ = $(FUNCTIONS_FILES:%.cpp=$(OBJ_DIR)%.o)
OBJ = $(SRC_OBJ) $(FUNCTIONS_OBJ)

all: $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADERS)
	$(CPPC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)%.o: $(FUNCTIONS_DIR)%.cpp $(HEADERS)
	$(CPPC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CPPC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re