/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:35:36 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:09:59 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

int	ft_stop_simulation(t_data *data)
{
	int	stop;

	pthread_mutex_lock(&data->death_mutex);
	stop = data->death;
	pthread_mutex_unlock(&data->death_mutex);
	return (stop);
}

int	ft_check_death(t_philo *philo, t_data *data)
{
	unsigned long	time_since_last_meal;

	pthread_mutex_lock(&data->meal_mutex);
	time_since_last_meal = ft_get_time() - philo->last_meal_time;
	pthread_mutex_unlock(&data->meal_mutex);
	if (time_since_last_meal >= data->time_to_die)
	{
		pthread_mutex_lock(&data->death_mutex);
		if (!data->death)
		{
			ft_print_state(data, philo->id, "died");
			data->death = 1;
			pthread_mutex_unlock(&data->death_mutex);
			return (1);
		}
		pthread_mutex_unlock(&data->death_mutex);
		return (1);
	}
	return (0);
}

int	ft_check_meals(t_data *data)
{
	int	completed_meals;
	int	i;

	completed_meals = 1;
	if (data->num_times_to_eat <= 0)
		return (0);
	pthread_mutex_lock(&data->meal_mutex);
	i = -1;
	while (++i < data->num_of_philos)
	{
		if (data->philos[i].meals_count < data->num_times_to_eat)
		{
			completed_meals = 0;
			break ;
		}
	}
	pthread_mutex_unlock(&data->meal_mutex);
	if (completed_meals)
	{
		pthread_mutex_lock(&data->death_mutex);
		data->death = 1;
		pthread_mutex_unlock(&data->death_mutex);
		return (1);
	}
	return (0);
}

void	*ft_simulation(void *arg)
{
	t_data	*data;
	int		i;

	data = (t_data *)arg;
	while (!ft_stop_simulation(data))
	{
		if (ft_check_meals(data))
			return (NULL);
		i = 0;
		while (i < data->num_of_philos)
		{
			if (ft_check_death(&data->philos[i], data))
				return (NULL);
			i++;
			usleep(100);
		}
	}
	return (NULL);
}
