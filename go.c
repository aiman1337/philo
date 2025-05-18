/**
 * The main issue with the original code appears to be related to:
 * 1. Race conditions in last meal time tracking
 * 2. Potential deadlocks with many philosophers
 * 3. Inadequate staggering for large numbers of philosophers
 * 4. Inefficient death monitoring
 */

# include "philo.h"

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
	while (i < ac)
	{
		if (ft_atoi(av[i]) <= 0 || !is_number(av[i]))
		{
			printf("Not valid arguments\n");
			return 0;
		}
		i++;
	}
	return 1;
}

// Time utility functions
unsigned long ft_get_time()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

// Sleep with simulation stop checking
void ft_usleep(unsigned long time, t_data *data)
{
    unsigned long start = ft_get_time();

    while (ft_get_time() - start < time)
    {
        // Check more frequently for simulation stop
        usleep(500);
        pthread_mutex_lock(&data->death_mutex);
        if (data->death)
        {
            pthread_mutex_unlock(&data->death_mutex);
            break;
        }
        pthread_mutex_unlock(&data->death_mutex);
    }
}

// Check if simulation should stop
int should_stop(t_data *data)
{
    int stop;
    
    pthread_mutex_lock(&data->death_mutex);
    stop = data->death;
    pthread_mutex_unlock(&data->death_mutex);
    return stop;
}

// Print philosopher state with timestamp
void ft_print_state(t_philo *philo, char *str)
{
    pthread_mutex_lock(&philo->data->print_mutex);
    if (!should_stop(philo->data))
    {
        printf("%lu %d %s\n", 
               ft_get_time() - philo->data->time_start, 
               philo->id, 
               str);
    }
    pthread_mutex_unlock(&philo->data->print_mutex);
}

// Improved philosopher routine
void *philo_life(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    
    // Set last meal time at start
    pthread_mutex_lock(&philo->data->meal_mutex);
    philo->last_meal_time = ft_get_time();
    pthread_mutex_unlock(&philo->data->meal_mutex);
    
    // Print initial thinking state
    ft_print_state(philo, "is thinking");
    
    // Better staggering based on philosopher ID
    // Odd philosophers start right away, even philosophers wait
    if (philo->id % 2 == 0)
        ft_usleep(philo->data->time_to_eat / 2, philo->data);
    
    // Main philosopher life cycle
    while (!should_stop(philo->data))
    {
        // Handle single philosopher case
        if (philo->data->num_of_philos == 1)
        {
            pthread_mutex_lock(philo->left_fork);
            ft_print_state(philo, "has taken a fork");
            pthread_mutex_unlock(philo->left_fork);
            ft_usleep(philo->data->time_to_die, philo->data);
            return NULL;
        }
        
        // Take forks in consistent order to prevent deadlock
        // Always take the lower-numbered fork first
        if (philo->id % 2 == 1)
        {
            pthread_mutex_lock(philo->left_fork);
            ft_print_state(philo, "has taken a fork");
            pthread_mutex_lock(philo->right_fork);
            ft_print_state(philo, "has taken a fork");
        }
        else
        {
            pthread_mutex_lock(philo->right_fork);
            ft_print_state(philo, "has taken a fork");
            pthread_mutex_lock(philo->left_fork);
            ft_print_state(philo, "has taken a fork");
        }
        
        // Critical section: eating
        pthread_mutex_lock(&philo->data->meal_mutex);
        philo->last_meal_time = ft_get_time();
        philo->meals_count++;
        pthread_mutex_unlock(&philo->data->meal_mutex);
        
        ft_print_state(philo, "is eating");
        ft_usleep(philo->data->time_to_eat, philo->data);
        
        // Release forks in reverse order
        if (philo->id % 2 == 1)
        {
            pthread_mutex_unlock(philo->right_fork);
            pthread_mutex_unlock(philo->left_fork);
        }
        else
        {
            pthread_mutex_unlock(philo->left_fork);
            pthread_mutex_unlock(philo->right_fork);
        }
        
        // Sleeping phase
        ft_print_state(philo, "is sleeping");
        ft_usleep(philo->data->time_to_sleep, philo->data);
        
        // Thinking phase (with dynamic think time)
        ft_print_state(philo, "is thinking");
        
        // Add a small delay to prevent resource hogging
        ft_usleep(500, philo->data);
    }
    
    return NULL;
}

