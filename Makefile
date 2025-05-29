NAME = philo

CC = cc
CFLAGS = -Wall -Werror -Wextra

SRC = philo.c inits.c utils.c simulation.c actions.c mutexes.c routine.c threads.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)


%.o: %.c philo.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re