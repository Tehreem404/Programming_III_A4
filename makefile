CFLAGS = -Wall -pedantic -std=c99 -DLIST
CC = gcc

all: ttest

ttest: ttest.o tree.c tree.h
	$(CC) $(CFLAGS) ttest.o tree.c -o ttest

ttest.o: ttest.c
	$(CC) $(CFLAGS) -c ttest.c -o ttest.o

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -c tree.c -o tree.o

clean:
	rm *.o -i ttest