#include <stdlib.h>
#include <stdio.h>
#include "../input_output.c"

/* This short main test for the correct working of the functions GoTofFileLine
  and ValueInLine */
int main()
{
  char * file_name = "value_in_line10.txt";
  char * path_name = "data/hand_made_data_for_testing/value_in_line10.txt";
  FILE *ptr_input_file = fopen(path_name, "r");
  size_t total_config;
  int n = 10; // Line expected to contain the value 10

  printf("Calling GoToLine(ptr_input_file, 10) to read there an int\
with fscanf\n");
  GoToFileLine(ptr_input_file, 10);
  fscanf(ptr_input_file, "%zu", &total_config);
  printf("Value obtained from %s: %zu\n", file_name, total_config);

  total_config = 20; 
  ValueInLineN(ptr_input_file, "%zu", &total_config, 10);
  printf("Value obtained from %s: %zu\n", file_name, total_config);

  fclose(ptr_input_file);

  return 0;
}
