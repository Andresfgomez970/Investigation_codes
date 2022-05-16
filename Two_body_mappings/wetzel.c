#include "wetzel.h"

/*  This code is intended to do the following: will suppose a distribution for 
  the Euler angles, true anomaly, and periapsis. For this, a point will be 
  generated with GeneratePointFromInputFunction. Then it will pass the 
  obtained point to the phase space, which wil represent the fiducual data.

Later, it will identify all possible orbital parameters with 
  MontecarloUniformConstrainedSampleGetNPoints, then save 
  them and repeat until a fit can be identified in the parameters.

The idea in this is to evaluate the consistency of the algorithm and a possible 
  bias it could have doing the mapping, thanks to observational error for 
  example. This is overall a very creative way to adjust the "how near function 
  that we are in fact supposing to be 10 percent".
*/

/* 
Structure of only one function in order to do an array of this ones and 
  have a kernel or a set of function to do something in general
*/  
typedef struct{

  double (*f) (const double * const param, const double * const x);
  
}Kernel_of_Function;

/*
Strucuture of for the kernel in question that is going to be mapped
*/  
typedef struct{
  // Dimension of the result input function to be mapped, number of random var
  int n_dim;

  // Dimension of parameters for each function; it is declared for
  //  initialization in memory. Size of vector para for each set of variables.
  int * dim_of_param;

  // Save the parameters for the function i in function_param[i]; so it would 
  //   have dim_of_param[i] parameters.
  double ** function_param;

  // Dimension that woud have each function in values space
  int * dim_of_val;

  // Ranges of each variable to be mapped; first index refers to the function,
  //   the second one to the n-th dimension in that function. e.g. 
  //   probability = F(x1,x2)g(x3) -> dim(val_0) = 1. 
  double ** val_0;
  double ** val_f;

  // Kernel of the functions
  Kernel_of_Function * kernel_fuction;

}Kernel_Info;

/*
input
  *param: parameters of the distribution.
  x: N dimensional array in which it will be evaluated. 

result
  evaluation of the gaussian function

This routine calculates the gaussian with parameters param in the point x.
*/
double Gaussian(const double * const param, const double * const x){
  return 1. / (sqrt(2 * M_PI) * param[1]) *
    exp(- (x[0] - param[0]) * (x[0]-param[0]) / ( 2 * param[1] * param[1]));
}

/*
input
  *param: parameters of the distribution.
  x: N dimensional array in which it will be evaluated. 
output
  1
Define a constant function of 1, the param and the value of the x are passed 
  because there is another routine that uses this function and that one must
  specify the info in the input.
*/
double Constant(const double * const param, const double * const x){
  return 1; 
}

/*
Memory is allocated to the pointer kernel, that will save all the function. 
  Therefore, this one is passed by referece. Also, the correspondent functions
  of the kernel are defined.
*/  
int DefineConstantKernel(Kernel_of_Function **kernel, int N_dim){
  
  //Allocating memeory of kernel of functions to save N functions
  * kernel = (Kernel_of_Function *) malloc(sizeof(Kernel_of_Function) * N_dim);
  
  // Define each function as constant
  for(int i = 0; i < N_dim; i++)
    (*kernel + i)->f = Constant;

  return 0; 
}

