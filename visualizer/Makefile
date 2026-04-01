CC = gcc
CFLAGS = -Wall -Werror -Wextra -g

NAME = visu-hex

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = includes

INCLUDES = -I$(INC_DIR)

SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null || echo "-I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2")
SDL_LDFLAGS = $(shell sdl2-config --libs 2>/dev/null || echo "-L/opt/homebrew/lib -L/usr/local/lib -lSDL2")

SRCS = main.c \
       parser.c \
       render.c \
       animation.c

SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
NC = \033[0m

all: check_sdl $(NAME)

check_sdl:
	@if ! command -v sdl2-config >/dev/null 2>&1; then \
		echo "$(YELLOW)Warning: sdl2-config not found.$(NC)"; \
		echo "$(YELLOW)Install with: brew install sdl2$(NC)"; \
	fi

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(NC)"
	$(CC) $(OBJS) $(SDL_LDFLAGS) -lm -o $(NAME)
	@echo "$(GREEN)$(NAME) created!$(NC)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) $(SDL_CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

# Install SDL2 on macOS
install_sdl:
	@echo "$(GREEN)Installing SDL2...$(NC)"
	@brew install sdl2

.PHONY: all clean fclean re check_sdl install_sdl
