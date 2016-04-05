CC = gcc -Wall
CFLAGS = -c
OBJS = misc.o interc.o sockets.o cliente.o
cliente: $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c defs.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f cliente ./*.o 

