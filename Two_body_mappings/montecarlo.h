#include <stdlib.h>
#include <stdio.h>

////////////////////////// About the input for the functions
// ---- Input -------
// total_config: Total of number points to get in montecarlo mapping
// j0: Number of configurations that has been already generated in order to be saved. This one is designated principally to say to the program up to which
//     number in the configurations has been mapped; a seed is assumed for this case. 
// n_points: number of points to be saved in variable variables; when it arrives to this number the n_points configurations made to the moment are saved.
// n_var: number of variables of the distribution or simply function to be mapped.
// var_0: initial values of the intervals within the mapping is done; if f(x,y) -> x0 = var[0] and y0=var[1]
// var_f: final values of the interval within the mapping is done.
// variables: save Nvar dimensional mapping; the structure is variables = {x_00,x_10...,x_Nvar0,x_01,....x_Nvar1,x_02,..,x_Nvar2...}

// --- Output --------
// -1 if a mapping of size Npoints was saved in variables
// other number indicates the number of Nvar dimensional points saves in variables; supposing such number is l, there are l*Nvar valuable
// values that can be read from variables

int MontecarloUniformSampleGetNPoints(size_t total_config, size_t j_0, size_t n_points, int n_var, double *var_0, double *var_f,
				      double *variables, double (*FunctionIn)(const double * const param, const double * const x), double *param ,
				      FILE *ptr_output_file);

int MontecarloUniformConstrainedSampleGetNPoints(size_t total_config, size_t j_0, size_t n_points, int n_var, double *var_0, double *var_f,
						 double *variables, double (*FunctionIn)(const double * const param, const double * const x),
						 double *param , FILE* ptr_output_file,
						 _Bool (* ConstrainF) (const double * const param, const double * const x),
						 double * param_constrain); 

int GeneratePointFromInputFunction(double * const point, double (*FunctionIn) (const double * const param, const double * const x), const double * const param, 
double *var_0, double *var_f, int dim);

//// Save variables in the input pointer
int SaveVariablesInFile(double *variables, int n_var, int n_points, FILE *ptr_output_file);
