CC=gcc
CFLAGS=-Wall

kapish: kapish.o
	$(CC) -o kapish kapish.o $(CFLAGS)

kapish.o: kapish.c
	$(CC) -c kapish.c $(CFLAGS)

clean:
	rm kapish.o kapish

