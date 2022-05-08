#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "input_output.h"
#include "mappings.h"

int test_byte_size(void);
int test_header_construct(void);
int user_header_transform(void);
int print_separator(void);

int main()
{
  print_separator();
  test_header_construct();
  printf("Check cat test_file.dat\n");
  print_separator();
  print_separator();
  printf("\n");
  
  print_separator();
  printf("test_file.dat nbytes\n");
  test_byte_size();
  print_separator();
  print_separator();
  printf("\n");

  print_separator();
  user_header_transform();
  printf("Check transform_user_header from user_header\n");
  print_separator();
  print_separator();
  printf("\n");

  return 0;
}

int print_separator(void){
  for(int i = 0; i < 30; i++)
    printf("---");
  printf("\n");
  return 0;
}

int user_header_transform(void){
  int nl = 0;
  FILE * ptr_test_file = ReadFileName("user_header.txt", "r");  
  char namefile[MAX_STRING_HEADER], method[MAX_STRING_HEADER];
  char ** variables_names;
  char * dummy;
  int n_var;
  double * var_0, * var_f;
  // To 1000 since any line measure more than this; remember floats 17 chars.
  char line[1000]; 
  size_t n_points;

  // Get number of variables, they are in line 8, nc here for lines
  while (fgets(line, MAX_STRING_HEADER, ptr_test_file)){
      nl++;
      if (nl==8)
        break; 
    }
  n_var = atoi(line);

  // Allocate memory
  variables_names = (char **) malloc(sizeof(char *) * n_var);
  var_0 = (double *) malloc(sizeof(double) * n_var);
  var_f = (double *) malloc(sizeof(double) * n_var);

  for(int i = 0; i  < n_var; i++)
    variables_names[i] = (char *) malloc(sizeof(char) * MAX_STRING_HEADER);

  // Fill all to write header
  nl = 0;
  fseek(ptr_test_file, 0L, SEEK_SET);

  int m_ranges = 0;
  // Get number of variables, they are in line 8, nc here for lines
  while (fgets(line, MAX_STRING_HEADER, ptr_test_file)){
      nl++;
      if(nl == 2)
        sprintf(namefile, "%s", line);
      else if(nl == 4)
        sprintf(method, "%s", line);
      else if(nl == 6)
        n_points = strtol(line, &dummy, 10); 
      else if(nl == 9 + 3 * m_ranges)
        sprintf(variables_names[m_ranges], "%s", line);
      else if(nl == 10 + 3 * m_ranges)
        var_0[m_ranges] = strtod(line, &dummy);
      else if(nl == 11 + 3 * m_ranges)
        var_f[m_ranges++] = strtod(line, &dummy);
    }
  fclose(ptr_test_file);

  ptr_test_file = ReadFileName("transform_user_header.txt", "w");

  WriteMapHeader(namefile, method, n_points, variables_names, n_var, var_0, 
                 var_f,  ptr_test_file);

  

  return 0;
}


int test_byte_size(void){
  char c;
  int nc = 0;
  int n_bytes = 0;
  char line[MAX_STRING_HEADER];
  char * dummy;

  FILE * ptr_test_file = ReadFileName("test_file.dat", "r");

  // Get number of bytes, they are in line 2, nc here for lines
  while (fgets(line, 1000, ptr_test_file)){
      nc++;
      if (nc==2)
        break; 
    }
  n_bytes = atoi(line);

  // print the number of bytes in question, nc here for number of characters
  fseek(ptr_test_file, 0L, SEEK_SET);
  nc = 0;
  while(nc < n_bytes){
    ++nc;
    c = fgetc(ptr_test_file);
    printf("%c", c);
  }
  
  return 0; 
}

int test_header_construct(void){

  char * method = "test_method";
  char * name_file ="test_name";
  size_t n_points = 1e7;
  const char* variables_names[6] = {"#rmin(min/max)", "#f(min/max)", "#e(min/max)", 
                                      "#w(min/max)", "#i(min/max)", "#W(min/max)"};
  int n_var = 6;
  double var_0[6];
  double var_f[6];

  FILE * ptr_test_file = ReadFileName("test_file.dat", "w");
  // FILE * ptr_test_file = fopen("test_file.dat", "w");

  // naming from 1 to 6, and giving intervals of 0-1, 1-2, ...,5-6
  for(int i = 0; i < 6; i++){
    var_0[i] = i * 1.0;
    var_f[i] = (i + 1) * 1.0;
  }

  WriteMapHeader(name_file, method, n_points, variables_names, n_var, var_0, 
                 var_f,  ptr_test_file);


  fclose(ptr_test_file);
  return 0;
}