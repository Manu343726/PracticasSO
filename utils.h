/* 
 * File:   utils.h
 * Author: manu343726
 *
 * Created on 6 de octubre de 2013, 22:26
 */

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
* Removes the beggining slash of a file.
*
* @param string [in] Pointer (Reference) to the string
*/
void remove_slash(char* string);


/**
* Reads the next header form the specified tar file.
*
* @param file handler of the tar file.
*
* @return Returns the location of the header if it was readed successfully. Returns NULL otherwise.
*/
stHeader* read_header(FILE* file);


#endif	/* UTILS_H */

