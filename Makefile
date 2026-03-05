 NAME = webserv

CXX = c++

# Flags (-Wall -Werror -Wall) to be added.
CXXFLAGS = -std=c++98 -Iincludes

SRCDIR = src/
CONFIG_FILES = config.cpp parser.cpp tokenizer.cpp
HELPERS_FILES = Body.cpp FtFile.cpp MimeTypes.cpp StringManip.cpp utils.cpp
HTTP_FILES = ErrorResponse.cpp Executor.cpp handleCgi.cpp HttpRequest.cpp HttpResponse.cpp PostParser.cpp RequestParser.cpp TransactionManager.cpp
NETWORK_FILES = client.cpp listening_socket.cpp server_manager.cpp sockets_manager.cpp
SRC_FILES = main.cpp $(addprefix config/, $(CONFIG_FILES)) $(addprefix helpers/, $(HELPERS_FILES)) $(addprefix http/, $(HTTP_FILES)) $(addprefix network/, $(NETWORK_FILES))

SRCS = $(addprefix $(SRCDIR), $(SRC_FILES))
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

<<<<<<< HEAD
.PHONY: all clean fclean re
=======
.PHONY: all clean fclean re
>>>>>>> cookies-implementation
