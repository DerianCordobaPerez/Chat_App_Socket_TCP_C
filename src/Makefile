CC=gcc
CFLAGS=-O3 -Wall
LIBS=-fsanitize=address -pthread

main: main.c
	$(CC) -o chat main.c $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f chat