#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct s_philo
{
	int		number_of_philosopher;
	float	time_to_die;
	float	time_to_eat;
	float	time_to_sleep;
	int		must_eat;

}	t_philo;

int	ft_isdigit(int x)
{
	if (x >= '0' && x <= '9')
		return (1);
	return (0);
}

int	is_number(char *str)
{
	int	i;

	i = 0;
	while(str[i])
	{
		if (!ft_isdigit(str[i]))
			return 0;
		i++;
	}
	return 1;
}

int	ft_atoi(const char *str)
{
	int		sign;
	long	res;

	res = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		res = (res * 10) + (*str - '0');
		str++;
	}
	return ((int)(res * sign));
}

int main(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		write(2, "not valid arguments\n", 20);
		return 0;
	}
	int i = 1;
	while(i < ac)
	{
		if (!is_number(av[i]))
		{
			write(2, "not valid arguments\n", 20);
			return 0;
		}
		printf("%d \n", ft_atoi(av[i]));
		if (ft_atoi(av[i]) <= 0)
		{
			if (i != 6)
				write(2, "not valid arguments\n", 20);
			return 0;
		}
		i++;
	}
	return 0;
}