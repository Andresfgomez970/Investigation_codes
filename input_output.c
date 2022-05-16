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

/* This routine implements a careful reading of the file, in case no file 
  is enccountered 
input:
  filename: name of file to be read
  readmode: type of reading
result
  FILE * buffer to the file of interes
*/
FILE *ReadFileName(const char *const filename, const char *const readmode)
{
  // Declare file pointer variable and try to read
  FILE *stream = fopen(filename, readmode);

  // Verify that in fact something was read.
  if (stream == NULL)
  {
    printf("It was not possible to read %s\n", filename);
    exit(-1); // exit in case of error
  }

  return stream;
}

/* This routine creates a blank string with a given lenght; useful to later
  save names. Last two indexes are added to the srting in order to contain 
  \n and \0 to know when the string ends and good printing. 
input:
  blank_name: string of lenght 'lenght', composed of ' '.
  lenght: number of ' ' in blank_name  
*/
void InitBlankString(char * const blank_name, size_t length)
{
  // Counter to know how many letters have been writen in the string
  size_t i; 
  // Write lenght blank spacs in the string
  for (i = 0; i < length - 2; i++)
  {
    blank_name[i] = ' ';
  }
  // Add \n and \0 to string, this for printing reasons and to know where really
  //   ends.
  blank_name[i++] = '\n';
  blank_name[i] = '\0';
}


/* This routine insert one string into another, initial position from insert 
  and lenght of the inserted string are used to insert. 
input:
  name: string in which new string is going to be inserted.
  name_lenght: number of index that name can have.
  pos: initial index from which the inserted string will be written.
  lenght: size of the string to b inserted, or the initial lenght characters of
    it.
  name_insert: string to be inserted.
result:
  name replacing from pos to pos + lenght the values of name_insert from 0 to
    lenght. e.g. Insert(Perro)
*/
int Insert(char *name, int name_lenght, int init_index, int lenght, const char *const name_insert)
{
  // Verify if the operations to be made are valid.
  if (init_index + lenght >= name_lenght){
    printf("%s", "In this case errors of unxpected behaviour, please check \
           that string to be inserted from pos does not exeed the lenght \
           on which is inserted");
    exit(0); 
  }
 
  // Run over the lenght of string to be inserted
  for (int i = 0; i < lenght; i++)
    name[init_index + i] = name_insert[i];  // insert from specified position
  
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
  pointer ptr to mkake it point to the value
input:
  ptr_input_file: pointer of file that is being read
  format: type of value to be pointed in ptr
  ptr: pointer that refers to the valuable information
result
  value pointed in ptr
*/
int ValueInLineN(FILE *ptr_input_file, const char *const format, void *ptr,
                 const int n)
{
  // Go to n-th line in the file for which ptr_input_file points to
  GoToFileLine(ptr_input_file, n);
  // Make ptr to point to one direction of memory of the value in line
  fscanf(ptr_input_file, format, ptr);

  return 0;
}

/* This routinec creates n_names with a maximum size of max_size and base_name, 
   the results is saved in names; note that if base_name = x and n_names = N, 
   the result stored in name is x1,...,XN.
*/
int  CreateNthBaseNames(char *** names, char * base_name, int n_names, int max_size){
  char dummy[MAX_STRING_HEADER]; // to write deault variables in case variable names are no given
  *names = (char **) malloc(sizeof(char *) * n_names);

  for (int i = 0; i < n_names; i++){
    sprintf(dummy, "%s%d", base_name, i);
    (*names)[i] = strdup(dummy);
  }

  return 0;
}

int CheckNthBaseNames(char ** names, char * base_name, int n_names, int max_size){
  char dummy[MAX_STRING_HEADER]; // to write deault variables in case variable names are no given
  int i = 0;

  while (i < n_names){
    sprintf(dummy, "%s%d", base_name, i);
    if(!strcmp(dummy, names[i]))
      i++;
    else
      return 0;
  }

  return 1;
}



/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.

result:
  int extracted, or -1 in case of possible error

This routine will obtain an int from the key mentioned from a simple .json file; with
  only one pair of{}.
*/
int ExtractJsonInt(struct json_object * jobj, char * key, _Bool optional){
	struct json_object *jobj_inside;
	int val_type, result = -1;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);
	if (val_type == JSON_INT)
		result = json_object_get_int(jobj_inside);
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": 1 ", key, key);
  }
  else{
		printf("The key %s intended to extract is not an int; verify format e.g. \"%s\": 1 ", key, key);
		exit(0);
	}
		
	return result;
}

