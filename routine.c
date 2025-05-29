/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:36:33 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:12:05 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*philo_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	pthread_mutex_unlock(&philo->data->meal_mutex);
	ft_think(philo);
	if (philo->id % 2 == 1)
		usleep((philo->data->time_to_eat / 2) * 1000);
	while (1)
	{
		if (ft_check_stop(philo))
			return (NULL);
		ft_pick_fork(philo);
		ft_eat(philo);
		ft_release_fork(philo);
		ft_sleep(philo);
		ft_think(philo);
	}
	return (NULL);
}

void	*lonely_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	ft_print_state(philo->data, philo->id, "is thinking");
	pthread_mutex_lock(philo->left_fork);
	ft_print_state(philo->data, philo->id, "has taken a fork");
	ft_usleep(philo->data->time_to_die, philo->data);
	ft_print_state(philo->data, philo->id, "died");
	pthread_mutex_unlock(philo->left_fork);
	return (NULL);
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
			printf("%lu %d died\n", ft_get_time() - data->time_start, 
				data->philos[i].id);
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

int	ft_check_stop(t_philo *philo)
{
	int	stop;

	pthread_mutex_lock(&philo->data->death_mutex);
	stop = philo->data->death;
	pthread_mutex_unlock(&philo->data->death_mutex);
	return (stop);
}
