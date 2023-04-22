
CC = $(CROSS_COMPILE)gcc
CFLAGS=-pedantic -Wall -Wextra -Werror -O2
LDFLAGS=

NAME_EXE=aesdsocket
OBJ = main.o server.o signalhandler.o

all: $(NAME_EXE)

$(NAME_EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME_EXE) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(NAME_EXE) $(OBJ)