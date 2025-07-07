/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 20:43:10 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/19 21:52:34 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	a;

	i = 0;
	a = 0;
	if ((*little == '\0') || (!big))
		return ((char *)big);
	while (big[i] && i < len)
	{
		while (big[i + a] == little[a] && (i + a < len))
		{
			if (little[a + 1] == '\0')
				return ((char *)&big[i]);
			a++;
		}
		i++;
	}
	return (NULL);
}
