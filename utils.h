/* 
 * File:   utils.h
 * Author: manu343726
 *
 * Created on 6 de octubre de 2013, 22:26
 */

#include <stdio.h>

#include "mitar.h"

#ifndef UTILS_H
#define	UTILS_H

/**
 * Copies the data from one range to the specified location
 * 
 * @details A high-level version of memcpy (I'm not sure if i'm allowed to use memcpy). It could be viewed as a low-level version of C++ std::copy
 * 
 * @param first Pointer pointing to the begin of the source range
 * @param last Pointer pointing to the end of the source range
 * @param d_first Pointer pointing to the begin of the destination range.
 */
void copy(void* first , void* last , void* d_first);

/**
* Copies the specified string into a new dynamically-allocated string.
* 
* @param string A pointer to a c-style string.
* 
* @returns A pointer to the cloned string. NULL if an error ocurred during the allocation.
*/
char* clone_str(char* string);

/**
* Removes the beggining slash of a file name.
*
* @param string [in] Pointer (Reference) to the string which contains the file name.
*/
void remove_slash(char* string);


/**
* Reads the next header form the specified tar file.
*
* @param file handler of the tar file.
*
* @return Returns the location of the header if it was readed successfully. Returns NULL otherwise.
*/
stHeaderEntry* read_header(FILE* file);

/**
 * Frees the memory used by a tar file header.
 * @param tar_header Pointer to the tar file header (dynamic-array of file headers). 
 * @param nFiles Number of files of the tar file.
 */
void free_header(stHeaderEntry* tar_header , int nFiles);

/**
 * Frees the memory used by the range [begin,end) of tar file header entries.
 * 
 * @details This function is dessigned to avoid memory-leaks in the execution of the "read_tar_header()" function and others.
 *          In that functions, first a chunk of memory is reserved to allocate the set of file headers, and later that set of 
 *          headers is trasversed to write the appropiate data. That writting includes a memory allocation for each file name
 *          (Calls to "clone_str()", and explicit malloc calls before fread(header[i].name)). 
 *          If that trasverse fails, we need to deallocate the set of strings we allocated previously, and the complete chunk
 *          of memory which holds the set of headers.
 * 
 * @param tar_header Pointer to the dynamically-allocated set of file headers (Tar file header).
 * @param begin Index of the first header entry to be freed. 
 * @param end Index of the end of the range (Note tha range specified [begin,end) is not end-inclusive).
 */
void free_header_range(stHeaderEntry* tar_header , size_t begin , size_t end);


#endif	/* UTILS_H */

