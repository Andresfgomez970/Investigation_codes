#include <../montecarlo.c>

#include <math.h> //Additional for example

double Gaussian2D(double* param, double *x);
double Concave(double* param, double *x);
void paraboloid();

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

int main(int argc, char *argv[]){

  for(total_config=50; total_config<201; total_config+=50){
    paraboloid(); 
  }
  
      
  return 0;
}

/////////////////////// Defining examples
void paraboloid(){
  j_0 = 0;
  n_points = 300;

  n_var = 2;
  n_par = 1; 

  variables = (double *) malloc( sizeof(double)* n_points*(n_var+1) );
  param = (double *) malloc(sizeof(double)*n_par);
  var_0 = (double *) malloc(sizeof(double)*n_var);
  var_f = (double *) malloc(sizeof(double)*n_var);  
  
  var_0[0] = -4; var_f[0] =  4;
  var_0[1] = -4; var_f[1] = 4;
  param[0] = 16;  

  char name[200];
  sprintf(name,"data/Mappings/paraboloid%zu.dat", total_config); 
  
  ptr_output_file = fopen(name,"wb");

  MontecarloUniformSampleGetNPoints(total_config, j_0, n_points, n_var, var_0, var_f, variables, Concave, param , ptr_output_file);  

  
}

/////////////////////// Defining functions to be mapped
double Gaussian2D(double *param, double *x){
  return 1./param[1]*exp(- (x[0]-param[0])*(x[0]-param[0])/(2*param[1]*param[1]) );
}

double Concave(double *param, double *x){
  return  param[0]-(x[0]*x[0]+x[1]*x[1]); 
}
                   
