/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:31:26 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/20 11:31:01 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <limits.h>

typedef struct s_data
{
	int				num_of_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	long			num_times_to_eat;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	unsigned long	time_start;
	int				death;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	meal_mutex;
	struct s_philo	*philos;
	int				all_ate;
}	t_data;

typedef struct s_philo
{
	int				id;
	int				meals_count;
	long			last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_data			*data;
}	t_philo;

void	init_data(t_data *data, int ac, char **av);
int		init_philo(t_philo **philo, t_data *data);
void	init_threads(t_data *data, t_philo *philo, pthread_t *monitor);
int		init_mutexes(t_data *data, t_philo *philo, pthread_t *monitor);

int		is_digit(char c);
int		is_number(char *av);
long	ft_atoi(const char *str);
int		ft_check_args(int ac, char **av);

void	*check_simulation(void *arg);
void	ft_usleep(long time, t_data *data);
void	philo_sleep_and_think(t_philo *philo);
int		check_all_ate(t_data *data, int all_ate);
int		check_philosopher_status(t_data *data, int *all_ate);

void	*philo_life(void *arg);
void	*lonely_life(void *arg);
void	philo_eat(t_philo *philo);
void	ft_print_state(t_philo *philo, char *str);
int		handle_single_philosopher(t_data *data, t_philo *philo);

long	ft_get_time(void);
void	destroy_mutexes(t_data *data);

#endif