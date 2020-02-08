#include <../montecarlo.c>

#include <math.h> //Additional for example

double Gaussian(double* param, double *x);

double Linear(double* param, double *x);

void Gaussian1DMapNPoints();
void Line1DMapNPoints(); 

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

  for(int i=20; i<101; i+=10){
    total_config = i; 
    Gaussian1DMapNPoints();
  }

  //Line1DMapNPoints(); 

  
  return 0;
}


/////////////////////// Defining examples
void Gaussian1DMapNPoints(){
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

  char namefile[500];
  sprintf(namefile,"data/Mappings/gaussian%zu.dat",total_config); 
  
  ptr_output_file = fopen(namefile,"wb");
  
  MontecarloUniformSampleGetNPoints(total_config, j_0, n_points, n_var, var_0, var_f, variables, Gaussian, param , ptr_output_file);  

  fclose(ptr_output_file); 
  
}

void Line1DMapNPoints(){
  total_config=100;
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

  
  ptr_output_file = fopen("data/Mappings/linear.dat","wb");

  MontecarloUniformSampleGetNPoints(total_config, j_0, n_points, n_var, var_0, var_f, variables, Linear, param , ptr_output_file);  
}



/////////////////////// Defining functions to be mapped
double Gaussian(double *param, double *x){
  return 1./param[1]*exp(- (x[0]-param[0])*(x[0]-param[0])/(2*param[1]*param[1]) );
}

double Linear(double *param, double *x){
  return param[0] + param[1]*x[0]; 
}
                   
