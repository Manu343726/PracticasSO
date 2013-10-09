#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
       
#include "mitar.h"
       
char uso[]="Uso: tar -c|x|t -f archivo_mtar [fich1 fich2 ...]\n";

int main(int argc, char *argv[]) {

  int opt, nExtra, retCode=EXIT_SUCCESS;
  flags flag=NONE;
  char *tarName=NULL;
  //FILE *tarFile, **inputFiles;
  //headerEntry *header;
  
  //Menor n�mero de argumentos m�nimo v�lido=3: mitar -tfFile.tar
  if(argc < 2){
    fprintf(stderr, uso);
    exit(EXIT_FAILURE);
  }
  //Parseamos los flags DIOOOS con lo bonito que es Boost::program_options ODIO C
  while((opt = getopt(argc, argv, "cxtf:")) != -1) {
    switch(opt) {
      case 'c':
        flag=(flag==NONE)?CREATE:ERROR;
        break;
      case 'x':
        flag=(flag==NONE)?EXTRACT:ERROR;
        break;
      case 'f':
        tarName = optarg;
        break;
      default:
        flag=ERROR;
    }
    //�Hemos detectado un fla v�lido?
    if(flag==ERROR){
      fprintf(stderr, uso);
      exit(EXIT_FAILURE);
    }
  }
  
  //Flag valido + argumento + fichero[s]
  if(flag==NONE || tarName==NULL) {
    fprintf(stderr, uso);
    exit(EXIT_FAILURE);
  }
  
  //#argumentos extra
  nExtra=argc-optind;
  
  //Ejecutamos la accion requerida
  switch(flag) {
    case CREATE:
      retCode=createTar(nExtra, &argv[optind], tarName);
      break;
    case EXTRACT:
      if(nExtra!=0){
        fprintf(stderr, uso);
        exit(EXIT_FAILURE);
      }
      retCode=extractTar(tarName);
      break;
    default:
      retCode=EXIT_FAILURE;
  }
  exit(retCode);
  
}