int InitConstantKernelInfo(Kernel_Info *kernel){
  // ------------------------------------------------------------
  // TODO: do with general function, input from header file (all data for
  //   program to run), program can be run in any units (-> G can be changed).

  // Init array to save the dimension of the parameters for each function
  kernel -> n_dim = 6; // This is the number of functions to be mapped
  kernel -> dim_of_param = (int *) malloc(sizeof(int) * kernel->n_dim);

  // When no params are given, as for a constant kernel the dimension on the
  //   parameters are chosen to be 0.
  for(int i = 0; i < kernel->n_dim; i++)
    (kernel -> dim_of_param)[i] = 1; 

  // Allocate the array to save the parameters for each function and then 
  //   allocate memory for the parameters of each function.
  kernel -> function_param = (double **) malloc(sizeof(double *) * kernel -> n_dim);

  for(int i = 0; i < kernel->n_dim; i++)
    (kernel -> function_param)[i] = (double *) malloc(sizeof(double) * (kernel -> dim_of_param)[i]);


  // This would save the dimensional space of the random variables of each 
  //  function.
  kernel -> dim_of_val = (int *) malloc(sizeof(int) * kernel->n_dim);
  
  // Again in constant kernel the dim_of_val is taken as 1 for each function.
  for(int i = 0; i < kernel->n_dim; i++)
    (kernel -> dim_of_val)[i] = 1; 

  // Allocate the memory for val_0 and val_f; the first index indicate the 
  //   function, the second one the initial or final value of the of the 
  //   variables to be mapped in question.
  kernel -> val_0 = (double **) malloc(sizeof(double *) * kernel->n_dim);
  kernel -> val_f = (double **) malloc(sizeof(double *) * kernel->n_dim);

  // Each val_i/f[function_index] will have dimension one as dim of value
  //  indicate.
  for(int i = 0; i < kernel->n_dim; i++){
    (kernel -> val_0)[i] = (double *) malloc(sizeof(double) * (kernel -> dim_of_val)[i]);
    (kernel -> val_f)[i] = (double *) malloc(sizeof(double) * (kernel -> dim_of_val)[i]);    
  }

  // -----------------------------------------------
  // TODO: recieve input for this
  // Initializing init values
  (kernel -> val_0)[0][0] = 3.0;
  (kernel -> val_0)[1][0] = 0.0;
  (kernel -> val_0)[2][0] = 0.8;
  (kernel -> val_0)[3][0] = 0.0;
  (kernel -> val_0)[4][0] = 0.0;
  (kernel -> val_0)[5][0] = 0.0;
  
  // Initializing final values
  (kernel -> val_f)[0][0] = 21;
  (kernel -> val_f)[1][0] = 2 * M_PI;
  (kernel -> val_f)[2][0] = 1.4;
  (kernel -> val_f)[3][0] = 2 * M_PI;
  (kernel -> val_f)[4][0] = M_PI;
  (kernel -> val_f)[5][0] = 2 * M_PI;

  // Allocating memory for the kernel and defining it inside function
  DefineConstantKernel(&(kernel -> kernel_fuction), kernel -> n_dim);
  
  return 0;
}

/*
Do generate point for kernel
*/ 
int GenPointForKernel(double * point, Kernel_Info kernel){
  // Go over each function and generate each point
  for(int i = 0; i < kernel.n_dim; i++){
    /*
    // Test of function
    printf("cycle: %d\n", i);
    printf("val_0[%d][0] = %lf\n", i, kernel.val_0[i][0]);
    printf("val_f[%d][0] = %lf\n", i, kernel.val_f[i][0]);
    printf("dim_of_param[%d] = %d\n", i, kernel.dim_of_param[i]);
    printf("kernel_function[%d].f = %lf\n", i, kernel.kernel_fuction[i].f(kernel.function_param[i], &point[i]));
    */
    
    // printf("%lf", point[i]);
    // printf("in1: %p\n", ptr_file);
    GeneratePointFromInputFunction(kernel.function_param[i], &point[i], kernel.kernel_fuction[i].f,
                                   kernel.val_0[i], kernel.val_f[i], kernel.n_dim);
    // printf("in2: %p\n", ptr_file);
    // printf(" %lf\n", point[i]);
  }

  // printf("in3: %p\n", ptr_file);
  return 0;
}

/*
Structure that encompass all variables important for the constrain over the 
mapping
*/  
typedef struct{
  // As the following function defines how near was the point over a possible
  //  observation, it was included here. In other case some meaningful value
  //  to study what happens will also be used.
  double (* MetricF)(const double * const param, const double * const x);
  // Function to evaluate the posterior condition
  _Bool (* ConstrainF) (const double * const param, const double * const x);
  // Parameters for constrain function
  double * param_constrain; 
}Constrain_info;

int InitConstrainInfo(Constrain_info * constrain_info){

  constrain_info -> MetricF = ChiSquareObservables;
  constrain_info -> ConstrainF = ChiSquareObservablesNPercentError;
  constrain_info -> param_constrain = (double *) malloc(sizeof(double) * 5);
  
  // constrain_info 
  double G = 43007.1;
  double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) )*G;
  (constrain_info -> param_constrain)[0] = 17.95;
  (constrain_info -> param_constrain)[1] = -17.47;
  (constrain_info -> param_constrain)[2] = 192.0*1.023;
  (constrain_info -> param_constrain)[3] = mu;
  (constrain_info -> param_constrain)[4] = 0.1;

  return 0; 
}