int ExtractJsonInt64(struct json_object * jobj, char * key, _Bool optional){
	struct json_object *jobj_inside;
	int val_type, result = -1;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);
	if (val_type == JSON_INT)
		result = json_object_get_int64(jobj_inside);
	else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": 1", key, key);
		exit(0);
  }
  else{
		printf("The ke %s intended to extract is not an int; verify fromat e.g. \"%s\": 1 ", key, key);
		exit(0);
	}

	return result;
}

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
int ExtractJsonDoubleArray(struct json_object * jobj, char * key, double ** array, _Bool optional){
	// Extract array
	struct json_object *tmp, *jobj_inside;
	int n_dim, val_type;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY){
		n_dim = json_object_array_length(jobj_inside);
		*array = (double *) malloc(sizeof(double) * n_dim);

		for (int i = 0; i < n_dim; i++) {
			// Set in tmp the values in array
			tmp = json_object_array_get_idx(jobj_inside, i);
			val_type = json_object_get_type(tmp);
      if (val_type == JSON_DOUBLE){
				(*array)[i] = json_object_get_double(tmp);
        }
			else{
				printf("The array value to extract from %s[%d] is not a double; verify format e.g. \"a\": [1., 2.], check the point in EACH number\n", key, i);
				exit(0);
			}
			/* printf("%s[%d] = %f\n", key, i, array[i]); */
		}
	}
	else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": [1, 3]", key, key);
		exit(0);
  }
	else{
		printf("%s is not identified as double array; it must go between [], e.g. [1.0, 3.0]",  key);
		exit(0);
	}	

	return 0;
}

/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.
  double ** array: this is the array intended to be read.

result:
  all values and array mamory initialized for array. 
  
This routine will obtain all array values from the key mentioned from a simple .json file; with
  only one pair of{}; note that array is assumed double.
*/
int Extract2DJsonDoubleArray(struct json_object * jobj, char * key, double *** array, _Bool optional){
	// Extract array
	struct json_object *tmp, *tmp2, *jobj_inside;
	int n_dim, n_dim_in, val_type;
  int i, j; // General counters

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY){
    // Declare aray to save arrays
		n_dim = json_object_array_length(jobj_inside);
		*array = (double **) malloc(sizeof(double *) * n_dim);

		for (i = 0; i < n_dim; i++){
			tmp = json_object_array_get_idx(jobj_inside, i);
			val_type = json_object_get_type(tmp);

      if (val_type == JSON_ARRAY){
        n_dim_in = json_object_array_length(tmp);
        (*array)[i] = (double *) malloc(sizeof(double) * n_dim_in);
        
        for (j = 0; j < n_dim_in; j++){
          tmp2 = json_object_array_get_idx(tmp, j);
          val_type = json_object_get_type(tmp2);          
          if (val_type == JSON_DOUBLE){
            (*array)[i][j] = json_object_get_double(tmp2);
          }
          else{
				  printf("The array value to extract from %s[%d][%d] is not double or is not a 2D array; verify format e.g. \"a\": [[1., 2.],[3., 4.0]], check the point in EACH number\n", key, i, j);
				  exit(0);
          }
        }
      }      
			else{
				printf("The array value to extract from %s[%d] is not an array; verify format e.g. \"a\": [[1., 2.],[3., 4.0]], check the point in EACH number\n", key, i);
				exit(0);
			}
		}
	}
	else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": [[1., 2.],[3., 4.0]]", key, key);
		exit(0);
  }
	else{
		printf("%s is not identified as array; it must go between [], e.g. [[1., 2.],[3., 4.0]]",  key);
		exit(0);
	}	

	return 0;
}

/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.
  Strcut Double2Array: extract array, and the dimensions of each embeded one.

result:
  all values and array mamory initialized for array. 
  
This routine will obtain all array values from the key mentioned from a simple .json file; with
  only one pair of{}; note that array is assumed double.
*/
int ExtractJsonDouble2DArray(struct json_object * jobj, char * key, Double2DArray * array, _Bool optional){
	// Extract array
	struct json_object *tmp, *tmp2, *jobj_inside;
	int val_type;
  int i, j; // General counters

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY){
    // Declare aray to save arrays
		array->len = json_object_array_length(jobj_inside);
		array->array = (double **) malloc(sizeof(double *) * array->len);
    array->dims = (int *) malloc(sizeof(int) * array->len);

		for (i = 0; i < array->len; i++){
			tmp = json_object_array_get_idx(jobj_inside, i);
			val_type = json_object_get_type(tmp);

      if (val_type == JSON_ARRAY){
        array->dims[i] = json_object_array_length(tmp);
        array->array[i] = (double *) malloc(sizeof(double) * array->dims[i]);
        
        for (j = 0; j < array->dims[i]; j++){
          tmp2 = json_object_array_get_idx(tmp, j);
          val_type = json_object_get_type(tmp2);          
          if (val_type == JSON_DOUBLE){
            array->array[i][j] = json_object_get_double(tmp2);
          }
          else{
				  printf("The array value to extract from %s[%d][%d] is not double or is not a 2D array; verify format e.g. \"a\": [[1., 2.],[3., 4.0]], check the point in EACH number\n", key, i, j);
				  exit(0);
          }
        }
      }      
			else{
				printf("The array value to extract from %s[%d] is not an array; verify format e.g. \"a\": [[1., 2.],[3., 4.0]], check the point in EACH number\n", key, i);
				exit(0);
			}
		}
	}
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": [[1., 2.],[3., 4.0]]", key, key);
		exit(0);
  }
	else{
		printf("%s is not identified as array; it must go between [], e.g. [1, 3]",  key);
		exit(0);
	}	

	return 0;
}


