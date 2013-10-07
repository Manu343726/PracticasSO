#include <string.h> //string functions
#include <stdio.h>  //streams
#include <stdlib.h> //malloc and friends
#include <unistd.h> //POSIX API wrappers (Are they using it? mmmm no...)
#include "mitar.h"

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
 * @param [out] header Pointer to the header of the tar 
 * @param [out] nFiles Number of files contained in the tar
 * @return EXIT_SUCCESS if the header was successfully readed. EXIT_FAILURE in other case.
 */
int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles);


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
    strcpy(header[i].name, fileNames[i]);

    //Copiamos el fichero
    header[i].size=copynFile(inputFile,tarFile,INT_MAX);
    fclose(inputFile);
  }

  //Escribimos el n�mero de ficheros junto a la cabecera
  rewind(tarFile);
  fwrite(&nFiles,sizeof(int), 1,  tarFile);
  fwrite(header,sizeof(stHeaderEntry), nFiles,  tarFile);
    
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

int readHeader(FILE *tarFile, stHeaderEntry **header, int *nFiles){

  //Numero de ficheros
  fread(nFiles,sizeof(int), 1, tarFile);
  
  //Cabecera del tar
  if((*header=(stHeaderEntry *)malloc(sizeof(stHeaderEntry)*(*nFiles)))==NULL) {
    perror("Error al reservar memoria para la cabecera del fichero mtar");
    fclose(tarFile);
    return(EXIT_FAILURE);
  }
  
  //Leemos la cabecera
  fread(*header,sizeof(stHeaderEntry), *nFiles,  tarFile);

  return(EXIT_SUCCESS);
}
