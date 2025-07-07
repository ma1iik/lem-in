/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/25 20:43:26 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/18 23:58:32 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char	*dest;
	unsigned char	*sour;
	size_t			i;

	dest = (unsigned char *)dst;
	sour = (unsigned char *)src;
	i = 0;
	if (!dest && !sour)
		return (NULL);
	if (dest > sour)
	{
		while (len > i)
		{
			dest[len - 1] = sour[len - 1];
			len--;
		}
	}
	else
		ft_memcpy(dest, sour, len);
	return (dest);
}