/*
Structure of all parameters of the running information
*/  
typedef struct{

  size_t total_config;
  size_t j_0;  // Actual number of configurations
  size_t n_points; // Number of points to be saved one they are full

}Run_info;

int InitRunInfo(Run_info * run_info){

  run_info -> total_config = 1000; 
  run_info -> j_0 = 0; 
  run_info -> n_points = 1000; 

  return 0; 
}

int MontecarloStrcuturesConstrainedSampleGetNPoints(FILE * ptr_output_file,
    Run_info run_info, Kernel_Info kernel_info, Constrain_info constrain_info){ 
  
  size_t j, l; // General counter, counter for ns_points
  int i = 0;
  // x array over which the condition is applied, and also saves the result of
  //  the metric
  double * x = (double *) malloc( (kernel_info.n_dim + 1) * sizeof(double)); 
 
  // Create and initialize variables array in order to fill it in the routine
  double * variables = (double *) malloc(run_info.n_points * (kernel_info.n_dim + 1) * sizeof(double));
 
  l = 0;
  j = run_info.j_0;
  while(j < run_info.total_config){
    // printf("%ld\n", j);
    // Generate point according to kernel and save in variables the found point; 
    //   later will be evaluated if it satisfy further posteriors.

    GenPointForKernel(x, kernel_info);

    for (i = 0; i < kernel_info.n_dim; i++){
      variables[l * (kernel_info.n_dim + 1) + i] = x[i];
      // printf("%lf ", x[i]);
      }
    
    // Calculate a metric to the condition
    x[i] = constrain_info.MetricF(constrain_info.param_constrain, x);
    
    // Save its values in the mean time to probably save in the future 
    variables[l * (kernel_info.n_dim + 1) + i] = x[i]; 

    // Evaluate saving condition
    if(constrain_info.ConstrainF(constrain_info.param_constrain, x)){
      l++;
      j++;
      // printf("index save: %ld \n", l * (1 + kernel_info.n_dim));
      // printf("%ld\n", j);  
      printf("%zu j , %d j<total_config \n", j, j < run_info.total_config); 
      printf("l==run_info.n_points %d\n\n", l==run_info.n_points);
    }
    
    
    if (l == run_info.n_points){
      SaveVariablesInFile(variables, kernel_info.n_dim + 1, run_info.n_points,
                      ptr_output_file);

      l = 0; 
    }
    
  }

  printf("here: %ld", l);
  if(l !=0 )
    SaveVariablesInFile(variables, kernel_info.n_dim + 1, l, ptr_output_file);

  return 0;
}

/* This is an example for the generation of points of a point that can
    corrspond to a general n-dimensional distribution. */
int GenerateNPointsExample(void){
  double point[1];
  double param[2];
  double var_0[2];
  double var_f[2];
  int i = 0;

  param[0] = 0;
  param[1] = 1;

  var_0[0] = -3;
  var_0[1] = 0;

  var_f[0] = 3;
  var_f[1] = 1;

  int dim = 1;

  FILE * output_file;

  output_file = fopen("test.txt", "wb+");

  while (i < 10000)
  {
    GeneratePointFromInputFunction(param, point, Gaussian, var_0, var_f, dim);
    i++;
    printf("%d %lf\n",i, point[0]);
    fprintf(output_file, "%lf\n", point[0]); 
  }

  return 0;
}

int main()
{ 
  // Define the structures that constanins all the info for the mapping
  Kernel_Info kernel_info;
  Constrain_info constrain_info;
  Run_info run_info;

  InitConstantKernelInfo(&kernel_info); 
  InitConstrainInfo(&constrain_info);
  InitRunInfo(&run_info);

  FILE * ptr_file; 
  ptr_file = fopen("Here.dat", "wb+");

  MontecarloStrcuturesConstrainedSampleGetNPoints(ptr_file, run_info, 
      kernel_info, constrain_info);

  fclose(ptr_file);
  return 0;
}


/* 
  // Verify info of Constant kernel
  printf("dim: %d\n", kernel_info.n_dim); 
  for(int i = 0; i < kernel_info.n_dim; i++){
    // printf("%d", kernel_info.dim_of_param[i]);
    printf("%lf", kernel_info.val_0[i][0]);
    printf(" %lf", kernel_info.val_f[i][0]);
    printf(" \n");
  }
  exit(0);
*/
