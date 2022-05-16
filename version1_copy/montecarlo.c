#include "montecarlo.h"

/*
input
  total_config: Total of number points to get in montecarlo mapping
  j0: Number of configurations that has been already generated in order to be 
    saved. This one is designated principally to say to the program up to which
    number in the configurations has been mapped; a seed is assumed for this case.
  n_points: number of points to be saved in variable variables; when it arrives 
    to this number the n_points configurations made to the moment are saved.
  dim: number of variables of the distribution to be mapped or simply the 
    dimension of the function to be mapped.
  var_0: initial values of the intervals within the mapping is done; 
    if f(x,y) -> x0 = var[0] and y0=var[1]
  var_f: final values of the interval within the mapping is done.
    variables: save Nvar dimensional mapping; the structure is variables = 
    {x_00,x_10...,x_Nvar0,x_01,....x_Nvar1,x_02,..,x_Nvar2...}

*/
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


/*
input:
total_config: Total of number points to get in montecarlo mapping
j0: Number of configurations that has been already generated in order to be 
  saved. This one is designated principally to say to the program up to which
  number in the configurations has been mapped; a seed is assumed for this case.
 n_points: number of points to be saved in variable variables; when it arrives 
   to this number the n_points configurations made to the moment are saved.
dim: number of variables of the distribution to be mapped or simply the 
  dimension of the function to be mapped.
var_0: initial values of the intervals within the mapping is done; 
  if f(x,y) -> x0 = var[0] and y0=var[1]
var_f: final values of the interval within the mapping is done.
variables: save Nvar dimensional mapping; the structure is variables = 
  {x_00,x_10...,x_Nvar0,x_01,....x_Nvar1,x_02,..,x_Nvar2...}
*/
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

/*
input:
point: this is the dim point to be mapped from the input distribution.
dim: number of variables of the distribution to be mapped or simply the 
  dimension of the function to be mapped.
var_0: initial values of the intervals within the mapping is done; 
  if f(x,y) -> x0 = var[0] and y0=var[1]; this one as the next has dim + 1
  indexes because the interval for the distirbution function is also
  contained.
var_f: final values of the interval within the mapping is done.

result:
  return the point mapped in the variable point.

note: the seed for the mapping is taken by default, to change it, this one has 
  to be set befor caling the function.
*/
int GeneratePointFromInputFunction(const double * const param, double * const point, double (*FunctionIn) (const double * const param, const double * const x), 
  double * const var_0, double * const var_f, int dim)
{
  // Variable to save the probability associated with point
  double probability_value;
  // Set flag of generated point to 0; because point have not been generated
  _Bool generated_state = 0; 
  // General counter 
  int i;

  // Try until point is generated
  while(generated_state == 0)
  { 
    // Generate a random point in the indicated renge
    for (i = 0; i < dim; i++){ 
      point[i] = var_0[i] + drand48() * (var_f[i] - var_0[i]);
    }
    // Evaluate the density of mass probability of the point
    probability_value = drand48();
    // Evaluate if this one the above chosen point is suitable
    generated_state = probability_value < FunctionIn(param, point);
  }

  return 0;
}

/*
input: 
  variables: array of data to be saved.
  n_dim: dimension of each point saved.
  n_points: number of point that are goint to be saved.
  ptr_output_file: file where all i going to be stored.
*/
int SaveVariablesInFile(double *variables, int n_dim, int n_points, FILE* ptr_output_file){
  fwrite(variables, sizeof(variables[0]), n_dim * n_points, ptr_output_file);  

  return 0;
}