int ExtractJsonArrayLen(struct json_object * jobj, char * key, _Bool optional){
	// Extract array
	struct json_object *jobj_inside;
	int val_type;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY)
		return json_object_array_length(jobj_inside);
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
		printf("There was not key %s found; verify format and existence e.g. \"%s\": [1., 2.]", key, key);
		exit(0);
  }
  else{
		printf("%s is not identified as array; it must go between [], e.g. [1, 3]",  key);
		exit(0);
  }
}

int ExtractJsonStringArray(struct json_object * jobj, char * key, char *** array, _Bool optional){
	// Extract array
	struct json_object *tmp, *jobj_inside;
	int n_dim, val_type;
  int i;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY){
		n_dim = json_object_array_length(jobj_inside);
		// Alocate memory
    *array = (char **) malloc(sizeof(char *) * n_dim);
    
		for (i = 0; i < n_dim; i++) {
			// Set in tmp the values in array
			tmp = json_object_array_get_idx(jobj_inside, i);
			val_type = json_object_get_type(tmp);
			if (val_type == JSON_STRING){
				(*array)[i] = strdup(json_object_get_string(tmp));
        }
			else{
				printf("The array values to extract are not strings; verify format e.g. \"a\": [\"here\", \"Hello\"], check the point in EACH number\n");
				exit(0);
			}
			/* printf("%s[%d] = %f\n", key, i, array[i]); */
		}
	}
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
    printf("There was not key %s found; verify format and existence e.g. \"%s\": [\"here\", \"Hello\"]", key, key);
  }
  else{
		printf("%s is not identified as array; it must go between [], e.g. [1, 3]",  key);
		exit(0);
  }

	return 0;
}

int Extract2DJsonStringArray(struct json_object * jobj, char * key, char **** array, _Bool optional){
	// Extract array
	struct json_object *tmp, *tmp2, *jobj_inside;
	int n_dim, n_dim_in, val_type;
  int i, j;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);

	if (val_type == JSON_ARRAY){
		n_dim = json_object_array_length(jobj_inside);
		// Alocate memory
    *array = (char ***) malloc(sizeof(char **) * n_dim);
    
		for (i = 0; i < n_dim; i++){
			// Set in tmp the values in array
			tmp = json_object_array_get_idx(jobj_inside, i);
			val_type = json_object_get_type(tmp);

      if (val_type == JSON_ARRAY){
        n_dim_in = json_object_array_length(tmp);
        (*array)[i] = (char **) malloc(sizeof(char *) * n_dim_in);

        for (j = 0; j < n_dim_in ; j++){
          tmp2 = json_object_array_get_idx(tmp, j);
			    val_type = json_object_get_type(tmp2);
          if (val_type == JSON_STRING){
            (*array)[i][j] = strdup(json_object_get_string(tmp2));
          }
          else{
            printf("The array values to extract are not strings; verify format e.g. \"a\": [[\"here\", \"Hello\"], [\"Help\"]], check the point in EACH number\n");
            exit(0);
          }
        }
      }
      else{
        printf("The array values to extract are not strings; verify format e.g. \"a\": [[\"here\", \"Hello\"], [\"Help\"]], check the point in EACH number\n");
        exit(0);
      }
		}
	}
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
    printf("There was not key %s found; verify format and existence e.g. \"%s\": [[\"here\", \"Hello\"], [\"Help\"]]", key, key);
  }
	else{
		printf("%s is not identified as array; it must go between [], e.g. [1, 3]",  key);
		exit(0);
	}	

	return 0;
}



