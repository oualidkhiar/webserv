# Compiler and Flags
CXX          := c++
CXXFLAGS     := -Wall -Wextra -Werror -std=c++98
RM           := rm -rf

# Executable Name
NAME         := webserv

# Source Files
SRCS         := Body.cpp ContainersManip.cpp Executor.cpp FilesManip.cpp \
                FtFile.cpp HttpRequest.cpp HttpResponse.cpp main.cpp \
                MimeTypes.cpp RequestParser.cpp StringManip.cpp \
                TransactionManager.cpp 

# Object Directory
OBJDIR       := obj

# Object Files (Now actually stored in obj/)
OBJS         := $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

# Default Rule
all: $(NAME)

# Link the Executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Compile Source Files into Object Files (with directory creation)
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Object Files and Directory
clean:
	$(RM) $(OBJDIR)

# Clean Everything
fclean: clean
	$(RM) $(NAME)

# Rebuild
re: fclean all

# Phony targets to prevent conflicts with files of the same name
.PHONY: all clean fclean re
