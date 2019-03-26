CC=gcc
CFLAGS=-Wall

diskcontroller: diskcontroller.o
	$(CC) -o diskcontroller diskcontroller.o $(CFLAGS)

diskcontroller.o: diskcontroller.c
	$(CC) -c diskcontroller.c $(CFLAGS)

clean:
	rm diskcontroller.o diskcontroller 

deepclean:
	rm diskcontroller.0 diskcontroller vdisk
