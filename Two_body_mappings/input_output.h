#ifndef _input_output
#define _input_output

#include <stdlib.h>
#include <stdio.h>

/*
 * input:  matrix cotained in a one dimensional array
 * output: matrix in the terminal as output
*/
int PrintMatrixArray(const double * const M,const int columns, const int rows);


/*
 * input:  file that is being read, line to locate buffer in the file
 * output: buffer with the location in specified line 
*/
int GoToFileLine(FILE * ptr_input_file, const int n);


/*
 * input:  file that is being read, format that is desired to be read, pointer to save the read infomation , line where the info lies
 * output: readed value in the line specified 
*/
int ValueInLineN(FILE * ptr_input_file, const char * const format, void *ptr, const int n); 


/*
 * input:  file to be read or writen, readmode
 * output: pointer to the file being read or written 
*/
FILE* ReadFileName(const char * const filename, const char * const readmode);

/*
 * input: string to initialize full of blanck spaces, lenght of string 
 * output: string initialize with blank spaces
*/
void InitString(char * const name, size_t lenght);


/*
 * input: string to insert another string, position from which is inserted new string, lenght of string to be inserted, string to be inserted
 * output: string name with name_instert inserted
*/
int Insert(char * name, int pos, int lenght, const char * const  name_insert);





#endif // _input_output