/*
input 
  struct json_object * jobj: this is an object which has the json file read
  char * key: string for the key name in question.
  double * array: this is the array intended to be read.

result:
  all values and array memory initialized for array. 
  
This routine will obtain all array values from the key mentioned from a simple .json file; with
  only one pair of{}; note that array is assumed double.
*/
int ExtractJsonString(struct json_object * jobj, char * key, char ** str, _Bool optional){
	struct json_object *jobj_inside;
	int val_type;

	json_object_object_get_ex(jobj, key, &jobj_inside);
	val_type = json_object_get_type(jobj_inside);
  if (val_type == JSON_STRING){
    *str = strdup(json_object_get_string(jobj_inside));
  }
  else if ((val_type == NONE) & optional){
    return NOT_EXISTENCE;
  }
  else if (val_type == NONE){
    printf("There was not key %s found; verify format and existence e.g. \"%s\": [[\"here\", \"Hello\"], [\"Help\"]]", key, key);
  }
  else
  {
    printf("The key \"%s\" trying to be extracted was not found", key);
    exit(0);
  }
  
	return 0;
}


/*
input 
  char * filename: name of the file that will be used to pass it to a string
  char ** str: the pointer that will save the text present in filename will
    be used.

result:
  All that is in filename in a string from in str.
*/
int FileToStrting(char * filename, char **str){
	long length;
	FILE * f_ptr= ReadFileName(filename, "rb");
	
	// Read string
	if (f_ptr)
	{
		fseek (f_ptr, 0, SEEK_END);
		length = ftell (f_ptr);
		fseek (f_ptr, 0, SEEK_SET);
		*str = malloc (length  + 1);
		
		if (*str)
			fread(*str, 1, length, f_ptr);

    (*str)[length] = '\0';
	}
  fclose (f_ptr);

	return 0;
}

/* This function return a hash value for a string; for a given string it
  tries to assignate an uniform distributed number */
unsigned hash(char * s){    
    unsigned hashval;
     
    for(hashval = 0;  *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    
    return hashval % FUNCTIONHASHSIZE;
}

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
Double_Funct_Element * LookupDoubleFunc(char * s, Double_Funct_Element ** table)
{
    Double_Funct_Element * np;

    for (np = table[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np;
    return NULL;
}

Bool_Funct_Element * LookupBoolFunc(char * s, Bool_Funct_Element ** table)
{
    Bool_Funct_Element * np;

    for (np = table[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np;
    return NULL;
}

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
                                         Double_Funct_Element ** table)
{
    Double_Funct_Element * np;
    unsigned hashval;

    if ((np  = LookupDoubleFunc(name, table)) == NULL){  // Not found
        np = (Double_Funct_Element *) malloc(sizeof(*np));
        // Verify correct alocation and name association
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        // In case there is not one there it will ne NULL; in other case the
        //  the new one will save the old one and it will occupy the place it
        //   had. 
        np->next = table[hashval];
        table[hashval] = np;
    }
    else // name is already found, therefore defn will be changed
        free((void *) np->double_func);
    if ((np->double_func = f) == NULL)
        return NULL;
    return np;
}

Bool_Funct_Element * InstallBoolFunc(char * name, _Bool (* f)(const double * const p, const double * const x),  
                                     Bool_Funct_Element ** table)
{
    Bool_Funct_Element * np;
    unsigned hashval;

    if ((np  = LookupBoolFunc(name, table)) == NULL){  // Not found
        np = (Bool_Funct_Element *) malloc(sizeof(*np));
        // Verify correct alocation and name association
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        hashval = hash(name);
        // In case there is not one there it will ne NULL; in other case the
        //  the new one will save the old one and it will occupy the place it
        //   had. 
        np->next = table[hashval];
        table[hashval] = np;
    }
    else // name is already found, therefore defn will be changed
        free((void *) np->bool_func);
    if ((np->bool_func = f) == NULL)
        return NULL;
    return np;
}

int FreeDoubleFuncTable(Double_Funct_Element ** hashtab, size_t size){
    Double_Funct_Element * temp;

    for(size_t i = 0; i < size; i++){
        while(hashtab[i] != NULL){
            temp = hashtab[i];
            hashtab[i] = hashtab[i] -> next;
            free(temp->name);
            free(temp);
        }
    }

  return 0;
}

int FreeBoolFuncTable(Bool_Funct_Element ** hashtab, size_t size){
    Bool_Funct_Element * temp;

    for(size_t i = 0; i < size; i++){
        while(hashtab[i] != NULL){
            temp = hashtab[i];
            hashtab[i] = hashtab[i] -> next;
            free(temp->name);
            free(temp);
        }
    }

  return 0;
}

double DoubleModule(double a, double b){
  while(a > b)
    a -= b;

  return a;
}


// An error-checked malloc() wrapper function
void * ErrorCheckMalloc(unsigned int size){
  void *ptr;
  ptr = malloc(size);
  if(ptr == NULL)
    FatalError("in ErrorCheckMalloc() on memory allocation");
  return ptr;
}

// To print the error
void FatalError(char *message){
  char error_message[100];
  strcpy(error_message, "[!!] Fatal Error ");
  strncat(error_message, message, 100);
  perror(error_message);
  exit(-1);
}
