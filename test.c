/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:06:06 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/17 16:23:51 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "philo.h"

int ft_atoi(const char *str)
{
	int i;
	int sign;
	int result;

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
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

int	is_digit(char c)
{
	if (c < '0' || c > '9')
		return 0;
	return 1;
}

int	is_number(char *av)
{
	int	i;

	i = 0;
	while (av[i])
	{
		if (!is_digit(av[i]))
			return 0;
		i++;
	}
	return 1;
}

int ft_check_args(int ac, char **av)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("Not valid arguments\n");
		return 0;
	}
	i = 1;
	while (i < ac)
	{
		if (ft_atoi(av[i]) <= 0 || !is_number(av[i]))
		{
			printf("Not valid arguments\n");
			return 0;
		}
		i++;
	}
	return 1;
}

long	ft_get_time()
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	init_data(t_data *data, t_philo *philo, int ac, char **av)
{
	data->num_of_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	data->num_times_to_eat = -1;
	if (ac == 6)
		data->num_times_to_eat = ft_atoi(av[5]);
	data->death = 0;
	data->all_ate = 0;
	data->time_start = ft_get_time();
	// data->philos = philo;
}

int	init_philo(t_philo **philo, t_data *data, int ac)
{
	int	i;

	i = 0;
	*philo = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!*philo)
		return 0;
	while (i < data->num_of_philos)
	{
		(*philo)[i].id = i + 1;
		(*philo)[i].meals_count = 0;
		(*philo)[i].data = data;
		(*philo)[i].last_meal_time = ft_get_time();
		i++;
	}
	data->philos = *philo;
	return 1;
}

void	ft_usleep(size_t time)
{
	long	date;

	date = ft_get_time();
	while (1)
	{
		if (ft_get_time() - date >= time)
			break ;
		usleep(100);
	}
}

void	ft_print_state(t_philo *philo, char *str)
{
	pthread_mutex_lock(&philo->data->print_mutex);
	pthread_mutex_lock(&philo->data->death_mutex);
	if (!philo->data->death)
		printf("%lu %d %s\n", ft_get_time() - philo->data->time_start, philo->id, str);
	pthread_mutex_unlock(&philo->data->death_mutex);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

void	*philo_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	pthread_mutex_unlock(&philo->data->meal_mutex);
	ft_print_state(philo, "is thinking");
	if (philo->id % 2 == 1)
		usleep(500);
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		if (philo->data->death)
		{
			pthread_mutex_unlock(&philo->data->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->data->death_mutex);
		
		pthread_mutex_lock(philo->left_fork);
		ft_print_state(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		ft_print_state(philo, "has taken a fork");

		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->last_meal_time = ft_get_time();
		pthread_mutex_unlock(&philo->data->meal_mutex);
	
		ft_print_state(philo, "is eating");
		ft_usleep(philo->data->time_to_eat);
		philo->meals_count++;
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
	
		ft_print_state(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep);

		ft_print_state(philo, "is thinking");
	}
	return NULL;
}

void	*check_simulation(void *arg)
{
	int		i;
	t_data	*data;

	i = 0;
	data = (t_data *)arg;
	while (1)
	{
		i = 0;
		while (i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time >= data->time_to_die)
			{
				pthread_mutex_unlock(&data->meal_mutex);
				pthread_mutex_lock(&data->death_mutex);
				data->death = 1;
				pthread_mutex_unlock(&data->death_mutex);
				pthread_mutex_lock(&data->print_mutex);
				printf("%lu %d died\n", ft_get_time() - data->time_start, data->philos[i].id);
				pthread_mutex_unlock(&data->print_mutex);
				return NULL;
			}
			pthread_mutex_unlock(&data->meal_mutex);
			i++;
		}
	}
	return NULL;
}

int	init_mutex_thread(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;

	i = 0;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->forks)
		return 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->num_of_philos];
		i++;
	}
	pthread_mutex_init(&data->death_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
	pthread_create(monitor, NULL, check_simulation, data);
	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
	return 1;
}

int main(int ac, char **av)
{
	int			i;
	t_data		data;
	t_philo		*philo;
	pthread_t	monitor;
	
	if(!ft_check_args(ac, av))
		return 1;
	init_data(&data, philo, ac, av);
	if(!init_philo(&philo, &data, ac))
		return 1;
	if(!init_mutex_thread(&data, philo, &monitor))
		return 1;
	i = 0;
	pthread_mutex_destroy(&data.print_mutex);
	pthread_mutex_destroy(&data.death_mutex);
	pthread_mutex_destroy(&data.meal_mutex);
	
	i = 0;
	while (i < data.num_of_philos)
	{
		pthread_mutex_destroy(&data.forks[i]);
		i++;
	}
	return 0;
}