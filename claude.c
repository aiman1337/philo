/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   claude.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 11:15:12 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/28 11:24:25 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:06:06 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/28 11:07:37 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test.h"

long ft_atoi(const char *str)
{
	int i;
	int sign;
	long result;

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
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return -1;
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (sign * result);
}

int	is_digit(char c)
{
	if (c < '0' || c > '9')
		return 0;
	return 1;
}

int	is_number(char *av)
{
	int	i;

	i = 0;
	if (av[i] == '+')
		i++;
	while (av[i])
	{
		if (!is_digit(av[i]))
			return 0;
		i++;
	}
	return 1;
}

int ft_check_args(int ac, char **av)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("Not valid arguments\n");
		return 0;
	}
	i = 1;
	while (i < ac && i != 5)
	{
		if (ft_atoi(av[i]) <= 0 || !is_number(av[i]))
		{
			printf("Not valid arguments\n");
			return 0;
		}
		i++;
	}
	if (ac == 6)
	{
		if (!is_number(av[i]))
		{
			printf("Not valid arguments\n");
			return 0;
		}
	}
	return 1;
}

long	ft_get_time()
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	init_data(t_data *data, int ac, char **av)
{
	data->num_of_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	data->num_times_to_eat = -1;
	if (ac == 6)
		data->num_times_to_eat = ft_atoi(av[5]);
	data->death = 0;
	data->all_ate = 0;
	data->time_start = ft_get_time();
}

int	init_philo(t_philo **philo, t_data *data, int ac)
{
	int	i;

	i = 0;
	*philo = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!*philo)
		return 0;
	while (i < data->num_of_philos)
	{
		(*philo)[i].id = i + 1;
		(*philo)[i].meals_count = 0;
		(*philo)[i].data = data;
		(*philo)[i].last_meal_time = data->time_start;
		i++;
	}
	data->philos = *philo;
	return 1;
}

int	death_getter(t_data *data)
{
	int	death;

	pthread_mutex_lock(&data->death_mutex);
	death = data->death;
	pthread_mutex_unlock(&data->death_mutex);
	return (death);
}

// OPTIMIZATION 1: Better precision sleep with adaptive sleep times
void	ft_usleep(size_t time, t_data *data)
{
	long	start;
	long	current;
	long	remaining;

	start = ft_get_time();
	while (1)
	{
		current = ft_get_time();
		remaining = time - (current - start);
		
		if (remaining <= 0)
			break;
		if (death_getter(data))
			break;
		
		// Adaptive sleep: larger sleeps for longer waits, smaller for precision
		if (remaining > 10)
			usleep(1000);  // 1ms
		else if (remaining > 1)
			usleep(100);   // 100us
		else
			usleep(10);    // 10us for final precision
	}
}

void	ft_print_state(t_philo *philo, char *str)
{
	pthread_mutex_lock(&philo->data->print_mutex);
	if (!death_getter(philo->data))
		printf("%lu %d %s\n", ft_get_time() - philo->data->time_start, philo->id, str);
	pthread_mutex_unlock(&philo->data->print_mutex);
}

// OPTIMIZATION 2: Better fork order to prevent deadlock without delays
void	take_forks(t_philo *philo)
{
	pthread_mutex_t *first_fork;
	pthread_mutex_t *second_fork;
	
	// Always take lower numbered fork first to prevent deadlock
	if (philo->id % 2 == 1)
	{
		first_fork = philo->left_fork;
		second_fork = philo->right_fork;
	}
	else
	{
		first_fork = philo->right_fork;
		second_fork = philo->left_fork;
	}
	
	pthread_mutex_lock(first_fork);
	ft_print_state(philo, "has taken a fork");
	pthread_mutex_lock(second_fork);
	ft_print_state(philo, "has taken a fork");
}

void	release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	*philo_life(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	ft_print_state(philo, "is thinking");

	if (philo->id % 2 == 1)
		usleep(500);
	
	while (!death_getter(philo->data))
	{
		take_forks(philo);

		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->last_meal_time = ft_get_time();
		pthread_mutex_unlock(&philo->data->meal_mutex);
			
		ft_print_state(philo, "is eating");
		ft_usleep(philo->data->time_to_eat, philo->data);

		pthread_mutex_lock(&philo->data->meal_mutex);
		philo->meals_count++;
		pthread_mutex_unlock(&philo->data->meal_mutex);

		release_forks(philo);
		
		if (death_getter(philo->data))
			break;
			
		ft_print_state(philo, "is sleeping");
		ft_usleep(philo->data->time_to_sleep, philo->data);

		if (death_getter(philo->data))
			break;

		ft_print_state(philo, "is thinking");
	}
	return NULL;
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
	return NULL;
}

int check_all_ate(t_data *data, int all_ate)
{
	pthread_mutex_lock(&data->death_mutex);
	if (data->num_times_to_eat != -1 && all_ate)
	{
		data->death = 1;
		data->all_ate = 1;
		pthread_mutex_unlock(&data->death_mutex);
		return 1;
	}
	pthread_mutex_unlock(&data->death_mutex);
	return 0;
}

int check_philosopher_status(t_data *data, int *all_ate)
{
	int i = 0;
	long current_time = ft_get_time();
	
	while (i < data->num_of_philos)
	{
		pthread_mutex_lock(&data->meal_mutex);

		if (current_time - data->philos[i].last_meal_time >= data->time_to_die)
		{
			pthread_mutex_unlock(&data->meal_mutex);
			pthread_mutex_lock(&data->death_mutex);
			data->death = 1;
			pthread_mutex_unlock(&data->death_mutex);
			pthread_mutex_lock(&data->print_mutex);
			printf("%lu %d died\n", current_time - data->time_start, data->philos[i].id);
			pthread_mutex_unlock(&data->print_mutex);
			return 1;
		}

		if (data->num_times_to_eat != -1 && data->philos[i].meals_count < data->num_times_to_eat)
			*all_ate = 0;
		
		pthread_mutex_unlock(&data->meal_mutex);
		i++;
	}
	return 0;
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
			return NULL;
		if (check_all_ate(data, all_ate))
			return NULL;
		if (data->time_to_die > 100)
			usleep(1000);
		else
			usleep(500);
	}
	return NULL;
}

void	init_threads(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;
	long start_time = ft_get_time();
	
	i = 0;
	while (i < data->num_of_philos)
	{
		philo[i].last_meal_time = start_time;
		pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]);
		i++;
	}
	pthread_create(monitor, NULL, check_simulation, data);
	
	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_join(philo[i].thread, NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
}

int	init_mutexes(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;

	i = 0;
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
	if (!data->forks)
		return (0);
	pthread_mutex_init(&data->death_mutex, NULL);
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	
	if (data->num_of_philos == 1)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		data->philos[i].left_fork = &data->forks[i];
		pthread_create(&philo[i].thread, NULL, lonely_life, &philo[i]);
		pthread_join(philo[i].thread, NULL);
		return 1;
	}
	
	while (i < data->num_of_philos)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->num_of_philos];
		i++;
	}
	init_threads(data, philo, monitor);
	return (1);
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

int main(int ac, char **av)
{
	int			i;
	t_data		data;
	t_philo		*philo;
	pthread_t	monitor;

	if(!ft_check_args(ac, av))
		return (1);
	init_data(&data, ac, av);
	if (data.num_times_to_eat == 0)
		return (0);
	if(!init_philo(&philo, &data, ac))
		return (1);
	if(!init_mutexes(&data, philo, &monitor))
		return (1);
	destroy_mutexes(&data);
	free(data.forks);
	free(philo);
	return 0;
}
