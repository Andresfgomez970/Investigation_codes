#include "input_output.h"

/* This file is desired to have several routines that could be commonly 
  used in order to manage data flux */


/* This routine is designed in order to print a 2D matrrix that is saved in 
  a one dimensional array; following the convention Mij = M(i * columns + j) */
int PrintMatrixArray(const double * const M, const int columns, const int rows)
{
  // Define counters
  int i, j;

  // Runing first over rows
  for (i = 0; i < rows; i++)
  {
    // Run over all columns in a given row
    for(j = 0; j < columns; j++)
    {
      // Print with all sig figures
      printf("%.15lf ",M[i * columns + j]);  
    }
    // Print space for good formatting in 2D matrix
    printf("%s","\n");
  }

  return 0;
}


/* This routine put the pointer that reads the file (ptr_input_file) in its 
  n-th line */
int GoToFileLine(FILE * ptr_input_file, const int n)
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
  while (i != n-1)
  {
    // Read one char in ptr ptr_input_file and save it in c
    fread(&c, sizeof(char), 1, ptr_input_file);  
    // Verify increase condition
    if (c=='\n')
      i++;
  }

  return 0;
}


/* This routine go to the n-th line and save the value in the general purpose 
*/
int ValueInLineN(FILE * ptr_input_file, const char * const format, void *ptr, 
                 const int n){

  GoToFileLine(ptr_input_file, n); 
  fscanf(ptr_input_file, format, ptr, n);
  
  return 0; 

}

FILE * ReadFileName(const char * const filename, const char * const readmode){

  FILE * stream = fopen(filename,readmode);

  if (stream == NULL){
    printf("It was not possible to read %s\n",filename);
    exit(-1);
  }

  return stream;
}


int Insert(char * name, int pos, int lenght,const char * const name_insert){
  for (int i=0; i<lenght; i++)
    name[pos+i] = name_insert[i];
  return 0;
}


void InitString(char * const name, size_t length){
  size_t i;
  for(i=0; i<length-2; i++ ){name[i] = ' ';}
  name[i++]='\n'; name[i]='\0';
}
