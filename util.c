#include "util.h"

// Formatea el disco virtual. Guarda el mapa de bits del super bloque 
// y el directorio único.

int myMkfs(MiSistemaDeFicheros* miSistemaDeFicheros, int tamDisco, char* nombreArchivo) {
	// Creamos el disco virtual:
	miSistemaDeFicheros->discoVirtual = open(nombreArchivo, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	// Algunas comprobaciones mínimas:
	assert(sizeof (EstructuraSuperBloque) <= TAM_BLOQUE_BYTES);
	assert(sizeof (EstructuraDirectorio) <= TAM_BLOQUE_BYTES);
	int numBloques = tamDisco / TAM_BLOQUE_BYTES;
	int minNumBloques = 3 + MAX_BLOQUES_CON_NODOSI + 1;
	int maxNumBloques = NUM_BITS;
	if (numBloques < minNumBloques) {
		return 1;
	}
	if (numBloques >= maxNumBloques) {
		return 2;
	}

	/// MAPA DE BITS
	// Inicializamos el mapa de bits
	int i;
	for (i = 0; i < NUM_BITS; i++) {
		miSistemaDeFicheros->mapaDeBits[i] = 0;
	}

	// Los primeros tres bloques tendrán el superbloque, mapa de bits y directorio
	miSistemaDeFicheros->mapaDeBits[MAPA_BITS_IDX] = 1;
	miSistemaDeFicheros->mapaDeBits[SUPERBLOQUE_IDX] = 1;
	miSistemaDeFicheros->mapaDeBits[DIRECTORIO_IDX] = 1;
	// Los siguientes NUM_INODE_BLOCKS contendrán nodos-i
	for (i = 3; i < 3 + MAX_BLOQUES_CON_NODOSI; i++) {
		miSistemaDeFicheros->mapaDeBits[i] = 1;
	}
	escribeMapaDeBits(miSistemaDeFicheros);

	/// DIRECTORIO
	// Inicializamos el directorio (numArchivos, archivos[i].libre) y lo escribimos en disco
	// ...

	/// NODOS-I
	EstructuraNodoI nodoActual;
	nodoActual.libre = 1;
	// Escribimos nodoActual MAX_NODOSI veces en disco
	// ...

	/// SUPERBLOQUE
	// Inicializamos el superbloque (ver common.c) y lo escribimos en disco
	// ...
	sync();

	// Al finalizar tenemos al menos un bloque
	assert(myQuota(miSistemaDeFicheros) >= 1);

	printf("SF: %s, %d B (%d B/bloque), %d bloques\n", nombreArchivo, tamDisco, TAM_BLOQUE_BYTES, numBloques);
	printf("1 bloque para SUPERBLOQUE (%lu B)\n", sizeof(EstructuraSuperBloque));
	printf("1 bloque para MAPA DE BITS, que cubre %lu bloques, %lu B\n", NUM_BITS, NUM_BITS * TAM_BLOQUE_BYTES);
	printf("1 bloque para DIRECTORIO (%lu B)\n", sizeof(EstructuraDirectorio));
	printf("%d bloques para nodos-i (a %lu B/nodo-i, %lu nodos-i)\n",MAX_BLOQUES_CON_NODOSI,sizeof(EstructuraNodoI),MAX_NODOSI);
	printf("%d bloques para datos (%d B)\n",miSistemaDeFicheros->superBloque.numBloquesLibres,TAM_BLOQUE_BYTES*miSistemaDeFicheros->superBloque.numBloquesLibres);
	printf("¡Formato completado!\n");
	return 0;
}

int myImport(char* nombreArchivoExterno, MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno) {
	int handle = open(nombreArchivoExterno, O_RDONLY);
	if (handle == -1) {
		return 1;
	}

	/// Comprobamos que podemos abrir el archivo a importar
	struct stat stStat;
	if (stat(nombreArchivoExterno, &stStat) != false) {
		return 2;
	}

	/// Comprobamos que hay suficiente espacio
	// stStat.st_size > ...
	// ...

	/// Comprobamos que el tamaño total es suficientemente pequeño para ser almacenado en MAX_BLOCKS_PER_FILE
	// stStat.st_size > ...
	// ...

	/// Comprobamos que la longitud del nombre del archivo es adecuada
	// ...

	/// Comprobamos que el fichero no exista, y localizamos el primer archivo libre disponible
	int i = 0;
	BOOLEAN archivoLibreEncontrado = false;
	int idxArchivoLibre;
	while (i < MAX_ARCHIVOS_POR_DIRECTORIO) {
		// ...
		i++;
	}

	/// Comprobamos que existe un nodo-i libre
	if (miSistemaDeFicheros->numNodosLibres <= 0) {
		return 7;
	}

	/// Comprobamos que todavía cabe un archivo en el directorio (MAX_ARCHIVOS_POR_DIRECTORIO)
	if (miSistemaDeFicheros->directorio.numArchivos >= MAX_ARCHIVOS_POR_DIRECTORIO) {
		return 8;
	}

	/// Actualizamos toda la información:
	/// directorio, mapa de bits, nodo-i, bloques de datos, superbloque ...
	// ...
	int numNodoI;
	if ((numNodoI = buscaNodoLibre(miSistemaDeFicheros)) == -1) {
		return 9;
	}
	// ...
	double numBloques = ceil((float) stStat.st_size / (float) TAM_BLOQUE_BYTES);
	// ...

	sync();
	close(handle);
	return 0;
}

int myExport(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivoInterno, char* nombreArchivoExterno) {
	// Buscamos el archivo "nombreArchivoInterno"
	int i = 0;
	while (i < MAX_ARCHIVOS_POR_DIRECTORIO) {
		// ...
		i++;
	}
	if(i>=MAX_ARCHIVOS_POR_DIRECTORIO) {
		return 1;
	}

	int numNodoActual = miSistemaDeFicheros->directorio.archivos[i].idxNodoI;

	// Preguntamos si sobreescribir:
	int handle;
	int archivoExiste;
	char sobreEscribir;
	if ((handle = open(nombreArchivoExterno, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
		archivoExiste = errno;
		if (archivoExiste == EEXIST) {
			printf("El archivo \"%s\" ya existe, ¿sobrescribo? (S/N): ", nombreArchivoExterno);
			sobreEscribir = tolower(getchar());
			if (sobreEscribir == 'n')
				return 0;
		}
		errno = archivoExiste;
		perror("open(...,O_CREAT|O_WRONLY,...)");
	}

	// Copiamos bloque a bloque del archivo interno al externo:
	EstructuraNodoI* temp = miSistemaDeFicheros->nodosI[numNodoActual];
	char buffer[TAM_BLOQUE_BYTES];

	for (i = 0; i < temp->numBloques - 1; i++) {
		// ...
	}

	// Calculamos los bytes restantes y los escribimos:
	int bytesRestantes = 0; // -> Incorrecto
	// int bytesRestantes = ... -> Poner el cálculo correcto;
	lseek(miSistemaDeFicheros->discoVirtual, temp->idxBloques[i] * TAM_BLOQUE_BYTES, SEEK_SET);
	read(miSistemaDeFicheros->discoVirtual, buffer, bytesRestantes);
	write(handle, buffer, bytesRestantes);
	if (close(handle) == -1) {
		return 2;
	}
	return 0;
}

int myRm(MiSistemaDeFicheros* miSistemaDeFicheros, char* nombreArchivo) {
	/// Completar:
	// Busca el archivo con nombre "nombreArchivo"
	// Obtiene el nodo-i asociado y lo actualiza
	// Actualiza el superbloque (numBloquesLibres) y el mapa de bits
	// Libera el puntero y lo hace NULL
	// Actualiza el archivo
	// Finalmente, actualiza en disco el directorio, nodoi, mapa de bits y superbloque
	// ...
	return 0;
}

void myLs(MiSistemaDeFicheros* miSistemaDeFicheros) {
	struct tm* localTime;
	int numArchivosEncontrados = 0;
	EstructuraNodoI nodoActual;
	int i;
	// Recorre el sistema de ficheros, listando los archivos encontrados
	for (i = 0; i < MAX_ARCHIVOS_POR_DIRECTORIO; i++) {
		// ...
	}

	if (numArchivosEncontrados == 0) {
		printf("Directorio vacío\n");
	}
}

void myExit(MiSistemaDeFicheros* miSistemaDeFicheros) {
	int i;
    close(miSistemaDeFicheros->discoVirtual);
    for(i=0; i<MAX_NODOSI; i++) {
    	free(miSistemaDeFicheros->nodosI[i]);
    	miSistemaDeFicheros->nodosI[i] = NULL;
    }
    exit(1);
}
