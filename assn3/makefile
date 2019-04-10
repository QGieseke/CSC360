CC=gcc
CFLAGS=-Wall

#fileio: fileio.o
#	$(CC) -o fileio fileio.o $(CFLAGS)

#fileio.o: fileio.c
#	$(CC) -c fileio.c $(CFLAGS)

file: file.o
	$(CC) -o file file.o $(CFLAGS)

file.o: file.c
	$(CC) -c file.c $(CFLAGS)

diskcontroller: diskcontroller.o
	$(CC) -o diskcontroller diskcontroller.o $(CFLAGS)

diskcontroller.o: diskcontroller.c
	$(CC) -c diskcontroller.c $(CFLAGS)

clean:
	rm diskcontroller.o diskcontroller fileio fileio.o

deepclean:
	make clean
	rm vdisk
