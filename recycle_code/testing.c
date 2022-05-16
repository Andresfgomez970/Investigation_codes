#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Init3DCharArray(char **** array, int * dims, char array_values[dims[0]][dims[1]][dims[3]], _Bool verbose){
    *array = (char ***) malloc(sizeof(char **)  * dims[0]);

    for (int i = 0; i < dims[0]; i++){
    (*array)[i] = (char **) malloc(sizeof(char *)  * dims[i + 1]);
    for (int j = 0; j < dims[i + 1]; j++){
        (*array)[i][j] = array_values[i][j]; // Why working with both ways?
        if (verbose)
            printf("%s ", (*array)[i][j]);
        }
    if (verbose)
        printf("\n");
    }
 
    return 0;
}

int main(int argc, char const *argv[])
{
/*   char *** function_param_names;
  int dim_of_param_values[4] = {2, 4, 1, 100}; 
  char function_param_names_values[2][4][100] = {{"mean_x", "sigma_x", "mean_y", "sigma_y"}, {"None"}}; 

  Init3DCharArray(&function_param_names, dim_of_param_values, function_param_names_values, 1); */
/* 
  char * b;
  char a[100] = "string_long";
  b = a;
  printf("%s", b); */

  _Bool a;
  a = 0;
  printf("%d", a);

  return 0;
}


