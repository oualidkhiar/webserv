NAME = webserv

CXX = c++

# Flags (-Wall -Werror -Wall) to be added.
CXXFLAGS = -std=c++98

# minimalistic automatic makefile (wildcard are allowed btw !)
SRCS = ${wildcard src/*.cpp HandleRequests/*.cpp} 

OBJS = $(SRCS:.cpp=.o)

%.o : %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

all: ${NAME}

$(NAME): $(OBJS)
	${CXX} ${CXXFLAGS} $(OBJS) -o ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re