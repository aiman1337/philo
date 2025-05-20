/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 11:07:15 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/20 11:29:51 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_usleep(long time, t_data *data)
{
	long	date;

	date = ft_get_time();
	while (1)
	{
		pthread_mutex_lock(&data->death_mutex);
		if (data->death)
		{
			pthread_mutex_unlock(&data->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->death_mutex);
		if (ft_get_time() - date >= time)
			break ;
		usleep(100);
	}
}

void	*check_simulation(void *arg)
{
	t_data	*data;
	int		all_ate;

	data = (t_data *)arg;
	while (1)
	{
		all_ate = 1;
		if (check_philosopher_status(data, &all_ate))
			return (NULL);
		if (check_all_ate(data, all_ate))
			return (NULL);
	}
	return (NULL);
}

void	philo_sleep_and_think(t_philo *philo)
{
	ft_print_state(philo, "is sleeping");
	ft_usleep(philo->data->time_to_sleep, philo->data);
	ft_print_state(philo, "is thinking");
}

int	check_all_ate(t_data *data, int all_ate)
{
	pthread_mutex_lock(&data->death_mutex);
	if (data->num_times_to_eat != -1 && all_ate)
	{
		data->death = 1;
		data->all_ate = 1;
		pthread_mutex_unlock(&data->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&data->death_mutex);
	return (0);
}

int	check_philosopher_status(t_data *data, int *all_ate)
{
	int	i;

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
			printf("%lu %d died\n", 
				ft_get_time() - data->time_start, data->philos[i].id);
			pthread_mutex_unlock(&data->print_mutex);
			return (1);
		}
		if (data->num_times_to_eat != -1 && 
			data->philos[i].meals_count < data->num_times_to_eat)
			*all_ate = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		i++;
	}
	return (0);
}
