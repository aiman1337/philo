/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahouass <ahouass@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:06:06 by ahouass           #+#    #+#             */
/*   Updated: 2025/05/15 11:57:30 by ahouass          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int money = 0;

typedef struct s_data
{
    int             add;
    pthread_mutex_t mutex;
}   t_data;

void    *add_money(void *arg)
{
    int add = *(int *)arg;

    int i = 0;
    while(i < add)
    {
        pthread_mutex_lock(&data.);
        money++;
        pthread_mutex_unlock(&m1);
        i++;
    }
    return NULL;
}

int main()
{
    t_data          data;
    pthread_t       t1;
    pthread_t       t2;
    pthread_mutex_t m1;

    pthread_mutex_init(&m1, NULL);
    int add = 1000000000;

    data.add = add;
    data.mutex = m1;
    pthread_create(&t1, NULL, &add_money, (void *)&data);
    pthread_create(&t2, NULL, &add_money, (void *)&data);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_mutex_destroy(&m1);

    printf("money %d\n", money);
}