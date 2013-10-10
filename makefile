CFLAGS = -Wall -g

all : mitar.o rut_mitar.o mitar.h utils.h utils.o
	gcc $(CFLAGS) -o mitar mitar.o rut_mitar.o utils.o

clean:
	-rm *.o mitar
