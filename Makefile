NAME = webserv

SRCS = 	client \
		config \
		html \
		http \
		location \
		main \
		request \
		response \
		server \
		service \
		utils \
		\

SRCS_DIR = src

OBJS = $(SRCS:%=$(OBJ_DIR)/%.o)

OBJ_DIR = obj

INC_DIR = include

FLAGS = -Wall -Werror -Wextra -g -std=c++98

CC = c++

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o : $(SRCS_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)/*

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re bonus
