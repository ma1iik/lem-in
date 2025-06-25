CC =gcc
CFLAGS = -Wall -Werror -Wextra -g
NAME = lem-in
SRCS = main.c utils.c
OBJS = $(SRCS:.c=o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean:
	rm -rf $(OBJS) $(NAME)