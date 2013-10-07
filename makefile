CFLAGS = -Wall -g

all : mitar.o rut_mitar.o mitar.h
	gcc $(CFLAGS) -o mitar mitar.o rut_mitar.o

clean:
	-rm *.o mitar
