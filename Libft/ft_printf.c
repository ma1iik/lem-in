/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ma1iik <ma1iik@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 20:30:00 by ma1iik            #+#    #+#             */
/*   Updated: 2025/07/09 02:34:12 by ma1iik           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdarg.h>

static int	ft_putchar_pf(char c)
{
	return (write(1, &c, 1));
}

static int	ft_putstr_pf(char *str)
{
	if (!str)
	{
		write(1, "(null)", 6);
		return (6);
	}
	ft_putstr_fd(str, 1);
	return (ft_strlen(str));
}

static int	ft_putnbr_pf(int n)
{
	int		len;
	char	*str;

	str = ft_itoa(n);
	if (!str)
		return (0);
	len = ft_strlen(str);
	ft_putstr_fd(str, 1);
	free(str);
	return (len);
}

static int	ft_putnbr_unsigned(unsigned int n)
{
	int		len;

	len = 0;
	if (n >= 10)
	{
		len += ft_putnbr_unsigned(n / 10);
		len += ft_putchar_pf((n % 10) + '0');
	}
	else
		len += ft_putchar_pf(n + '0');
	return (len);
}

static int	ft_puthex_lower(unsigned long long n)
{
	char	*base;
	int		len;

	base = "0123456789abcdef";
	len = 0;
	if (n >= 16)
	{
		len += ft_puthex_lower(n / 16);
		len += ft_putchar_pf(base[n % 16]);
	}
	else
		len += ft_putchar_pf(base[n]);
	return (len);
}

static int	ft_puthex_upper(unsigned int n)
{
	char	*base;
	int		len;

	base = "0123456789ABCDEF";
	len = 0;
	if (n >= 16)
	{
		len += ft_puthex_upper(n / 16);
		len += ft_putchar_pf(base[n % 16]);
	}
	else
		len += ft_putchar_pf(base[n]);
	return (len);
}

static int	ft_putptr(unsigned long long ptr)
{
	int	len;

	len = 0;
	if (ptr == 0)
		len += ft_putstr_pf("0x0");
	else
	{
		len += ft_putstr_pf("0x");
		len += ft_puthex_lower(ptr);
	}
	return (len);
}

static int	ft_handle_format(va_list ap, char c)
{
	int	len;

	len = 0;
	if (c == 'c')
		len += ft_putchar_pf(va_arg(ap, int));
	else if (c == 's')
		len += ft_putstr_pf(va_arg(ap, char *));
	else if (c == 'p')
		len += ft_putptr(va_arg(ap, unsigned long long));
	else if (c == 'd' || c == 'i')
		len += ft_putnbr_pf(va_arg(ap, int));
	else if (c == 'u')
		len += ft_putnbr_unsigned(va_arg(ap, unsigned int));
	else if (c == 'x')
		len += ft_puthex_lower(va_arg(ap, unsigned int));
	else if (c == 'X')
		len += ft_puthex_upper(va_arg(ap, unsigned int));
	else if (c == '%')
		len += ft_putchar_pf('%');
	else
	{
		len += ft_putchar_pf('%');
		len += ft_putchar_pf(c);
	}
	return (len);
}

int	ft_printf(const char *format, ...)
{
	va_list	ap;
	int		i;
	int		len;

	if (!format)
		return (-1);
	va_start(ap, format);
	i = 0;
	len = 0;
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1])
		{
			i++;
			len += ft_handle_format(ap, format[i]);
		}
		else
			len += ft_putchar_pf(format[i]);
		i++;
	}
	va_end(ap);
	return (len);
}