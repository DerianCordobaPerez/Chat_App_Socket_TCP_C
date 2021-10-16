CC=gcc
CFLAGS=-O3 -Wall
LIBS=-pthread
SRC=/

all: main.out

main.out: main.o
	$(CC) $(CFLAGS) $(LIBS) -o main.out main.o

main.o: $(SRC)main.c
	$(CC) $(CFLAGS) -c $(SRC)main.c

.PHONY: clean
clean:
	rm -f *.o *.out