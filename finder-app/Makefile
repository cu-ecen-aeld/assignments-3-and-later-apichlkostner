
CC = $(CROSS_COMPILE)gcc
CFLAGS=
LDFLAGS=

OBJ = writer.o

writer: $(OBJ)
	$(CC) $(CFLAGS) -o writer $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm writer $(OBJ)