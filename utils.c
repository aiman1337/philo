/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:18:14 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:28:47 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ft_atoi(const char *str)
{
	int		i;
	int		sign;
	long	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

int	is_digit(char c)
{
	if (c < '0' || c > '9')
		return (0);
	return (1);
}

int	is_number(char *av)
{
	int	i;

	i = 0;
	if (av[i] == '+')
		i++;
	while (av[i])
	{
		if (!is_digit(av[i]))
			return (0);
		i++;
	}
	return (1);
}

int	ft_check_optional_parameter(int ac, char **av)
{
	if (ac == 6)
	{
		if (!is_number(av[5]))
			return (0);
	}
	return (1);
}

int	ft_check_args(int ac, char **av)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("Not valid arguments\n");
		return (0);
	}
	i = 1;
	while (i < ac && i != 5)
	{
		if (ft_atoi(av[i]) <= 0 || !is_number(av[i]))
		{
			printf("Not valid arguments\n");
			return (0);
		}
		i++;
	}
	if (!ft_check_optional_parameter(ac, av))
	{
		printf("Not valid arguments\n");
		return (0);
	}
	return (1);
}
