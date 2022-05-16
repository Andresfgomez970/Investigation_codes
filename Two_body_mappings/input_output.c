#include "input_output.h"

/* This file is desired to have several routines that could be commonly
  used in order to manage data flux */


/*
input  
  M: matrix cotained in a one dimensional array
  columns: number of columns
  rows: number of rows
output
  matrix in the terminal as output

This routine is designed in order to print a 2D matrrix that is saved in
  a one dimensional array; following the convention Mij = M(i * columns + j).
In other words first go all the values of the first row, then the second one 
  and so on.
*/
int PrintMatrixArray(const double *const M, const int columns, const int rows)
{
  // Define counters
  int i, j;

  // Runing first over rows
  for (i = 0; i < rows; i++)
  {
    // Run over all columns in a given row
    for (j = 0; j < columns; j++)
    {
      // Print with all sig figures
      printf("%.15lf ", M[i * columns + j]);
    }
    // Print space for good formatting in 2D matrix
    printf("%s", "\n");
  }

  return 0;
}

/*
input
  ptr_input_file: pointer of file that is being read
  n: line to locate buffer (file pointer) in the file

result 
  buffer (ptr_input_file) with the location in specified line 

This routine put the pointer that reads the file (ptr_input_file) in its
  n-th line
*/
int GoToFileLine(FILE *ptr_input_file, const int n)
{
  // Defined to count lines
  int i = 0;
  // Defined to read char by char
  char c;

  /* Put the pointer ptr_input_file with an offset of 0 from the beggning of
    the file */
  fseek(ptr_input_file, 0L, SEEK_SET);

  /* Read char by char and increase i each time a new line character is
    encountered */
  while (i != n - 1)
  {
    // Read one char in ptr ptr_input_file and save it in c
    fread(&c, sizeof(char), 1, ptr_input_file);
    // Verify increase condition
    if (c == '\n')
      i++;
  }

  return 0;
}

/* This routine go to the n-th line and use the value in the multi purpose 
  pointer ptr to mkake it point to the value*/
int ValueInLineN(FILE *ptr_input_file, const char *const format, void *ptr,
                 const int n)
{
  // Go to n-th line in the file for which ptr_input_file points to
  GoToFileLine(ptr_input_file, n);
  // Make ptr to point to one direction in memory
  fscanf(ptr_input_file, format, ptr);

  return 0;
}

/* This routine implements a careful reading of the file, in case no file 
  is enccountered */
FILE *ReadFileName(const char *const filename, const char *const readmode)
{
  // Declare file pointer variable and try to read
  FILE *stream = fopen(filename, readmode);

  // Verify that in fact something was read.
  if (stream == NULL)
  {
    printf("It was not possible to read %s\n", filename);
    exit(-1);
  }

  return stream;
}

/* This routine creates a string with lenght spaces to write a name; two 
  additional index are added to the srting in order to contain \n and \0 to
  know when the string ends */
void InitString(char *const blank_name, size_t length)
{
  // Counter to know how many letters have been writen in the string
  size_t i; 
  // Write lenght blank spacs in the string
  for (i = 0; i < length; i++)
  {
    blank_name[i] = ' ';
  }
  // Add \n and \0 to string
  blank_name[i++] = '\n';
  blank_name[i] = '\0';
}

/* This routine insert one string into another, initial position from insert 
  and lenght of the inserted string are expected too. */
int Insert(char *name, int pos, int lenght, const char *const name_insert)
{
  // run over the lenght of string to be inserted
  for (int i = 0; i < lenght; i++)
    name[pos + i] = name_insert[i];  // insert from specified position
  
  return 0;
}