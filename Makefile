CC=gcc
CFLAGS=-O3

ws: main.o encoding.o
	$(CC) $(CFLAGS) -o ws main.o encoding.o

clean:
	rm *.o ws

