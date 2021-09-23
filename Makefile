CC=gcc
CFLAGS=-std=c99 -ggdb -O0
DEBUG_CFLAGS=-ggdb -O0

ws: main.o encoding.o
	$(CC) $(CFLAGS) -o ws main.o encoding.o

debug: main.o encoding.o
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o ws_debug main.o encoding.o

clean:
	rm *.o ws*

