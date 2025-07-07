/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/01 16:31:07 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/23 19:32:13 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	unsigned int	a;
	unsigned int	b;
	char			*newstr;

	a = 0;
	b = 0;
	if (!s1 || !s2)
		return (NULL);
	newstr = malloc(sizeof(char) * ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!newstr)
		return (NULL);
	while (s1[a])
	{
		newstr[a] = s1[a];
		a++;
	}
	while (s2[b])
	{
		newstr[a + b] = s2[b];
		b++;
	}
	newstr[a + b] = '\0';
	return (newstr);
}
