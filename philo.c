/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:31:47 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/17 19:20:05 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

unsigned long ft_get_time()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void ft_usleep(long time, t_data *data)
{
	unsigned long date;

	date = ft_get_time();
	while (ft_get_time() - date < time)
	{
		pthread_mutex_lock(&data->death_mutex);
		if (data->death)
		{
			pthread_mutex_unlock(&data->death_mutex);
			break;
		}
		pthread_mutex_unlock(&data->death_mutex);
		usleep(100);
	}
}

int ft_atoi(const char *str)
{
	int i;
	int sign;
	int result;

	i = 0;
	sign = 1;
	result = 0;

	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

int check_args(int ac, char **av)
{
	int i;
	int j;

	if (ac < 5 || ac > 6)
	{
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return 0;
	}
	i = 1;
	while (i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				printf("Arguments must be positive integers\n");
				return 0;
			}
			j++;
		}
		if (ft_atoi(av[i]) <= 0)
		{
			printf("Arguments must be positive integers\n");
			return 0;
		}
		i++;
	}
	return 1;
}

void init_data(t_data *data, int ac, char **av)
{
	data->num_of_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	data->num_times_to_eat = -1;
	if (ac == 6)
		data->num_times_to_eat = ft_atoi(av[5]);
	data->time_start = ft_get_time();
	data->death = 0;
	data->all_ate = 0;
}

void ft_print_mutex(char *str, t_data *data, t_philo *philo)
{
	pthread_mutex_lock(&data->death_mutex);
	if (!data->death)
	{
		pthread_mutex_unlock(&data->death_mutex);
		pthread_mutex_lock(&data->print_mutex);
		printf("%lu %d %s\n", ft_get_time() - data->time_start, philo->id, str);
		pthread_mutex_unlock(&data->print_mutex);
	}
	else
		pthread_mutex_unlock(&data->death_mutex);
}

void *philo_life(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	
	// Initialize last_meal_time properly at the start
	pthread_mutex_lock(&philo->data->meal_mutex);
	philo->last_meal_time = ft_get_time();
	pthread_mutex_unlock(&philo->data->meal_mutex);
	
	ft_print_mutex("is thinking", philo->data, philo);
	
	// Stagger philosopher start times to avoid deadlock
	if (philo->id % 2 == 1)
			ft_usleep(philo->data->time_to_eat / 2, philo->data);
		// usleep(500);
	
	while (1)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		if (philo->data->death)
		{
			pthread_mutex_unlock(&philo->data->death_mutex);
			break;
		}
		pthread_mutex_unlock(&philo->data->death_mutex);
		
		// Eat process
		pthread_mutex_lock(philo->left_fork);
		ft_print_mutex("has taken a fork", philo->data, philo);
		pthread_mutex_lock(philo->right_fork);
		ft_print_mutex("has taken a fork", philo->data, philo);
		
		// Update meal time with mutex protection
		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->last_meal_time = ft_get_time();
		pthread_mutex_unlock(&philo->data->meal_mutex);
		
		ft_print_mutex("is eating", philo->data, philo);
		ft_usleep(philo->data->time_to_eat, philo->data);
		
		// Update meals count
		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->meals_count++;
		pthread_mutex_unlock(&philo->data->meal_mutex);
		
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		
		ft_print_mutex("is sleeping", philo->data, philo);
		ft_usleep(philo->data->time_to_sleep, philo->data);
		
		ft_print_mutex("is thinking", philo->data, philo);
	}
	return NULL;
}

