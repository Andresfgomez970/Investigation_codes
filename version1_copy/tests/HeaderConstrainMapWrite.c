#include <../montecarlo.c>

#include <../mappings.c>
#include <../input_output.c>
#include <../two_body_analytical.c> // things in mapping use this 

int main(){
  char input_filename[MAX_STRING_HEADER];
  char out_filename[MAX_STRING_HEADER]; 
  sprintf(input_filename,"%s.dat","data/Mappings/gaussian10");
  sprintf(out_filename,"%sConstrained.dat","data/Mappings/gaussian10"); 
							    
  FILE * ptr_input_file = ReadFileName(input_filename,"rb+"); 
  FILE * ptr_output_file = ReadFileName(out_filename,"w+"); 
  char * constrain_message = "-3<x<3";

  WriteMapConstrainHeader(ptr_input_file, ptr_output_file, constrain_message, out_filename); 

  fclose(ptr_input_file);
  fclose(ptr_output_file); 

  printf("Print of the resulting header:\n\n\n");

  ptr_input_file = ReadFileName(out_filename,"rb+"); 
  char c;
  while( fread(&c,sizeof(char),1,ptr_input_file)   != 0)
    printf("%c",c);

  
  return 0; 
}