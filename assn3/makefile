CC=gcc
CFLAGS=-std=c99

#fileio: fileio.o
#	$(CC) -o fileio fileio.o $(CFLAGS)

#fileio.o: fileio.c
#	$(CC) -c fileio.c $(CFLAGS)

file: file.o filecontroller.o diskcontroller.o
	$(CC) -o file file.o $(CFLAGS)

file.o: file.c
	$(CC) -c file.c $(CFLAGS)

#filecontroller: filecontroller.o 
#	$(CC) -o filecontroller filecontroller.o $(CFLAGS)

filecontroller.o: filecontroller.c
	$(CC) -c filecontroller.c $(CFLAGS)

#diskcontroller: diskcontroller.o
#	$(CC) -o diskcontroller diskcontroller.o $(CFLAGS)

diskcontroller.o: diskcontroller.c
	$(CC) -c diskcontroller.c $(CFLAGS)

clean:
	rm diskcontroller.o diskcontroller fileio fileio.o file.o file filecontroller.o filecontroller

deepclean:
	make clean
	rm vdisk
