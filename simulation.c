/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 11:04:47 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/24 20:25:25 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_eat(t_philo *philo)
{
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
}

void	ft_print_state(t_philo *philo, char *str)
{
	pthread_mutex_lock(&philo->data->print_mutex);
	pthread_mutex_lock(&philo->data->death_mutex);
	if (!philo->data->death)
		printf("%lu %d %s\n", 
			ft_get_time() - philo->data->time_start, philo->id, str);
	pthread_mutex_unlock(&philo->data->death_mutex);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

int	handle_single_philosopher(t_data *data, t_philo *philo)
{
	pthread_mutex_init(&data->forks[0], NULL);
	data->philos[0].left_fork = &data->forks[0];
	pthread_create(&philo[0].thread, NULL, lonely_life, &philo[0]);
	pthread_join(philo[0].thread, NULL);
	return (1);
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
		philo_eat(philo);
		philo_sleep_and_think(philo);
	}
	return (NULL);
}

void	*lonely_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	ft_print_state(philo, "is thinking");
	pthread_mutex_lock(philo->left_fork);
	ft_print_state(philo, "has taken a fork");
	ft_usleep(philo->data->time_to_die, philo->data);
	ft_print_state(philo, "died");
	pthread_mutex_unlock(philo->left_fork);
	return (NULL);
}
