CC=gcc
CFLAGS=-Wall

fileio: fileio.o
	$(CC) -o fileio fileio.o $(CFLAGS)

fileio.o: fileio.c
	$(CC) -c fileio.c $(CFLAGS)

diskcontroller: diskcontroller.o
	$(CC) -o diskcontroller diskcontroller.o $(CFLAGS)

diskcontroller.o: diskcontroller.c
	$(CC) -c diskcontroller.c $(CFLAGS)

clean:
	rm diskcontroller.o diskcontroller fileio fileio.o

deepclean:
	make clean
	rm vdisk
