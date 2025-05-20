/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inits.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 10:58:39 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/20 11:27:01 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	init_data(t_data *data, int ac, char **av)
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

int	init_philo(t_philo **philo, t_data *data)
{
	int	i;

	i = 0;
	*philo = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!*philo)
		return (0);
	while (i < data->num_of_philos)
	{
		(*philo)[i].id = i + 1;
		(*philo)[i].meals_count = 0;
		(*philo)[i].data = data;
		(*philo)[i].last_meal_time = ft_get_time();
		i++;
	}
	data->philos = *philo;
	return (1);
}

void	init_threads(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		usleep(100);
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
}

int	init_mutexes(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;

	i = 0;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->forks)
		return (0);
	pthread_mutex_init(&data->death_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	if (data->num_of_philos == 1)
		return (handle_single_philosopher(data, philo));
	while (i < data->num_of_philos)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = 
			&data->forks[(i + 1) % data->num_of_philos];
		i++;
	}
	init_threads(data, philo, monitor);
	return (1);
}
