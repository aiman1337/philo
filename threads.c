/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:33:23 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/28 22:54:21 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	create_philo_threads(t_philo *philo, t_data *data)
{
	int	i;

	data->time_start = ft_get_time();
	if (data->num_of_philos == 1)
	{
		if (pthread_create(&philo[0].thread, NULL, lonely_life, &philo[0]))
			return (0);
		return (1);
	}
	i = 0;
	while (i < data->num_of_philos)
	{
		if (pthread_create(&(philo[i].thread), NULL, philo_life, &philo[i]))
			return (0);
		i++;
	}
	return (1);
}

int	join_threads(t_philo *philo, t_data *data)
{
	pthread_t	monitor_thread;
	int			i;

	if (data->num_of_philos > 1)
	{
		if (pthread_create(&monitor_thread, NULL, ft_simulation, data))
			return (0);
	}
	i = 0;
	while (i < data->num_of_philos)
	{
		if (pthread_join(philo[i].thread, NULL))
			return (0);
		i++;
	}
	if (data->num_of_philos > 1)
		pthread_join(monitor_thread, NULL);
	return (1);
}

int	init_thread(t_philo *philo, t_data *data)
{
	if (!create_philo_threads(philo, data))
		return (0);
	return (join_threads(philo, data));
}
