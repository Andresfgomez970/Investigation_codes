#include "input_output.h"


int PrintMatrixArray(const double * const M, const int columns, const int rows){
  int i, j;
  for (i=0; i<rows; i++){
    for(j=0; j<columns; j++){
      printf("%.15lf ",M[i*columns+j]);
    }
    printf("%s","\n");
  }

  return 0;
}

int GoToFileLine(FILE * ptr_input_file, const int n){
  int i = 0;
  char c;

  fseek(ptr_input_file, 0L, SEEK_SET);

  while (i != n-1){
    fread(&c,sizeof(char),1,ptr_input_file);
    
    if (c=='\n')
      i++;
    
  }

  return 0;
}

int ValueInLineN(FILE * ptr_input_file, const char * const format, void *ptr, const int n){

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
