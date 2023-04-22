
CC = $(CROSS_COMPILE)gcc
CFLAGS=-pedantic -Wall -Wextra -Werror -Og -g
LDFLAGS=

NAME_EXE=aesdsocket
OBJ = daemon.o main.o readline.o server.o signalhandler.o vector.o

all: $(NAME_EXE)

$(NAME_EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME_EXE) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(NAME_EXE) $(OBJ)