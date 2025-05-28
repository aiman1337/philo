/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:06:06 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/24 18:52:47 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ft_get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	destroy_mutexes(t_data *data)
{
	int	i;

	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->death_mutex);
	pthread_mutex_destroy(&data->meal_mutex);
	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_data		data;
	t_philo		*philo;
	pthread_t	monitor;

	if (ac != 5 && ac != 6)
	{
		printf("Not valid arguments\n");
		return (1);
	}
	if (!ft_check_args(ac, av))
		return (1);
	init_data(&data, ac, av);
	if (data.num_times_to_eat == 0)
		return (0);
	if (!init_philo(&philo, &data))
		return (1);
	if (!init_mutexes(&data, philo, &monitor))
		return (1);
	destroy_mutexes(&data);
	free(philo);
	free(data.forks);
	return (0);
}
