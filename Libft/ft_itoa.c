/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misrailo <misrailo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/03 16:13:06 by misrailo          #+#    #+#             */
/*   Updated: 2021/12/21 18:55:15 by misrailo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_itoasize(int n)
{
	int		nbr;
	int		size;

	nbr = n;
	size = 0;
	if (nbr < 0 && nbr > -2147483648)
	{
		size++;
		nbr = -nbr;
	}
	if (nbr == 0)
		return (1);
	else if (nbr == -2147483648)
		return (11);
	while (nbr > 0)
	{
		nbr /= 10;
		size++;
	}
	return (size);
}

static int	ft_isneg(int n)
{
	if (n < 0)
		return (1);
	return (0);
}

static int	ft_makepos(int n)
{
	if (n < 0)
		return (-n);
	else
		return (n);
}

char	*ft_itoa(int n)
{
	char			*str;
	unsigned int	makepos;
	int				isneg;
	int				i;
	int				size;

	i = 1;
	makepos = ft_makepos(n);
	isneg = ft_isneg(n);
	size = ft_itoasize(n);
	str = malloc(sizeof(char) * ft_itoasize(n) + 1);
	if (!str)
		return (NULL);
	if (makepos == 0)
		str[makepos] = '0';
	while (makepos > 0)
	{	
		str[size - i] = (makepos % 10) + '0';
		makepos /= 10;
		i++;
	}
	if (isneg)
		*str = '-';
	str[size] = '\0';
	return (str);
}
