CPPC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
DEBUG = -g -fsanitize=address
NAME = webserv

# Directories
SRC_DIR = src/
OBJ_DIR = obj/
HEADERS_DIR = includes/
INTERFACE_DIR = $(HEADERS_DIR)Interfaces/
COLORS_DIR = $(INTERFACE_DIR)Colors/
FUNCTIONS_DIR = $(SRC_DIR)Functions/
CLASSES_DIR = $(SRC_DIR)Classes/

# Header files
HEADER_FILES = Global.hpp ConfigParser.hpp Server.hpp File.hpp WebServer.hpp Route.hpp functions.hpp
HEADERS = $(addprefix $(HEADERS_DIR), $(HEADER_FILES))

# Interface files (colors)
COLORS_FILES = IColor.hpp BlueColor.hpp GreenColor.hpp RedColor.hpp DefaultColor.hpp BoldFont.hpp
COLORS = $(addprefix $(COLORS_DIR), $(COLORS_FILES))

# Files
SRC_FILES = main.cpp
FUNCTIONS_FILES = string_manipulation.cpp helper_function.cpp
CLASSES_FILES = Global.cpp ConfigParser.cpp Server.cpp File.cpp WebServer.cpp Route.cpp

# Full paths
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
FUNCTIONS_SRC = $(addprefix $(FUNCTIONS_DIR), $(FUNCTIONS_FILES))
CLASSES_SRC = $(addprefix $(CLASSES_DIR), $(CLASSES_FILES))

# Object files
SRC_OBJ = $(SRC_FILES:%.cpp=$(OBJ_DIR)%.opp)
FUNCTIONS_OBJ = $(FUNCTIONS_FILES:%.cpp=$(OBJ_DIR)%.opp)
CLASSES_OBJ = $(CLASSES_FILES:%.cpp=$(OBJ_DIR)%.opp)
OBJ = $(SRC_OBJ) $(FUNCTIONS_OBJ) $(CLASSES_OBJ)

all: CFLAGS += $(DEBUG)
all: $(NAME)

d: CFLAGS += $(DEBUG)
d: re

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.opp: $(SRC_DIR)%.cpp $(HEADERS) $(COLORS)
	$(CPPC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(OBJ_DIR)%.opp: $(CLASSES_DIR)%.cpp $(HEADERS) $(COLORS)
	$(CPPC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(OBJ_DIR)%.opp: $(FUNCTIONS_DIR)%.cpp $(HEADERS) $(COLORS)
	$(CPPC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CPPC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all d clean fclean re
