#include <string.h> //string functions
#include <stdio.h>  //streams
#include <stdlib.h> //malloc and friends
#include <unistd.h> //POSIX API wrappers (Are they using it? mmmm no...)

#include "mitar.h"

#include "utils.h"

extern char *uso;

/**
 * Copies the specified amount of bytes from one file into another file.
 * @param origen input buffer
 * @param destimo output buffer
 * @param nBytes Number of bytes to be written
 * @return Number of bytes correctly written at the second buffer.
 */
int copynFile(FILE *origen, FILE *destimo, int nBytes);

/**
 * Reads the header of the specified tar file
 * @param [in]  tarFile Handler of the tar file (Technically is IN/OUT because the state stored at FILE struct is modified by the read ops, but from the user
 *                      point of view is IN)
 * @param [out] nFiles Number of files contained in the tar
 * @return The memory location of the header if it was readed successfully. NULL in other case.
 */
stHeaderEntry* read_tar_header(FILE *tarFile, int *nFiles);

/**
 * Writes the tar header into the specified tar file.
 * @param file tar file handler.
 * @param headers tar file header as an array of file headers.
 * @param n_files Number of diles to be "compressed" (Number of file headers).
 */
void write_tar_header(FILE* file , stHeaderEntry* headers , int n_files);

/*****************************************************************************************************************************************************
 *  TAR FILE LAYOUT:                                                                                                                                 *
 *  ===============                                                                                                                                  *
 *                                                                                                                                                   *
 *                                                                                                                                                   *
 *       sizeof(int)     header1 size                                                        headern_size                                            *
 *   <---------------> <-------------> <------------------------ ... --------------------> <------------->                                           *
 *                                                                                                                                                   *
 *  +-----------------+---------------+---------------------------------------------------+---------------+----------------+-----+----------------+  *
 *  | number_of_files | file_1_header |                          ...                      | file_n_header | file_1_content | ... | file_n_content |  *
 *  +-----------------+---------------+---------------------------------------------------+---------------+----------------+-----+----------------+  *
 *                                                                                                                                                   *
 *                     <--------------------------------------------------------------------------------->                                           *
 *                        Σ( sizeof(size_t) + sizeof(size_t) + (sizeof(char) * header[i].name_langth) )                                              *
 *                                                                                                                                                   *
 *                                                                                                                                                   *
 *  FILE HEADER LAYOUT:                                                                                                                              *
 *  ==================                                                                                                                               *
 *                                                                                                                                                   *
 *                                                                                                                                                   *
 *     sizeof(size_t)     sizeof(char) * name_length     sizeof(size_t)                                                                              *
 *  <-----------------> <----------------------------> <---------------->                                                                            *
 *                                                                                                                                                   *
 * +-------------------+------------------------------+------------------+                                                                           *
 * |    name_length    |         file_name            |     file_size    |                                                                           *
 * +-------------------+------------------------------+------------------+                                                                           *
 *                                                                                                                                                   *
 * NOTE: file_name includes \0 (name_langth is the length of the array, not the length of the string).                                               *                                           *
 *                                                                                                                                                   *
 ****************************************************************************************************************************************************/


int createTar(int nFiles, char *fileNames[], char tarName[]) {//LEAK_FREE
    int i;
    size_t header_offset;
    FILE *tarFile, *inputFile;
    stHeaderEntry *header;

    //fichero[s]
    if (nFiles <= 0) {
        fprintf(stderr, uso);
        return (EXIT_FAILURE);
    }

    //Abrimos el fichero destino
    if (!(tarFile = fopen(tarName, "w"))) { //He cambiado el modo de apertura para no tener que borrar el archivo cada vez que ejecuto
        fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", tarName);
        perror(NULL);
        return (EXIT_FAILURE);
    }

    //Reservamos memoria para la estructura cabecera del mtar
    if (!(header = malloc(sizeof (stHeaderEntry) * nFiles))) {
        perror("Error al reservar memoria para la cabecera del fichero mtar");
        fclose(tarFile);
        remove(tarName);
        return (EXIT_FAILURE);
    }

    header_offset = sizeof (int); //number_of_files offset (See ascii-art above)

    for (i = 0; i < nFiles; ++i) {
        header_offset += sizeof (size_t) + (sizeof (char) * (strlen(fileNames[i]) + 1)) + sizeof (size_t); //See file header layout above.
    }

    //Nos posicionamos enla zona de datos del fichero mtar
    fseek(tarFile, header_offset, SEEK_SET);


    //Relleno la cabecera en RAM y copio los datos de los ficheros en el tar
    for (i = 0; i < nFiles; i++) {

        /* PARTE OPCIONAL 1 AQUI!!!!!!!!! */
        remove_slash(fileNames[i]);

        //Abrimos fichero fuente
        if ((inputFile = fopen(fileNames[i], "r")) == NULL) {
            fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", fileNames[i]);
            perror(NULL);
            free_header_range(header , 0 , i); //Avoid strings and header leaks (See documentation details).
            fclose(tarFile);
            remove(tarName);
            return (EXIT_FAILURE);
        }
        //Rellenamos la cabecera
        header[i].name = clone_str(fileNames[i]);
        header[i].name_length = strlen(fileNames[i]) + 1;

        //Copiamos el fichero
        header[i].file_size = copynFile(inputFile, tarFile, INT_MAX);
        fclose(inputFile);
    }

    //Escribimos el n�mero de ficheros junto a la cabecera
    rewind(tarFile);
    fwrite(&nFiles, sizeof (int), 1, tarFile);

    //Escribimos la cabecera:
    write_tar_header(tarFile, header, nFiles);

    fprintf(stdout, "Fichero mitar creado con exito\n");

    free_header(header , nFiles); //Avoid strings and header leaks (See documentation details).
    fclose(tarFile);

    return (EXIT_SUCCESS);
}

