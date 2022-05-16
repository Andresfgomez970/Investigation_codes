#ifndef _input_output
#define _input_output

#include <stdlib.h>
#include <stdio.h>
#include <json.h>
#include <string.h>

// Default values for json files in C
#define NONE 0
#define JSON_DOUBLE 2
#define JSON_INT 3
#define JSON_ARRAY 5
#define JSON_STRING 6

#define JSON_FILE 1

#define FUNCTIONHASHSIZE 101 // default size for array of functions
#define MAX_STRING_HEADER 100 // In memory will indicate 102 size

// Resturns for extraction
#define SUCCESS 0
#define READ_ERROR -1
#define NOT_EXISTENCE -2

// For parameters given as input
#define OPTIONAL 1

/*
Strucutures to create a hash table of functions with their sring name
*/
typedef struct Double_Funct_Element_tag{
    struct Double_Funct_Element_tag * next;
    char * name;
    double (* double_func) (const double * const param, const double * const x); 
}Double_Funct_Element;

typedef struct Bool_Funct_Element_tag{
    struct Bool_Funct_Element_tag * next;
    char * name;
    _Bool (* bool_func) (const double * const param, const double * const x); 
}Bool_Funct_Element;


/*
Define Array2D strcutrue to be easily extract
*/
typedef struct Double2DArrayTag{
  double ** array;
  int * dims;
  size_t len;
}Double2DArray;


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


int  CreateNthBaseNames(char *** names, char * name, int n_names, int max_size);
int CheckNthBaseNames(char ** names, char * base_name, int n_names, int max_size);


/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.

result:
  int extracted, or -1 in case of possible error

This routine will obtain an int from the key mentioned from a simple .json file; with
  only one pair of{}.
*/
int ExtractJsonInt(struct json_object * jobj, char * key, _Bool optional);
int ExtractJsonInt64(struct json_object * jobj, char * key, _Bool optional);

/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.
  double * array: this is the array intended to be read.

result:
  all values and array mamory initialized for array. 
  
This routine will obtain all array values from the key mentioned from a simple .json file; with
  only one pair of{}; note that array is assumed double.
*/
int ExtractJsonDoubleArray(struct json_object * jobj, char * key, double ** array, _Bool optional);
int ExtractJsonArrayLen(struct json_object * jobj, char * key, _Bool optional);
int Extract2DJsonDoubleArray(struct json_object * jobj, char * key, double *** array, _Bool optional);
int ExtractJsonDouble2DArray(struct json_object * jobj, char * key, Double2DArray * array, _Bool optional);

/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.
  double * str: this is the array intended to be read.

result:
  all values and array mamory initialized for array. 
  
This routine will obtain all array values from the key mentioned from a simple .json file; with
  only one pair of{}; note that array is assumed double.
*/
int ExtractJsonString(struct json_object * jobj, char * key, char ** str, _Bool optional);
int ExtractJsonStringArray(struct json_object * jobj, char * key, char *** array, _Bool optional);
int Extract2DJsonStringArray(struct json_object * jobj, char * key, char **** array, _Bool optional);

/*
input 
  char * filename: name of the file that will be used to pass it to a string
  char ** str: the pointer that will save the text present in filename will
    be used.

result:
  All that is in filename in a string from in str.
*/
int FileToStrting(char * filename, char **str);

/* This function return a hash value for a string; for a given string it
  tries to assignate an uniform distributed number */
unsigned hash(char * s);

/*
input 
  char * s: specific name for the function to be search in the input tabel
  Double_Funct_Element ** table: the array of function element in which the
    function named by s will be serach.

result:
  A function element with a next element which point to another function in
    thec ase its names shares a hash with another, the name of the function
    in an string, and the pointer to the function defined with such name.
*/
Double_Funct_Element * LookupDoubleFunc(char * s, Double_Funct_Element ** table);
Bool_Funct_Element * LookupBoolFunc(char * s, Bool_Funct_Element ** table);

/*
input 
  char * s: specific name for the function to be added to the input tabel.
  Double_Funct_Element ** table: the array of function in which the input function
    pointer is to be saved.

result:
  A function element with a next element which point to another function in
    thec ase its names shares a hash with another, the name of the function
    in an string, and the pointer to the function defined with such name.
*/
Double_Funct_Element * InstallDoubleFunc(char * name, double (* f)(const double * const p, const double * const x), 
                                         Double_Funct_Element ** table);
Bool_Funct_Element * InstallBoolFunc(char * name, _Bool (* f)(const double * const p, const double * const x),  Bool_Funct_Element ** table);

int FreeDoubleFuncTable(Double_Funct_Element ** hashtable, size_t size);
int FreeBoolFuncTable(Bool_Funct_Element ** hashtab, size_t size);

double DoubleModule(double a, double b);

/*
input:
  size: to be allocated to the pointer to which the assignation is made

result:
  error-checked pointer 

An error-checked malloc() wrapper function
*/
void * ErrorCheckMalloc(unsigned int size);


/* 
input:
  message: the idea is that it explains the error and location

  result: print message and exit
*/
void FatalError(char *message);


#endif // _input_output
