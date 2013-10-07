/* 
 * File:   utils.h
 * Author: manu343726
 *
 * Created on 6 de octubre de 2013, 22:26
 */

#ifndef UTILS_H
#define	UTILS_H

#ifndef byte
#include <stdint.h>
typedef uint8_t byte;
#endif /* byte */

/**
 * Copies the data from one range to the specified location
 * 
 * @details A high-level version of memcpy (I'm not sure if i'm allowed to use memcpy). It could be viewed as a low-level version of C++ std::copy
 * 
 * @param first Pointer pointing to the begin of the source range
 * @param last Pointer pointing to the end of the source range
 * @param d_first Pointer pointing to the begin of the destination range.
 */
void copy(void* first , void* last , void* d_first)
{
    byte*  ptr   = (byte*)first;
    byte* d_ptr  = (byte*)d_first;
    byte* last_b = (byte*)last;
    
    while(ptr != last_b)
    {
        *d_ptr++ = *ptr++;
    }
}

#endif	/* UTILS_H */

