/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:18:02 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:55:43 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ft_get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

int	death_getter(t_data *data)
{
	int	death;

	pthread_mutex_lock(&data->death_mutex);
	death = data->death;
	pthread_mutex_unlock(&data->death_mutex);
	return (death);
}

void	ft_usleep(size_t time, t_data *data)
{
	long	date;

	date = ft_get_time() + time;
	while (ft_get_time() < date)
	{
		if (death_getter(data))
			break ;
		usleep(100);
	}
}

int	start_simulation(t_data *data, char **av, int ac)
{
	t_philo	*philo;

	init_data(data, ac, av);
	if (!create_data_mutexs(data))
		return (1);
	if (data->num_times_to_eat == 0)
	{
		destroy_data_mutexs(data);
		return (0);
	}
	if (!init_philo(&philo, data))
	{
		destroy_data_mutexs(data);
		return (1);
	}
	data->philos = philo;
	if (!init_thread(philo, data))
	{
		destroy_data_mutexs(data);
		free(philo);
		return (1);
	}
	destroy_data_mutexs(data);
	free(philo);
	return (0);
}

int	main(int ac, char **av)
{
	t_data	data;

	if (!ft_check_args(ac, av))
		return (1);
	return (start_simulation(&data, av, ac));
}
