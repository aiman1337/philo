/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutexes.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:34:34 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:14:35 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_mutex_fail(t_data *data)
{
	if (pthread_mutex_init(&(data->death_mutex), NULL)
		|| pthread_mutex_init(&(data->print_mutex), NULL)
		|| pthread_mutex_init(&(data->meal_mutex), NULL))
		return (1);
	return (0);
}

int	create_data_mutexs(t_data *data)
{
	int	i;

	data->forks = malloc(data->num_of_philos * sizeof(pthread_mutex_t));
	if (!data->forks)
		return (0);
	i = 0;
	while (i < data->num_of_philos)
	{
		if (pthread_mutex_init(&(data->forks[i]), NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&(data->forks[i]));
			free(data->forks);
			return (0);
		}
		i++;
	}
	if (check_mutex_fail(data))
	{
		while (--i >= 0)
			pthread_mutex_destroy(&(data->forks[i]));
		free(data->forks);
		return (0);
	}
	return (1);
}

void	destroy_data_mutexs(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&(data->forks[i]));
		i++;
	}
	pthread_mutex_destroy(&(data->death_mutex));
	pthread_mutex_destroy(&(data->print_mutex));
	pthread_mutex_destroy(&(data->meal_mutex));
	free(data->forks);
}

void	ft_print_state(t_data *data, int philo_id, char *state)
{
	unsigned long	timestamp;

	pthread_mutex_lock(&data->print_mutex);
	timestamp = ft_get_time() - data->time_start;
	if (!data->death)
		printf("%-4lu %-3d %s\n", timestamp, philo_id, state);
	pthread_mutex_unlock(&data->print_mutex);
}
