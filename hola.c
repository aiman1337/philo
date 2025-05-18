/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hola.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:06:06 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/18 18:48:55 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int ft_atoi(const char *str)
{
	int i = 0;
	int sign = 1;
	int result = 0;

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

int is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

int is_number(char *av)
{
	int i = 0;
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
	int i = 1;

	if (ac != 5 && ac != 6)
	{
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return 0;
	}
	while (i < ac)
	{
		if (ft_atoi(av[i]) <= 0 || !is_number(av[i]))
		{
			printf("Arguments must be positive integers\n");
			return 0;
		}
		i++;
	}
	return 1;
}

long ft_get_time()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void init_data(t_data *data, int ac, char **av)
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
}

int init_philo(t_philo **philo, t_data *data)
{
	int i = 0;

	*philo = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!*philo)
		return 0;
	while (i < data->num_of_philos)
	{
		(*philo)[i].id = i + 1;
		(*philo)[i].meals_count = 0;
		(*philo)[i].data = data;
		(*philo)[i].last_meal_time = data->time_start;
		i++;
	}
	data->philos = *philo;
	return 1;
}

void ft_usleep(unsigned long time, t_data *data)
{
	unsigned long start = ft_get_time();

	while (ft_get_time() - start < time)
	{
		usleep(100);
		pthread_mutex_lock(&data->death_mutex);
		if (data->death)
		{
			pthread_mutex_unlock(&data->death_mutex);
			break;
		}
		pthread_mutex_unlock(&data->death_mutex);
	}
}

void ft_print_state(t_philo *philo, char *str)
{
	pthread_mutex_lock(&philo->data->print_mutex);
	pthread_mutex_lock(&philo->data->death_mutex);
	if (!philo->data->death)
		printf("%lu %d %s\n", ft_get_time() - philo->data->time_start, philo->id, str);
	pthread_mutex_unlock(&philo->data->death_mutex);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

void *philo_life(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	pthread_mutex_unlock(&philo->data->meal_mutex);
	
	ft_print_state(philo, "is thinking");
	
	// Better staggering to prevent deadlock
	if (philo->id % 2 == 0)
		ft_usleep(philo->data->time_to_eat / 2, philo->data);
		// usleep(1500);
	
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		if (philo->data->death)
		{
			pthread_mutex_unlock(&philo->data->death_mutex);
			break;
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
		ft_usleep(philo->data->time_to_eat, philo->data);

		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->meals_count++;
		pthread_mutex_unlock(&philo->data->meal_mutex);

		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);

		ft_print_state(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep, philo->data);

		ft_print_state(philo, "is thinking");
	}
	return NULL;
}

void *check_simulation(void *arg)
{
	t_data *data = (t_data *)arg;
	int i;
	int all_ate;

	while (1)
	{
		i = 0;
		all_ate = 1;
		while (i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
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
			if (data->num_times_to_eat != -1 && 
				data->philos[i].meals_count < data->num_times_to_eat)
				all_ate = 0;
			pthread_mutex_unlock(&data->meal_mutex);
			i++;
		}
		pthread_mutex_lock(&data->death_mutex);
		if (all_ate && data->num_times_to_eat != -1)
		{
			data->all_ate = 1;
			data->death = 1;
			pthread_mutex_unlock(&data->death_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&data->death_mutex);
	}
	return NULL;
}

int init_mutexes(t_data *data)
{
	int i = 0;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->forks)
		return 0;
	while (i < data->num_of_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return 0;
		i++;
	}
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0 ||
		pthread_mutex_init(&data->death_mutex, NULL) != 0 ||
		pthread_mutex_init(&data->meal_mutex, NULL) != 0)
		return 0;
	return 1;
}

int main(int ac, char **av)
{
	t_data data;
	t_philo *philo;
	pthread_t monitor;
	int i;

	if (!ft_check_args(ac, av))
		return 1;
	
	init_data(&data, ac, av);
	if (!init_philo(&philo, &data))
		return (1);
	
	if (!init_mutexes(&data))
		return (1);
	i = 0;
	while (i < data.num_of_philos)
	{
		philo[i].left_fork = &data.forks[i];
		philo[i].right_fork = &data.forks[(i + 1) % data.num_of_philos];
		i++;
	}
	i = 0;
	while (i < data.num_of_philos)
	{
		if (pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]) != 0)
			return (1);
		i++;
	}

	if (pthread_create(&monitor, NULL, check_simulation, &data) != 0)
		return (1);
	i = 0;
	while (i < data.num_of_philos)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	return (0);
}