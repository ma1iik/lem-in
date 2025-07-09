# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ma1iik <ma1iik@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/12/18 19:37:01 by misrailo          #+#    #+#              #
#    Updated: 2025/07/09 02:34:56 by ma1iik           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC=gcc

NAME=libft.a

AR= ar -rc

FLAG=-Wall -Werror -Wextra 

SRC=	ft_strlcpy.c \
		ft_strlen.c \
		ft_strlcat.c \
		ft_isalnum.c \
		ft_memcmp.c \
		ft_isalpha.c \
		ft_isascii.c \
		ft_isdigit.c \
		ft_isprint.c \
		ft_tolower.c \
		ft_toupper.c \
		ft_atoi.c \
		ft_strncmp.c \
		ft_putchar_fd.c \
		ft_putendl_fd.c \
		ft_putnbr_fd.c \
		ft_putstr_fd.c \
		ft_striteri.c \
		ft_bzero.c \
		ft_calloc.c \
		ft_calloc.c \
		ft_memchr.c \
		ft_memcpy.c \
		ft_memmove.c \
		ft_memset.c \
		ft_itoa.c \
		ft_split.c \
		ft_strchr.c \
	 	ft_strdup.c \
		ft_strjoin.c \
		ft_strmapi.c \
		ft_strncpy.c \
		ft_strnstr.c \
		ft_strrchr.c \
		ft_strtrim.c \
	 	ft_substr.c \
		ft_memccpy.c \
		get_next_line.c \
		ft_strcmp.c \
		ft_printf.c \

SRC_B=	ft_lstnew.c \
		ft_lstadd_front.c \
		ft_lstsize.c \
		ft_lstlast.c \
		ft_lstclear.c \
		ft_lstadd_back.c \
		ft_lstdelone.c \
		ft_lstmap.c\
		ft_lstiter.c \

OBJ = $(SRC:.c=.o)

OBJ_B = $(SRC_B:.c=.o)

all : $(NAME)

$(NAME): $(OBJ)
	@$(AR) $(NAME) $(OBJ)

bonus : $(OBJ_B)
	@$(AR) $(NAME) $(OBJ_B)

%.o : %.c
	@$(CC) $(FLAG) -c $< -o $@

clean:
	@rm -f $(OBJ) $(OBJ_B)

fclean: clean
	@rm -f $(NAME)

re: fclean all