/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:37:48 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:22:03 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_pick_fork(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(&philo->data->death_mutex);
	ft_print_state(philo->data, philo->id, "has taken a fork");
	pthread_mutex_unlock(&philo->data->death_mutex);
	pthread_mutex_lock(philo->right_fork);
	pthread_mutex_lock(&philo->data->death_mutex);
	ft_print_state(philo->data, philo->id, "has taken a fork");
	pthread_mutex_unlock(&philo->data->death_mutex);
}

void	ft_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->meal_mutex); 
	philo->last_meal_time = ft_get_time();
	philo->meals_count++;
	pthread_mutex_unlock(&philo->data->meal_mutex); 
	pthread_mutex_lock(&philo->data->death_mutex);
	ft_print_state(philo->data, philo->id, "is eating");
	pthread_mutex_unlock(&philo->data->death_mutex);
	ft_usleep(philo->data->time_to_eat, philo->data);
}

void	ft_release_fork(t_philo *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	ft_sleep(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death_mutex);
	ft_print_state(philo->data, philo->id, "is sleeping");
	pthread_mutex_unlock(&philo->data->death_mutex);
	ft_usleep(philo->data->time_to_sleep, philo->data);
}

void	ft_think(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death_mutex);
	ft_print_state(philo->data, philo->id, "is thinking");
	pthread_mutex_unlock(&philo->data->death_mutex);
}
