
CC = $(CROSS_COMPILE)gcc
CFLAGS=
LDFLAGS=

OBJ = writer.o

all: writer

writer: $(OBJ)
	$(CC) $(CFLAGS) -o writer $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f writer $(OBJ)