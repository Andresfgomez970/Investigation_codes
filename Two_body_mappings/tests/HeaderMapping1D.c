#include <../montecarlo.c>

#include <../mappings.c>
#include <../two_body_analytical.c>
#include <../input_output.c>

#include <math.h> //Additional for example

double Gaussian(double* param, double *x);

void Gaussian1DMapExample();

size_t total_config;
size_t j_0;
size_t n_points;

int n_var;
int n_par; 

double *var_0;
double *var_f;
double *variables; 
double *param;

FILE * ptr_output_file;
FILE * ptr_input_file;
char namefile[500];

int main(int argc, char *argv[]){

  total_config=10; 
  printf("Result of mapping with header with %zu points",total_config); 
  Gaussian1DMapExample();
  

  ptr_input_file = fopen(namefile,"rb"); 
  char c;
  int state; 
  while( ( state = fread(&c,sizeof(char),1,ptr_input_file) )  != 0)
    printf("%c",c);

  fseek(ptr_input_file,0L,SEEK_SET); 
  printf("\n\n\n\n----------------------------- Printing file until line 15 --------------------------------\n"); 

  int i=0; 
  while (i != 15){
    fread(&c,sizeof(char),1,ptr_input_file);
    printf("%c",c); 
 
    if (c=='\n')
      i++; 
   }
  fclose(ptr_input_file); 

  ptr_output_file = fopen(namefile, "rb+"); 

  printf("\n\n\n\n----------------------------- Writig a from the byte_size_header-2 postion (replace last digit in number) --------------------------------\n"); 

  fseek(ptr_output_file,806,SEEK_SET); 
  fprintf(ptr_output_file,"%c",'c'); 

  
  fseek(ptr_output_file,0L,SEEK_SET); 
  while( ( state = fread(&c,sizeof(char),1,ptr_output_file) )  != 0)
    printf("%c",c);

     
  fclose(ptr_output_file);

  // return output to normal state
  Gaussian1DMapExample();
  
  
  return 0;
}


/////////////////////// Defining examples
void Gaussian1DMapExample(){
  j_0 = 0;
  n_points = 200;

  n_var = 1;
  n_par = 2; 

  variables = (double *) malloc( sizeof(double)* n_points*(n_var+1) );
  param = (double *) malloc(sizeof(double)*n_par);
  var_0 = (double *) malloc(sizeof(double)*n_var);
  var_f = (double *) malloc(sizeof(double)*n_var);  
  
  var_0[0] = -4; var_f[0] =  4;
  param[0] = 0;  param[1] = 1; 

  sprintf(namefile,"data/Mappings/gaussian%zu.dat",total_config); 
 
  ptr_output_file = fopen(namefile,"wb");
  
  char *method="UniformMontecarloMap";
  const char* variables_names[1] = {"#x(min/max)"};

  WriteMapHeader(namefile, method, total_config, variables_names, n_var, var_0, var_f, ptr_output_file);  
  MontecarloUniformSampleGetNPoints(total_config, j_0, n_points, n_var, var_0, var_f, variables, Gaussian, param , ptr_output_file);  

  fclose(ptr_output_file); 

}


/////////////////////// Defining functions to be mapped
double Gaussian(double *param, double *x){
  return 1./param[1]*exp(- (x[0]-param[0])*(x[0]-param[0])/(2*param[1]*param[1]) );
}


                   
