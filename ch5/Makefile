CC=gcc
CFLAGS= -pthread -std=c99 -Werror

OBJS = a.o

all: p2

p2: $(OBJS)
	$(CC) $(CFLAGS) -o p2 $(OBJS)

a.o: project2.c

clean:
	rm -f *~ *.o p2
