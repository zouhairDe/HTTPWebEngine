CLANG = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror -Wshadow
DEBUG_FLAGS = -g -fsanitize=address
OPTIMIZATION_FLAGS = -O3
HEADER_FILES = Global.hpp ConfigParser.hpp Server.hpp File.hpp WebServer.hpp \
				Route.hpp Functions.hpp RequestProccessor.hpp GETResponse.hpp \

COLOR_HEADERS = IColor.hpp BlueColor.hpp GreenColor.hpp RedColor.hpp DefaultColor.hpp \
				BoldFont.hpp
FUNCTIONS_FILES = string_manipulation.cpp helper_function.cpp ServerFunctions.cpp
CLASSES_FILES = Global.cpp ConfigParser.cpp Server.cpp File.cpp WebServer.cpp \
				Route.cpp RequestProccessor.cpp GETResponse.cpp POSTResponse.cpp \

				
MAIN_FILE = main.cpp

SRC_DIR = src/
INCLUDE_DIR = include/
INTERFACE_DIR = $(INCLUDE_DIR)Interfaces/
COLORS_DIR = $(INTERFACE_DIR)Colors/
FUNCTIONS_DIR = $(SRC_DIR)Functions/
CLASSES_DIR = $(SRC_DIR)Classes/
DEBUG_OBJ_DIR = debug_obj/
OBJ_DIR = obj/

FLAGS += -I$(INCLUDE_DIR) -I$(COLORS_DIR)

INCLUDES = $(addprefix $(INCLUDE_DIR), $(HEADER_FILES)) $(addprefix $(COLORS_DIR), $(COLOR_HEADERS))
FILES = $(FUNCTIONS_FILES) $(CLASSES_FILES) $(MAIN_FILE)
OBJ_FILES = $(addprefix $(OBJ_DIR), $(FILES:.cpp=.opp))
DEBUG_OBJ_FILES = $(addprefix $(DEBUG_OBJ_DIR), $(FILES:.cpp=_debug.opp))
NAME_DEBUG = webserv_debug
NAME = webserv

# ==== RELEASE ==== #

all: $(OBJ_DIR) $(NAME)

opt: FLAGS += $(OPTIMIZATION_FLAGS)
opt: all

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ_FILES) $(INCLUDES)
	$(CLANG) $(FLAGS) $(OBJ_FILES) -o $(NAME)

$(OBJ_DIR)%.opp: $(FUNCTIONS_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)%.opp: $(CLASSES_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)%.opp: $(SRC_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# ==== DEBUG ==== #

debug: FLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_OBJ_DIR) $(NAME_DEBUG)


$(DEBUG_OBJ_DIR):
	@mkdir -p $(DEBUG_OBJ_DIR)

$(NAME_DEBUG): $(DEBUG_OBJ_FILES) $(INCLUDES)
	$(CLANG) $(FLAGS) $(DEBUG_OBJ_FILES) -o $(NAME_DEBUG)

$(DEBUG_OBJ_DIR)%_debug.opp: $(FUNCTIONS_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(DEBUG_OBJ_DIR)%_debug.opp: $(CLASSES_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(DEBUG_OBJ_DIR)%_debug.opp: $(SRC_DIR)%.cpp $(INCLUDES)
	$(CLANG) $(FLAGS) -I$(INCLUDE_DIR) -c $< -o $@


# ==== Docker ==== #

docker-build:
	docker build -t my-nginx-webserv docker/

docker-run:
	docker run -d -p 8080:80 --name webserv-container my-nginx-webserv
	docker exec webserv-container cp /var/www/html/index.nginx-debian.html /var/www/html/index.html
	docker exec -it webserv-container /bin/bash

docker-stop:
	docker stop webserv-container

docker-clean:
	docker rm -f webserv-container

docker-all: docker-build docker-run

# ==== CLEAN ==== #

clean:
	rm -f $(OBJ_FILES)
	rm -f $(DEBUG_OBJ_FILES)

fclean: clean
	rm -f $(NAME) $(NAME_DEBUG)

re: fclean all

.PHONY: all debug clean fclean re
