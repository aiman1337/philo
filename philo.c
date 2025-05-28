/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 22:18:02 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/28 22:22:55 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "philo.h"

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

int	print_error(char *err)
{
	printf("%s\n", err);
	return (0);
}

void	ft_print_state(t_data *data, int philo_id, char *state)
{
	unsigned long	timestamp;

	pthread_mutex_lock(&data->print_mutex);
	timestamp = ft_get_time() - data->time_start;
	if (!data->death)
		printf("%-4lu %-3d %s\n", timestamp, philo_id, state);
	pthread_mutex_unlock(&data->print_mutex);
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
	data->time_start = 0;
}

int	init_philo(t_philo **philo, t_data *data)
{
	int	i;

	i = 0;
	*philo = malloc(sizeof(t_philo) * data->num_of_philos);
	if (!*philo)
		return 0;
	while (i < data->num_of_philos)
	{
		(*philo)[i].id = i + 1;
		(*philo)[i].data = data;
		(*philo)[i].meals_count = 0;
		(*philo)[i].last_meal_time = 0;
		(*philo)[i].left_fork = &data->forks[i];
		(*philo)[i].right_fork = &data->forks[(i + 1) % data->num_of_philos];
		i++;
	}
	return 1;
}

int	ft_check_stop(t_philo *philo)
{
	int	stop;

	pthread_mutex_lock(&philo->data->death_mutex);
	stop = philo->data->death;
	pthread_mutex_unlock(&philo->data->death_mutex);
	return (stop);
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
	ft_print_state(philo->data, philo->id,"is thinking");
	pthread_mutex_lock(philo->left_fork);
	ft_print_state(philo->data, philo->id,"has taken a fork");
	ft_usleep(philo->data->time_to_die, philo->data);
	ft_print_state(philo->data, philo->id,"died");
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
			printf("%lu %d died\n", ft_get_time() - data->time_start, data->philos[i].id);
			pthread_mutex_unlock(&data->print_mutex);
			return 1;
		}
		if (data->num_times_to_eat != -1 && data->philos[i].meals_count < data->num_times_to_eat)
			*all_ate = 0;
		pthread_mutex_unlock(&data->meal_mutex);
		i++;
	}
	usleep(100);
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
	}
	return NULL;
}

void	init_threads(t_data *data, t_philo *philo, pthread_t *monitor)
{
	int	i;
	
	i = 0;
	data->time_start = ft_get_time();
	while (i < data->num_of_philos)
	{
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

int	check_mutex_fail(t_data *data)
{
	if (pthread_mutex_init(&(data->death_mutex), NULL)
		|| pthread_mutex_init(&(data->print_mutex), NULL)
		|| pthread_mutex_init(&(data->meal_mutex), NULL))
		return (1);
	return (0);
}

int	create_data_mutexs(t_data *data)
{
	int	i;

	data->forks = malloc(data->num_of_philos * sizeof(pthread_mutex_t));
	if (!data->forks)
		return (0);
	i = 0;
	while (i < data->num_of_philos)
	{
		if (pthread_mutex_init(&(data->forks[i]), NULL))
		{
			while (--i >= 0)
				pthread_mutex_destroy(&(data->forks[i]));
			free(data->forks);
			return (0);
		}
		i++;
	}
	if (check_mutex_fail(data))
	{
		while (--i >= 0)
			pthread_mutex_destroy(&(data->forks[i]));
		free(data->forks);
		return (0);
	}
	return (1);
}

void	destroy_data_mutexs(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&(data->forks[i]));
		i++;
	}
	pthread_mutex_destroy(&(data->death_mutex));
	pthread_mutex_destroy(&(data->print_mutex));
	pthread_mutex_destroy(&(data->meal_mutex));
	free(data->forks);
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

int main(int ac, char **av)
{
	int			i;
	t_data		data;
	t_philo		*philo;
	pthread_t	monitor;

	if(!ft_check_args(ac, av))
		return (1);
	init_data(&data, ac, av);
	if (!create_data_mutexs(&data))
		return (1);
	if (data.num_times_to_eat == 0)
	{
		destroy_data_mutexs(&data);
		return (0);
	}
	if (!init_philo(&philo, &data))
	{
		destroy_data_mutexs(&data);
		return (1);
	}
	data.philos = philo;
	if (!init_thread(philo, &data))
	{
		destroy_data_mutexs(&data);
		free(philo);
		return (1);
	}
	destroy_data_mutexs(&data);
	free(philo);
	return 0;
}
