all: MiSistemaDeFicheros

MiSistemaDeFicheros: MiSistemaDeFicheros.o parse.o common.o util.o
	gcc MiSistemaDeFicheros.o parse.o common.o util.o -lm -lreadline -o MiSistemaDeFicheros

MiSistemaDeFicheros.o: MiSistemaDeFicheros.c
	gcc -Wall -g -c MiSistemaDeFicheros.c

parse.o: parse.c
	gcc -Wall -g -c parse.c

common.o: common.c
	gcc -Wall -g -c common.c

util.o: util.c
	gcc -Wall -g -c util.c

clean:
	rm -rf *.o MiSistemaDeFicheros
