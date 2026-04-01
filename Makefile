CC =gcc
CFLAGS = -Wall -Werror -Wextra -Wno-unused-variable -fsanitize=address -g -std=c99
NAME = lem-in
VISUALIZER_DIR = visualizer

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = includes
LIBFT_DIR = Libft
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

SRCS =	main.c \
		parsing.c \
		utils.c \
		pathfinding.c \
		path_scoring.c \
		ant_movement.c \

SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

all : $(NAME) visu-hex

$(NAME) : $(LIBFT) $(OBJS)
	$(CC) $(OBJS) $(LIBFT_FLAGS) -fsanitize=address -o $(NAME)

visu-hex:
	@$(MAKE) -s -C $(VISUALIZER_DIR) all
	@cp $(VISUALIZER_DIR)/visu-hex .

$(LIBFT):
	@$(MAKE) -s -C $(LIBFT_DIR)
	@$(MAKE) -s -C $(LIBFT_DIR) bonus

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@$(MAKE) -s -C $(LIBFT_DIR) clean
	@$(MAKE) -s -C $(VISUALIZER_DIR) clean
	@rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) -s -C $(LIBFT_DIR) fclean
	@$(MAKE) -s -C $(VISUALIZER_DIR) fclean
	@rm -f $(NAME) visu-hex

re: fclean all

.PHONY: all clean fclean re