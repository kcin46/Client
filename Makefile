CC = gcc
PROGS = p1

p1: p1.o
	gcc -o p1 p1.o

p1.o: p1.c
	gcc -c p1.c

clean:
	rm -f *.o $(PROGS)
