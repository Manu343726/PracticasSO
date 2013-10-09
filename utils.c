
#include <string.h> //strlen, etc
#include <stdlib.h> //malloc

#include "utils.h"

#ifndef byte
#include <stdint.h>
typedef uint8_t byte;
#endif /* byte */


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

char* clone_str(char* string)
{
	char* str = (char*)malloc(sizeof(char) * (strlen(string) + 1));

	if(!str) return (NULL);

    strcpy(string , str);

	return str;
}

void remove_slash(char** string)
{
	char* str = *string;

	if( str[0] != '\0' && str[0] == '/')
	{
		char* new_str = (char*)malloc(sizeof(char) * strlen(str));

		strcpy(str + 1 , new_str);
    	*string = new_str;
	}
} 

stHeader* read_header(FILE* file)
{
	char name[PATH_MAX];
	size_t name_length;
	size_t file_size;
	
	/* header layout at tar file:
	
	+---------------------------------------------+
	|  name_length |  name char array | file_size |
    +---------------------------------------------+
                      ^^^^^^^^^^^^^^^
 				 		  with \0
	                   

    */

	fread(&name_length , sizeof(size_t)       , 1 , file);
	fread(&name        , sizeof(char)         , name_length , file);
	fread(&file_size   , sizeof(size_t)       , 1 , file);

	stHeader* header = (stHeader*)malloc(sizeof(stHeader));

    header->name = clone_str(name);
	header->name_length = name_length;
	header->file_size = file_size;

	return header;
}

void write_header(FILE* file , stHeader* header)
{
	char name[1024];
	
	/* header layout at tar file:
	
	+---------------------------------------------+
	|  name_length |  name char array | file_size |
    +---------------------------------------------+
                      ^^^^^^^^^^^^^^^
 					      with \0
	                   

    */

	fwrite(&(header->name_length) , sizeof(size_t)       , 1 , file);
	fwrite(header->name           , sizeof(char)         , header->name_length , file);
	fwrite(&(header->file_size)   , sizeof(size_t) , 1 , file);
}
