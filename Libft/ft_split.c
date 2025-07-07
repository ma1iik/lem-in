/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/01 18:07:12 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/29 18:18:25 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_wordcount(const char *str, char c)
{
	int		i;
	int		words;

	i = 0;
	words = 0;
	while (str[i])
	{
		if (str[i] == c)
			i++;
		else
		{
			words++;
			while (str[i] != '\0' && str[i] != c)
				i++;
		}
	}
	return (words);
}

char	*ft_words(const char *str, char c)
{
	int		i;
	int		l;
	char	*word;

	i = 0;
	l = 0;
	while (*str == c)
		str++;
	while (str[i] != c && str[i] != '\0')
		i++;
	word = malloc(sizeof(char) * (i + 1));
	if (word == NULL)
		return (NULL);
	while (str[l] != c && str[l] != '\0')
	{
		word[l] = str[l];
		l++;
	}
	word[l] = '\0';
	return (word);
}

void	ft_freemem(char **ptr_str, int i)
{
	while (i > 0)
	{
		free(ptr_str[i - 1]);
		i--;
	}
}

char	**ft_split(char const *s, char c)
{
	int		i;
	char	**ptr_str;
	int		words;

	i = 0;
	words = ft_wordcount(s, c);
	if (!s)
		return (NULL);
	ptr_str = malloc(sizeof(char *) * (words + 1));
	if (!ptr_str)
		return (NULL);
	while (i < words)
	{
		while (*s && *s == c)
			s++;
		ptr_str[i] = ft_words(s, c);
		if (ptr_str[i] == NULL)
			ft_freemem(ptr_str, i);
		while (*s && *s != c)
			s++;
		i++;
	}
	ptr_str[i] = 0;
	return (ptr_str);
}