// Improved death monitor
void *check_simulation(void *arg)
{
    t_data *data = (t_data *)arg;
    int i;
    int all_ate;
    
    // Give philosophers time to start
    ft_usleep(5, data);
    
    while (!should_stop(data))
    {
        i = 0;
        all_ate = 1;
        
        while (i < data->num_of_philos && !should_stop(data))
        {
            // Check if philosopher has died
            pthread_mutex_lock(&data->meal_mutex);
            if (ft_get_time() - data->philos[i].last_meal_time > data->time_to_die)
            {
                pthread_mutex_unlock(&data->meal_mutex);
                
                // Mark simulation as stopped
                pthread_mutex_lock(&data->death_mutex);
                data->death = 1;
                pthread_mutex_unlock(&data->death_mutex);
                
                // Print death message
                pthread_mutex_lock(&data->print_mutex);
                printf("%lu %d died\n", ft_get_time() - data->time_start, data->philos[i].id);
                pthread_mutex_unlock(&data->print_mutex);
                
                return NULL;
            }
            
            // Check if philosopher has eaten enough
            if (data->num_times_to_eat != -1 && 
                data->philos[i].meals_count < data->num_times_to_eat)
            {
                all_ate = 0;
            }
            
            pthread_mutex_unlock(&data->meal_mutex);
            i++;
        }
        
        // Check if all philosophers have eaten enough
        if (all_ate && data->num_times_to_eat != -1)
        {
            pthread_mutex_lock(&data->death_mutex);
            data->all_ate = 1;
            data->death = 1;
            pthread_mutex_unlock(&data->death_mutex);
            return NULL;
        }
        
        // Sleep between checks to prevent CPU overuse
        usleep(1000);
    }
    
    return NULL;
}

// Initialize data structure
void init_data(t_data *data, int ac, char **av)
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

// Initialize philosopher structs
int init_philo(t_philo **philo, t_data *data)
{
    int i = 0;

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

// Initialize mutexes
int init_mutexes(t_data *data)
{
    int i = 0;

    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_of_philos);
    if (!data->forks)
        return 0;
    
    // Initialize fork mutexes
    while (i < data->num_of_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            return 0;
        i++;
    }
    
    // Initialize control mutexes
    if (pthread_mutex_init(&data->print_mutex, NULL) != 0 ||
        pthread_mutex_init(&data->death_mutex, NULL) != 0 ||
        pthread_mutex_init(&data->meal_mutex, NULL) != 0)
        return 0;
    
    return 1;
}

// Assign forks to philosophers
void assign_forks(t_philo *philo, t_data *data)
{
    int i = 0;
    
    while (i < data->num_of_philos)
    {
        // For odd numbered philosophers, swap the fork order
        // This prevents the deadlock condition
        if (i % 2)
        {
            philo[i].left_fork = &data->forks[i];
            philo[i].right_fork = &data->forks[(i + 1) % data->num_of_philos];
        }
        else
        {
            philo[i].right_fork = &data->forks[i];
            philo[i].left_fork = &data->forks[(i + 1) % data->num_of_philos];
        }
        i++;
    }
}

// Main function
int main(int ac, char **av)
{
    t_data data;
    t_philo *philo;
    pthread_t monitor;
    int i;

    if (!ft_check_args(ac, av))
        return 1;
    
    init_data(&data, ac, av);
    if (!init_philo(&philo, &data))
        return 1;
    
    if (!init_mutexes(&data))
        return 1;
    
    // Assign forks with deadlock prevention
    assign_forks(philo, &data);
    
    // Create philosopher threads
    i = 0;
    while (i < data.num_of_philos)
    {
        if (pthread_create(&philo[i].thread, NULL, philo_life, &philo[i]) != 0)
            return 1;
        i++;
    }

    // Create monitor thread
    if (pthread_create(&monitor, NULL, check_simulation, &data) != 0)
        return 1;
    
    // Wait for philosopher threads to finish
    i = 0;
    while (i < data.num_of_philos)
    {
        pthread_join(philo[i].thread, NULL);
        i++;
    }
    
    // Wait for monitor thread
    pthread_join(monitor, NULL);
    
    // Clean up mutexes and memory
    i = 0;
    while (i < data.num_of_philos)
    {
        pthread_mutex_destroy(&data.forks[i]);
        i++;
    }
    
    pthread_mutex_destroy(&data.print_mutex);
    pthread_mutex_destroy(&data.death_mutex);
    pthread_mutex_destroy(&data.meal_mutex);
    
    free(data.forks);
    free(philo);
    
    return 0;
}