NAME = Webserv

CC = c++

CFLAGS = -std=c++98

REQDIR = HandleRequests/

SRCS =  src/main.cpp\
	src/tokenizer.cpp\
	src/parser.cpp\
	src/config.cpp\
	src/server_manager.cpp\
	src/sockets_manager.cpp\
	src/listening_socket.cpp\
	src/client.cpp\
	${REQDIR}Body.cpp ${REQDIR}ContainersManip.cpp ${REQDIR}Executor.cpp ${REQDIR}FilesManip.cpp \
                ${REQDIR}FtFile.cpp ${REQDIR}HttpRequest.cpp ${REQDIR}HttpResponse.cpp  \
                ${REQDIR}MimeTypes.cpp ${REQDIR}RequestParser.cpp ${REQDIR}StringManip.cpp \
                ${REQDIR}TransactionManager.cpp ${REQDIR}utils.cpp ${REQDIR}handleCgi.cpp



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