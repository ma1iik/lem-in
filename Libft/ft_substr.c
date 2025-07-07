/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/27 16:41:47 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/18 19:52:46 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*newstr;

	if (!s)
		return (NULL);
	if (len > ft_strlen(s))
		len = ft_strlen(s);
	if (ft_strlen(s) < start)
	{
		newstr = ft_calloc(sizeof(char), 1);
		if (!newstr)
			return (NULL);
		return (newstr);
	}
	if (ft_strlen(s) > len)
		newstr = ft_calloc(sizeof(char), len + 1);
	else
		newstr = ft_calloc(sizeof(char), (len - start) + 1);
	if (!newstr)
		return (NULL);
	ft_strncpy(newstr, s + start, len);
	return (newstr);
}
