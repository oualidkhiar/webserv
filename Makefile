NAME = Webserv

CC = c++

CFLAGS = -std=c++98

SRCS =  src/main.cpp\
	src/tokenizer.cpp\
	src/parser.cpp\
	src/config.cpp\
	src/server_manager.cpp\
	src/sockets_manager.cpp\
	src/listening_socket.cpp\
	src/client.cpp

OBJS = $(SRCS:.cpp=.o)

%.o : %.cpp
	${CC} ${CFLAGS} -c $< -o $@

all: ${NAME}

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all