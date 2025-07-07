/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 19:09:47 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/18 19:52:46 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"libft.h"

void	*ft_memchr(const void *s, int c, size_t len)
{
	unsigned char	*st;
	size_t			i;

	st = (unsigned char *)s;
	i = 0;
	while (len > i)
	{
		if (*st == (unsigned char)c)
			return ((unsigned char *)st);
		st++;
		i++;
	}
	return (NULL);
}
