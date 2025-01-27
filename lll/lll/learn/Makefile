SERVER = reciveHHHH.a7a
CLIENT = sendHHHH.a7a

FLAGS = -std=c++98

SEVER_FILE = main.cpp 
CLIENT_FILE = main2.cpp

SOBJ = $(SEVER_FILE:.cpp=.o)
COBJ = $(CLIENT_FILE:.cpp=.o)

all: $(SERVER) $(CLIENT) clean
	./$(SERVER)

$(SERVER): $(SOBJ)
	@c++ $(FLAGS) -o $(SERVER) $(SOBJ)

$(CLIENT): $(COBJ)
	@c++ $(FLAGS) -o $(CLIENT) $(COBJ)

%.o: %.cpp
	@c++ $(FLAGS) -c $< -o $@

clean:
	rm -f $(SOBJ) $(COBJ)

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re