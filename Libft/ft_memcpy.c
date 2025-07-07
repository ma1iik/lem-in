/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/23 11:40:51 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/18 23:21:03 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	void	*origin;

	if (!dst && !src)
		return (NULL);
	origin = dst;
	while (n > 0)
	{
		*((char *)dst) = *((char *)src);
		n--;
		dst++;
		src++;
	}
	return (origin);
}
