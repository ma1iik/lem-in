/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 17:37:38 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/21 18:55:53 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dst, const void *src, int c, size_t n)
{
	unsigned char	*dest;
	unsigned char	*sour;
	unsigned char	cr;

	dest = (unsigned char *)dst;
	sour = (unsigned char *)src;
	cr = (unsigned char)c;
	if (!dst && !src)
		return (NULL);
	dest = dst;
	while (n > 0)
	{
		*dest = *sour;
		n--;
		dest++;
		if (*sour == cr)
			return (dest);
		sour++;
	}
	return (NULL);
}