void *check_simulation(void *arg)
{
	t_data *data = (t_data *)arg;
	int i;
	int all_ate;

	while (1)
	{
		i = 0;
		all_ate = 1;

		while (i < data->num_of_philos)
		{
			// Check if philosopher died with proper mutex protection
			pthread_mutex_lock(&data->meal_mutex);
			if (ft_get_time() - data->philos[i].last_meal_time >= data->time_to_die)
			{
				pthread_mutex_unlock(&data->meal_mutex);
				pthread_mutex_lock(&data->death_mutex);
				data->death = 1;
				pthread_mutex_unlock(&data->death_mutex);
				
				pthread_mutex_lock(&data->print_mutex);
				printf("%lu %d died\n", ft_get_time() - data->time_start, data->philos[i].id);
				pthread_mutex_unlock(&data->print_mutex);
				
				return NULL; // Exit the thread after death is detected
			}
			
			// Check if all philosophers have eaten enough times
			if (data->num_times_to_eat != -1 && 
				data->philos[i].meals_count < data->num_times_to_eat)
				all_ate = 0;
			
			pthread_mutex_unlock(&data->meal_mutex);
			i++;
		}
		
		// If all philosophers have eaten enough, end simulation
		pthread_mutex_lock(&data->death_mutex);
		if (data->num_times_to_eat != -1 && all_ate)
		{
			data->all_ate = 1;
			data->death = 1;
			pthread_mutex_unlock(&data->death_mutex);
			return NULL;
		}
		pthread_mutex_unlock(&data->death_mutex);
		
		usleep(1000); // Small sleep to avoid CPU hogging
	}
	
	return NULL;
}

int main(int ac, char **av)
{
	t_data      data;
	t_philo     *philo;
	pthread_t   radar;
	int         i;

	if (!check_args(ac, av))
		return 1;
	
	init_data(&data, ac, av);
	
	philo = malloc(sizeof(t_philo) * data.num_of_philos);
	if (!philo)
		return 1;

	data.forks = malloc(sizeof(pthread_mutex_t) * data.num_of_philos);
	if (!data.forks)
	{
		free(philo);
		return 1;
	}
		
	i = 0;
	while (i < data.num_of_philos)
	{
		pthread_mutex_init(&data.forks[i], NULL);
		i++;
	}

	pthread_mutex_init(&data.print_mutex, NULL);
	pthread_mutex_init(&data.death_mutex, NULL);
	pthread_mutex_init(&data.meal_mutex, NULL);
	
	i = 0;
	while (i < data.num_of_philos)
	{
		philo[i].id = i + 1;
		philo[i].meals_count = 0;
		philo[i].last_meal_time = ft_get_time(); // Initialize with current time
		philo[i].left_fork = &data.forks[i];
		philo[i].right_fork = &data.forks[(i + 1) % data.num_of_philos];
		philo[i].data = &data;
		i++;
	}
	
	data.philos = philo;
	
	// Handle the special case of only one philosopher
	if (data.num_of_philos == 1)
	{
		pthread_create(&philo[0].thread, NULL, philo_life, &philo[0]);
		ft_usleep(data.time_to_die + 10, &data);
		pthread_mutex_lock(&data.print_mutex);
		printf("%lu %d died\n", ft_get_time() - data.time_start, 1);
		pthread_mutex_unlock(&data.print_mutex);
		pthread_join(philo[0].thread, NULL);
	}
	else
	{
		// Create philosopher threads
		i = 0;
		while (i < data.num_of_philos)
		{
			if (pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]) != 0)
			{
				write(2, "Failed to create thread\n", 24);
				return 1;
			}
			i++;
		}
		
		// Create monitor thread
		if (pthread_create(&radar, NULL, check_simulation, &data) != 0)
		{
			write(2, "Failed to create monitor thread\n", 31);
			return 1;
		}
		
		// Join philosopher threads
		i = 0;
		while (i < data.num_of_philos)
		{
			if (pthread_join(philo[i].thread, NULL) != 0)
			{
				write(2, "Failed to join thread\n", 22);
				return 1;
			}
			i++;
		}
		
		// Join monitor thread
		pthread_join(radar, NULL);
	}
	
	// Clean up
	pthread_mutex_destroy(&data.print_mutex);
	pthread_mutex_destroy(&data.death_mutex);
	pthread_mutex_destroy(&data.meal_mutex);
	
	i = 0;
	while (i < data.num_of_philos)
	{
		pthread_mutex_destroy(&data.forks[i]);
		i++;
	}
	
	free(philo);
	free(data.forks);
	
	return 0;
}