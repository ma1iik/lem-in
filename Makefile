CC =gcc
CFLAGS = -Wall -Werror -Wextra -Wno-unused-variable -g -std=c99
NAME = lem-in

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = includes
LIBFT_DIR = Libft
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

SRCS =	main.c \
		parsing.c \
		utils.c \

SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m

all : $(NAME)

$(NAME) : $(LIBFT) $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(NC)"
	$(CC) $(OBJS) $(LIBFT_FLAGS) -o $(NAME)
	@echo "$(GREEN)✅ $(NAME) created successfully!$(NC)"

$(LIBFT):
	@echo "$(GREEN)Building Libft...$(NC)"
	@make -C $(LIBFT_DIR)
	@make -C $(LIBFT_DIR) bonus

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning objects...$(NC)"
	@make -C $(LIBFT_DIR) clean
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Cleaning $(NAME) and Libft...$(NC)"
	@make -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re