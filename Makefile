CC=g++
OPTS=-g -O2 -Werror -pthread

all: cachetime.o
	$(CC) $(OPTS) -lm -o cachetime cachetime.o

cachetime.o: cachetime.h cachetime.c
	$(CC) $(OPTS) -c cachetime.c

clean:
	rm -r *.o cachetime;
