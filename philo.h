/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:31:26 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/29 16:30:08 by ahouass          ###   ########.fr       */
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
	unsigned long	time_to_die;
	unsigned long	time_to_eat;
	unsigned long	time_to_sleep;
	int				num_times_to_eat;
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
	unsigned long	last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_data			*data;
}	t_philo;

long	ft_atoi(const char *str);
int		is_digit(char c);
int		is_number(char *av);
int		ft_check_args(int ac, char **av);

void	init_data(t_data *data, int ac, char **av);
int		init_philo(t_philo **philo, t_data *data);
void	init_threads(t_data *data, t_philo *philo, pthread_t *monitor);

long	ft_get_time(void);
void	ft_print_state(t_data *data, int philo_id, char *state);
int		ft_check_stop(t_philo *philo);
int		death_getter(t_data *data);
void	ft_usleep(size_t time, t_data *data);
void	ft_pick_fork(t_philo *philo);
void	ft_eat(t_philo *philo);
void	ft_release_fork(t_philo *philo);
void	ft_sleep(t_philo *philo);
void	ft_think(t_philo *philo);
void	*philo_life(void *arg);
void	*lonely_life(void *arg);
int		check_all_ate(t_data *data, int all_ate);
int		check_philosopher_status(t_data *data, int *all_ate);
void	*check_simulation(void *arg);
int		check_mutex_fail(t_data *data);
int		create_data_mutexs(t_data *data);
void	destroy_data_mutexs(t_data *data);
int		ft_stop_simulation(t_data *data);
int		ft_check_death(t_philo *philo, t_data *data);
int		ft_check_meals(t_data *data);
void	*ft_simulation(void *arg);
int		create_philo_threads(t_philo *philo, t_data *data);
int		join_threads(t_philo *philo, t_data *data);
int		init_thread(t_philo *philo, t_data *data);
int		ft_check_optional_parameter(int ac, char **av);

#endif