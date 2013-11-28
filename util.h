#ifndef UTIL_H
#define	UTIL_H

#include <errno.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

// Formatea el disco virtual. Guarda el mapa de bits del super bloque 
// y el directorio único.
int myMkfs(MiSistemaDeFicheros* miSistemaDeFicheros, int tamDisco, char* nombreArchivo);

// Importa el fichero externo nombreArchivoExterno en nuestro sistema de ficheros,
// con el nombre nombreArchivoInterno
int myImport(char* nombreArchivoExterno, MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno);

// Exporta el fichero interno nombreArchivoInterno al sistema de ficheros del PC, con el
// nombre nombreArchivoExterno
int myExport(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, char* nombreArchivoExterno);

// Borra el fichero de nombre nombreArchivo
int myRm(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivo);

// Itera sobre los ficheros del directorio y muestra sus nombres (CON PARTE EXTRA)
void myLs(MiSistemaDeFicheros* miSistemaDeFicheros);

// Libera memoria y cierra el sistema de ficheros
void myExit(MiSistemaDeFicheros* miSistemaDeFicheros);

//////////////////////////////////////////////////////////////////////////////////////////
//////////// PARTE EXTRA. PROTOTIPO DE FUNCIONES QUE DEBEN IMPLEMENTARSE//////////
// lseek
int myLseek(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, int offset);

// read
int myRead(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, int nbytes);

//write
int myWrite(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, char* string);


#endif	/* UTIL_H */

