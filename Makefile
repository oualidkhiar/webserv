NAME = webserv

CC = c++

CFLAGS = -std=c++98

REQDIR = HandleRequests/

# minimalistic makefile (wildcard are allowed btw !)
SRCS = ${wildcard src/*.cpp HandleRequests/*.cpp} 

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