int extractTar(char tarName[]) {//LEAK_FREE
    stHeaderEntry *header;
    int nFiles, i;
    FILE *tarFile, *outputFile;

    //Abrimos el fichero destino
    if ((tarFile = fopen(tarName, "r")) == NULL) {
        fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", tarName);
        perror(NULL);
        return (EXIT_FAILURE);
    }

    //Leemos la cabecera del mtar
    if (!(header = read_tar_header(tarFile, &nFiles))) {
        //Leaking is managed by read_tar_header().
        fclose(tarFile);
        return (EXIT_FAILURE);
    }

    //Creamos los ficheros contenidos en el mtar
    for (i = 0; i < nFiles; i++) {
        if ((outputFile = fopen(header[i].name, "w")) == NULL) {//He cambiado el modo de apertura para no tener que borrar el archivo cada vez que ejecuto
            fprintf(stderr, "No se ha podido crear el fichero %s: \n", header[i].name);
            perror(NULL);
            free_header(header , nFiles); //Avoid strings and header leaks (See documentation details).
            fclose(tarFile);
            return (EXIT_FAILURE);
        }

        printf("[%i]: Creando fichero %s, tama�o %zu Bytes...", i, header[i].name, header[i].file_size);

        //Copiamos el fichero
        if (copynFile(tarFile, outputFile, header[i].file_size) != header[i].file_size) {
            fprintf(stderr, "No se ha podido copiar el fichero %s: \n", header[i].name);
            free_header(header , nFiles); //Avoid strings and header leaks (See documentation details).
            fclose(tarFile);
            fclose(outputFile);
            remove(header[i].name);
            return (EXIT_FAILURE);
        }

        fclose(outputFile);
        printf("Ok\n");

    }

    free_header(header , nFiles); //Avoid strings and header leaks (See documentation details).
    fclose(tarFile);
    return (EXIT_SUCCESS);
}

int copynFile(FILE *origen, FILE *destimo, int nBytes) {
    int nCopy = 0;
    char c;
    while (nCopy < nBytes && (c = getc(origen)) != EOF) {
        putc(c, destimo);
        nCopy++;
    }
    return (nCopy);
}

void write_tar_header(FILE* file, stHeaderEntry* headers, int n_files) { //LEAK_FREE (Caller ownership)
    int i = 0;

    for (i = 0; i < n_files; ++i) {
        /* header layout at tar file:
	
        +---------------------------------------------+
        |  name_length |  name char array | file_size |
        +---------------------------------------------+
                          ^^^^^^^^^^^^^^^
                              with \0
			               

         */

        fwrite(&(headers[i].name_length), sizeof (size_t), 1, file);
        fwrite(headers[i].name, sizeof (char), headers[i].name_length, file);
        fwrite(&(headers[i].file_size), sizeof (size_t), 1, file);
    }
}

stHeaderEntry* read_tar_header(FILE *tarFile, int *nFiles) { //LEAK_FREE
    stHeaderEntry* header;
    int i;

    //Numero de ficheros
    fread(nFiles, sizeof (int), 1, tarFile);

    //Cabecera del tar
    if ((header = (stHeaderEntry *) malloc(sizeof (stHeaderEntry)*(*nFiles))) == NULL) {
        perror("Error al reservar memoria para la cabecera del fichero mtar");
        fclose(tarFile);
        return (NULL);
    }

    for (i = 0; i < *nFiles; ++i) {
        /* header layout at tar file:

        +---------------------------------------------+
        |  name_length |  name char array | file_size |
        +---------------------------------------------+
                          ^^^^^^^^^^^^^^^
                              with \0


         */

        fread(&(header[i].name_length), sizeof (size_t), 1, tarFile);

        if (!(header[i].name = (char*) malloc(sizeof (char) * header[i].name_length))){
            perror("Error al reservar memoria para el nombre de la cabecera");
            free_header_range(header , 0 , i); //Avoid strings and header leaks (See documentation details)
            fclose(tarFile);
            return (NULL);
        }

        fread(header[i].name, sizeof (char), header[i].name_length, tarFile);
        fread(&(header[i].file_size), sizeof (size_t), 1, tarFile);
    }

    return header;
}
