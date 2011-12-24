all: dog.o
	gcc -lpthread dog.o -o dog

dog.o:
	gcc -c dog.c


