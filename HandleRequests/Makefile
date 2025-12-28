# Compiler and Flags
CXX          := c++
CXXFLAGS     := -Wall -Wextra -Werror -std=c++98 # Change to -std=c++11 or higher if preferred
RM           := rm -rf

# Executable Name
NAME         := webserv

# Source Files
SRCS         := main.cpp \
                HttpRequest.cpp \
                HttpResponse.cpp \
                RequestParser.cpp \
                StringManip.cpp    \
				Body.cpp \
				FtFile.cpp

# Object Files (Stored in a separate directory to keep root clean)
OBJS         := $(SRCS:.cpp=.o)

# Default Rule
all: $(NAME)

# Link the Executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Compile Source Files into Object Files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Object Files
clean:
	$(RM) $(OBJS)

# Clean Everything
fclean: clean
	$(RM) $(NAME)

# Rebuild
re: fclean all

.PHONY: all clean fclean re
