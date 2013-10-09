#ifndef _MITAR_H
#define _MITAR_H

#include <limits.h> //PATH_MAX

typedef enum{
  NONE,
  ERROR,
  CREATE,
  EXTRACT,
} flags;

typedef struct {
    char* name;
    size_t name_length;
    size_t file_size;
}   stHeaderEntry;


/*
 * Acabo de leer en el enunciado que se puede usar C++ sin usar OO. Teniendo en cuenta 
 * que C++ no es estrictamente OO (Hoy en día tiende a ser genérico, no hay más que ver
 * la importancia de los algoritmos basados en templates, como el 90% de la librería estándar)
 * podría hacer la práctica entera con un par de 
 * std::copy(std::begin(files) , std::end(files) , std::ostream_iterator(output_file))
 * (No es exactamente así, pero es la idea).
 * 
 * Pero no vamos a tocar los cojones...
 * 
 * 
 * Lo que quiero decir con esto es: No hay que decir a la gente cosas como "puedes usar C, o C++ pero sin clases". C with clases es un
 * concepto de los años ochenta. C++ es muchísimo más que eso. Oir ese tipo de cosas explica la absuluta falta de "buenos programadores" (Dejémoslo en 
 *  "programadores") de C++ en españa.... Lo siento pero una de las cosas que mas aborrezco son estos cinco caracteres "C/C++". C es una cosa, C++ otra muy distintas,
 * no los trates como iguales. Hace bastante tiempo que desistí de esperar que en esta carrera enseñaran a programar a la gente, pero cosas como
 * esas tampoco ayudan... Y si alguien piensa que soy un exagerado o un friki, recuerden que la primera clase de tercero de carrera (Carrera de cuatro años) 
 * consistió en repasar el concepto de puntero, y el 70% de la clase no lo tenía claro.
 */

/**
 * Creates a tar file with the specified files as content.
 * @param nFiles Number of files that will be contained in the tar file.
 * @param fileNames Array of strings containing the names of the input files.
 * @param tarName String containing the name of the tar file.
 * @return EXIT_SUCCESS if the tar was created successfully. EXIT_FAILURE in other case.
 */
int createTar(int nFiles, char *fileNames[], char tarName[]);

/**
 * Extracts the specified tar file
 * @param tarName String containing the name of the tar file
 * @return EXIT_SUCCESS if the tar file was extracted successfylly. EXIT_FAILURE in other case.
 */
int extractTar(char tarName[]);


#endif /* _MITAR_H */
