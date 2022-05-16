#ifndef _montecarlo
#define _montecarlo

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <json.h>
#include <string.h>
#include <time.h>
#include "input_output.h"

// This will be the default table of functions that ther user can use
//   passing strings from a paramter file.
extern Double_Funct_Element * double_func_montecarlo_table[FUNCTIONHASHSIZE];

/* 
Structure of only one function in order to do an array of this ones and 
  have a kernel or a set of function to do something in general
*/  
typedef struct Kernel_of_FunctionTag{

  double (*f) (const double * const param, const double * const x);
  
}Kernel_of_Function;

/*
Strucuture for the kernel in question that is going to be mapped
*/  
typedef struct Kernel_InfoTag{
  // Dimension of the result input function to be mapped, number of random var
  int n_dim;

  // Dimension of parameters for each function; it is declared for
  //  initialization in memory. Size of vector para for each set of variables.
  int * dim_of_param;

  // Save the parameters for the function i in function_param[i]; so it would 
  //   have dim_of_param[i] parameters.
  char *** function_param_names;
  _Bool allocate_param_names;
  char ** function_names;
  double ** function_param;

  // Dimension that woud have each function in values space
  int * dim_of_val;

  // Ranges of each variable to be mapped; first index refers to the function,
  //   the second one to the n-th dimension in that function. e.g. 
  //   probability = F(x1,x2)g(x3) -> dim(val_0) = 1. 
  double ** val_0;
  double ** val_f;
  char ** function_variables_names;
  _Bool allocate_variables_names;

  // This have the maximum values to map each probability function
  double * f_max_values;
  
  // Number of functions; this is very important since it will really dictate
  //   the first size in dim_of_param, as well as in function_param, val_0, and
  //   val_f.
  int n_functions;

  // Kernel of the functions
  Kernel_of_Function * kernel_fuction;
}Kernel_Info;

int SetUpMontecarloDefaults();
int SetUpDefaultMontecarloFunctionsTable();
int FreeDefaultMontecarloFunctionsTable();
int FreeMontecarloDefaults();

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

int GeneratePointFromInputFunction(const double * const param, double * const point, double (*FunctionIn) (const double * const param, const double * const x), 
  double * const var_0, double * const var_f, double f_max_value, int dim);

int GenPointForKernel(double * point, Kernel_Info * kernel);

///////////////////
// Default Kernels
//////////////////
// Constant
double Constant(const double * const param, const double * const x);
//  param: constains alpha_0, beta_0, gamma_0, z, Mhost, alpha_1, beta_1, gamma_1, z, Mhost
//  x: the respective rmin
double WetzelrminFixed(const double * const param, const double * const x);
//  param: constains alpha_0, beta_0, gamma_0, z, Mhost, alpha_1, beta_1, gamma_1, z, Mhost   
//  x: the respective eccentticity
double WetzeleFixed(const double * const param, const double * const x);
double Gaussian(const double * const param, const double * const x);


int DefineConstantKernelFunctions(Kernel_of_Function **kernel, int n_functions);
int DefineKernelFunctions(Kernel_of_Function **kernel, int n_functions, char ** function_names);

int InitConstantKernelExample(Kernel_Info *kernel);
int InitConstantKernelFromInput(Kernel_Info *kernel, int n_dim, double * val_0,  double * val_f, char ** variables_name, char ** parameters_name);
int InitConstantKernelFromJsonFile(Kernel_Info *kernel, char *filename);

int InitKernelFromInput(Kernel_Info *kernel, int n_dim, int * dim_of_param, double ** function_param, char *** function_param_names,
                        char ** function_variables_names, char ** function_names, int * dim_of_val, double **val_0, double **val_f,
                        double * f_max_values, int n_functions);
int InitKernelFromJsonFile(Kernel_Info * kernel_info, char * param_filename);
int InitKernelDefaults(Kernel_Info * kernel, char *** function_param_names, char ** function_variables_names, char ** function_names);


int FreeConstantKernelDefault(Kernel_Info * kernel_info);
int FreeConstantKernelComplete(Kernel_Info * kernel_info);

int FreeKernelDefault(Kernel_Info * kernel_info);
int FreeKernelComplete(Kernel_Info * kernel_info);


//// Save variables in the input pointer
int SaveVariablesInFile(double *variables, int n_var, int n_points, FILE *ptr_output_file);

#endif // _montecarlo
