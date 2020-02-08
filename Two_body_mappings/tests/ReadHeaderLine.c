#include <stdlib.h>
#include <stdio.h>
#include "../input_output.c"

int main(){
  FILE* ptr_input_file = fopen ("data/Mappings/gaussian10.dat", "r");
  size_t total_config; 

  printf("Calling GoToLine(9,ptr_input_file) to read there total_config with fscanf\n"); 
  GoToFileLine(10,ptr_input_file); 
  fscanf(ptr_input_file,"%zu",&total_config); 
  printf("Value obtained from gaussian10.dat: %zu\n", total_config);

  
  
  fclose(ptr_input_file);
  
  return 0; 
}
