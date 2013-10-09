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
stHeaderEntry* readHeader(FILE *tarFile, int *nFiles);


int createTar(int nFiles, char *fileNames[], char tarName[]) {
  
  int i;
  FILE *tarFile, *inputFile;
  stHeaderEntry *header;
  
  //fichero[s]
  if(nFiles<=0) {
    fprintf(stderr, uso);
    return(EXIT_FAILURE);
  }

  //Abrimos el fichero destino
  if( !(tarFile=fopen(tarName, "wx")) ) {
    fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", tarName);
    perror(NULL);
    return(EXIT_FAILURE);
  }

  //Reservamos memoria para la estructura cabecera del mtar
  if( !(header=malloc(sizeof(stHeaderEntry)*nFiles)) ) {
    perror("Error al reservar memoria para la cabecera del fichero mtar");
    fclose(tarFile);
    remove(tarName);
    return(EXIT_FAILURE);
  }
  
  //Nos posicionamos enla zona de datos del fichero mtar
  fseek(tarFile, sizeof(int)+sizeof(stHeaderEntry)*nFiles, SEEK_SET);
  
  
  //Relleno la cabecera en RAM y copio los datos de los ficheros en el tar
  for(i=0; i<nFiles; i++) {
	
	/* PARTE OPCIONAL 1 AQUI!!!!!!!!! */
	remove_slash(fileNames + i);
	
    //Abrimos fichero fuente
    if((inputFile=fopen(fileNames[i], "r"))==NULL) {
      fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", fileNames[i]);
      perror(NULL);
      free(header);
      fclose(tarFile);
      remove(tarName);
      return(EXIT_FAILURE);
    }
    //Rellenamos la cabecera
    strcpy(header[i].name, fileNames[i]); //OK

    //Copiamos el fichero
    header[i].size=copynFile(inputFile,tarFile,INT_MAX);
    fclose(inputFile);
  }

  //Escribimos el n�mero de ficheros junto a la cabecera
  rewind(tarFile);
  fwrite(&nFiles,sizeof(int), 1,  tarFile);
  //fwrite(header,sizeof(stHeaderEntry), nFiles,  tarFile); <--- no lo uso
    
  fprintf(stdout, "Fichero mitar creado con exito\n");

  free(header);
  fclose(tarFile);
  
  return(EXIT_SUCCESS);
}

int extractTar(char tarName[]) {
  stHeaderEntry *header;
  int nFiles,i;
  FILE *tarFile,*outputFile;
  
  //Abrimos el fichero destino
  if((tarFile=fopen(tarName, "r"))==NULL) {
    fprintf(stderr, "No se ha podido abrir el fichero tar %s: \n", tarName);
    perror(NULL);
    return(EXIT_FAILURE);
  }
  
  //Leemos la cabecera del mtar
  if(readHeader(tarFile, &header, &nFiles)==EXIT_FAILURE){
    fclose(tarFile);
    return(EXIT_FAILURE);
  }
  
  //Creamos los ficheros contenidos en el mtar
  for(i=0; i<nFiles; i++){
    if((outputFile=fopen(header[i].name, "wx"))==NULL) {
      fprintf(stderr, "No se ha podido crear el fichero %s: \n", header[i].name);
      perror(NULL);
      free(header);
      fclose(tarFile);
      return(EXIT_FAILURE);
    }
    
    printf("[%i]: Creando fichero %s, tama�o %i Bytes...", i, header[i].name, header[i].size);
    
    //Copiamos el fichero
    if(copynFile(tarFile,outputFile,header[i].size)!=header[i].size){
      fprintf(stderr, "No se ha podido copiar el fichero %s: \n", header[i].name);
      free(header);
      fclose(tarFile);
      fclose(outputFile);
      remove(header[i].name);
      return(EXIT_FAILURE);
    }
    
    fclose(outputFile);
    printf("Ok\n");
    
  }
  
  free(header);
  fclose(tarFile);
  return(EXIT_SUCCESS);
}
  

int copynFile(FILE *origen, FILE *destimo, int nBytes){
  int nCopy=0;
  char c;
  while( nCopy<nBytes && (c=getc(origen))!=EOF ){
    putc(c, destimo);
    nCopy++;
  }
  return(nCopy);
}

stHeaderEntry* readHeader(FILE *tarFile, int *nFiles){
	stHeaderEntry* header;
	int i;
	
  //Numero de ficheros
  fread(nFiles,sizeof(int), 1, tarFile);
  
  //Cabecera del tar
  if((*header=(stHeaderEntry *)malloc(sizeof(stHeaderEntry)*(*nFiles)))==NULL) {
    perror("Error al reservar memoria para la cabecera del fichero mtar");
    fclose(tarFile);
    return(NULL);
  }

	for(i = 0 ; i < *nFiles ; ++i)
	{
		/* header layout at tar file:
	
		+---------------------------------------------+
		|  name_length |  name char array | file_size |
		+---------------------------------------------+
		                  ^^^^^^^^^^^^^^^
	 				 		  with \0
			               

		*/

		fread(&(header[i].name_length) , sizeof(size_t)       , 1 , file);

		if(!(header[i].name = (char*)malloc(sizeof(char) * header[i].name_length)))
			perror("Error al reservar memoria para el nombre de la cabecera");
			fclose(tarFile);
			return(NULL);
	  	}

		fread(&(header[i].name)        , sizeof(char)         , name_header[i].name_length , file);
		fread(&(header[i].file_size)   , sizeof(size_t)       , 1 , file);
	}

  return header;
}
