#ifndef _input_output
#define _input_output

#include <stdlib.h>
#include <stdio.h>

/*
input  
  M: matrix cotained in a one dimensional array
  columns: number of columns
  rows: number of rows

result
  matrix in the terminal as output

This routine is designed in order to print a 2D matrrix that is saved in
  a one dimensional array; following the convention Mij = M(i * columns + j).
In other words first go all the values of the first row, then the second one 
  and so on.
*/
int PrintMatrixArray(const double *const M, const int columns, const int rows);

/*
input
  ptr_input_file: pointer of file that is being read
  n: line to locate buffer (file pointer) in the file

result 
  buffer (ptr_input_file) with the location in specified line 

This routine put the pointer that reads the file (ptr_input_file) in its
  n-th line
*/
int GoToFileLine(FILE *ptr_input_file, const int n);

/*
input
  ptr_input_file: pointer of file that is being read
  format: format that is desired to be read in line n 
  ptr: pointer that will point to the read infomation 
  n: line where the info lies

result
  ptr pointing to the value in the line specified
*/
int ValueInLineN(FILE *ptr_input_file, const char *const format, void *ptr, 
                 const int n);

/*
input 
  ptr_input_file: pointer of file that is going being read, written or append.
  readmode: this indicate how the data in the file can be affected; some 
    examples are.

output
  ReadFileName: pointer to the file being read, written or append; in case of 
    not existence of file it exits.

This routine implements a careful reading of the file; in case no file 
  is encountered this is printed and program exits.
*/
FILE *ReadFileName(const char *const filename, const char *const readmode);

/*
input 
  string to initialize full of blanck spaces, lenght of string 

output
  string initialize with blank spaces

This function creates a string with lenght spaces to write a name; two 
  additional index are added to the srting in order to contain \n and \0 to
  know when the string ends.
*/
void InitBlankString(char *const blank_name, size_t lenght);

/*
input 
  name: string in which name_insert is to be inserted.
  pos: position from which is inserted the new string (index count from 0)
  lenght: lenght of string to be inserted.
  name_insert: string to be inserted in name.

result 
  string name with name_instert inserted in it.

This routine insert one string into another, initial position from insert 
  and lenght of the inserted string are expected too.
*/
int Insert(char *name, int name_lenght, int init_index, int lenght, const char *const name_insert);

#endif // _input_output
