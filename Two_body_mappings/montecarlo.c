#include "montecarlo.h"

int MontecarloUniformSampleGetNPoints(size_t total_config, size_t j_0, size_t n_points, int n_var, double *var_0, double *var_f,
				      double *variables, double (*FunctionIn)(const double * const param, const double * const x), double *param ,
				      FILE* ptr_output_file){
  size_t j,l;
  int i;
  double x[n_var];
  int n_dim = n_var + 1;
  
  l = 0;
  for(j=j_0; j<total_config; j++){


    for (i=0; i<n_var; i++){
      x[i] = var_0[i] + drand48()*(var_f[i] - var_0[i]);
      variables[l*n_dim + i] = x[i];
    }

    variables[l*n_dim + i] = FunctionIn(param,x); 
    l++;

    if (l==n_points){
      SaveVariablesInFile(variables, n_dim, n_points, ptr_output_file);
      l=0; 
    }
    
  }

  SaveVariablesInFile(variables, n_dim, l, ptr_output_file);
  
  return 0;
}


int MontecarloUniformConstrainedSampleGetNPoints(size_t total_config, size_t j_0, size_t n_points, int n_dim, double *var_0, double *var_f,
  double *variables, double (*FunctionIn)(const double * const param, const double * const x),
	double *param , FILE* ptr_output_file,
	_Bool (* ConstrainF) (const double * const param, const double * const x),
  double * param_constrain){
  
  size_t j,l; // General counter, counter for ns_points
  int i;
  double x[n_dim]; //x array over which the condition is applied
  
  l = 0;
  j = j_0; 
  while(j<total_config){

    for (i=0; i<n_dim-1; i++){
      x[i] = var_0[i] + drand48()*(var_f[i] - var_0[i]);
      variables[l*n_dim + i] = x[i];
    }

    x[i] = FunctionIn(param,x); 
    variables[l*n_dim + i] = x[i]; 

    if( ConstrainF(param_constrain,x) ){
      l++;
      j++;
      printf("%zu j , %d j<total_config \n",j, j < total_config); 
    }
      
    if (l==n_points){
      SaveVariablesInFile(variables, n_dim, n_points, ptr_output_file);
      l=0; 
    }
    
  }

  SaveVariablesInFile(variables, n_dim, l, ptr_output_file);
  
  return 0;
}


int SaveVariablesInFile(double *variables, int n_dim, int n_points, FILE* ptr_output_file){

  fwrite(variables, sizeof(variables[0]), n_dim * n_points, ptr_output_file);  
  
  return 0;
}



