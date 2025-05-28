/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:31:26 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/28 22:17:46 by ahouass          ###   ########.fr       */
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <limits.h>

typedef struct s_data
{
	int             num_of_philos;
	size_t          time_to_die;
	size_t          time_to_eat;
	size_t          time_to_sleep;
	size_t          num_times_to_eat;
	pthread_mutex_t *forks;
	pthread_mutex_t print_mutex;
	unsigned long   time_start;
	int             death;
	pthread_mutex_t death_mutex;
	pthread_mutex_t meal_mutex;
	struct s_philo  *philos;
	int             all_ate;
} t_data;

typedef struct s_philo
{
	int             id;
	int             meals_count;
	unsigned long   last_meal_time;
	pthread_t       thread;
	pthread_mutex_t *left_fork;
	pthread_mutex_t *right_fork;
	t_data			*data;
} t_philo;

#endif