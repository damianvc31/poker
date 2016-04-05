CC = gcc -Wall
CFLAGS = -c
OBJS = misc.o interc.o sockets.o reparto.o calculadora.o ICM.o server.o
server: $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c defs.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f server ./*.o 

