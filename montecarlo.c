#include "montecarlo.h"

Double_Funct_Element * double_func_montecarlo_table[FUNCTIONHASHSIZE] = {NULL};

int SetUpMontecarloDefaults(){
  // INit possible distribution functions
  SetUpDefaultMontecarloFunctionsTable();
  // Init seed for alleatory number initializations
  srand48(time(0));

  return 0;
}

int SetUpDefaultMontecarloFunctionsTable(){
  InstallDoubleFunc("Constant", Constant, double_func_montecarlo_table); 
  InstallDoubleFunc("Gaussian", Gaussian, double_func_montecarlo_table);  
  InstallDoubleFunc("WetzelrminFixed", WetzelrminFixed, double_func_montecarlo_table);
  InstallDoubleFunc("WetzeleFixed", WetzeleFixed, double_func_montecarlo_table);

  return 0;
}

// Respective free

int FreeDefaultMontecarloFunctionsTable(){
  FreeDoubleFuncTable(double_func_montecarlo_table, FUNCTIONHASHSIZE);

  return 0;
}

int FreeMontecarloDefaults(){
  FreeDoubleFuncTable(double_func_montecarlo_table, FUNCTIONHASHSIZE);

  return 0;
}

// Inline functions to used for the wetzel distributions defined.
static inline double g(double z){
    return 1 / pow(1 + z, 4);
}

static inline double Mstar(double z){
    return pow(12.42 - 1.56 * z + 0.038 * z * z, 10);
}

static inline double Ri(double alpha_i, double beta_i, double gamma_i, double z, double Mhost){
    return alpha_i * (1 + beta_i * pow(g(z) * Mhost / Mstar(z), gamma_i));
}

static inline double Ci(double alpha_i, double beta_i, double gamma_i, double z, double Mhost){
    return alpha_i * (1 + beta_i * pow(Mhost / Mstar(z), gamma_i));
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
  double * const var_0, double * const var_f, double const f_max_value, int dim)
{
  // Variable to save the probability associated with point
  double f_proposal;
  // Set flag of generated point to 0; because point have not been generated
  _Bool generated_state = 0; 
  // General counter 
  int i;

  // Try until point is generated
  while(!generated_state){
    // Generate a random point in the indicated renge
    for (i = 0; i < dim; i++){ 
      point[i] = var_0[i] + drand48() * (var_f[i] - var_0[i]);
    }
    // Evaluate the density of mass probability of the point
    f_proposal = drand48() * f_max_value;
    // Evaluate if this one the above chosen point is suitable
    generated_state = f_proposal < FunctionIn(param, point);
  }

  return 0;
}

/*
Do generate point for kernel
*/ 
int GenPointForKernel(double * point, Kernel_Info * kernel){
  int dim_mapped = 0;

  // Go over each function and generate each point
  for(int i = 0; (i < kernel->n_functions) && (dim_mapped < kernel->n_dim); i++){
    GeneratePointFromInputFunction(kernel->function_param[i], &point[dim_mapped], kernel->kernel_fuction[i].f,
                                   kernel->val_0[i], kernel->val_f[i], kernel->f_max_values[i], kernel->dim_of_val[i]);
    
    dim_mapped = dim_mapped + kernel->dim_of_val[i];
  }


  if (kernel->n_dim != dim_mapped){
    printf("%s", "Dimentionality error: the dimensions mapped are not in agreement");
    exit(0);
  }

  return 0;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Example Kernel ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

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
  return 1.; 
}

//  param: constains alpha_0, beta_0, gamma_0, z, Mhost, alpha_1, beta_1, gamma_1, z, Mhost
//  x: the respective rmin
double WetzelrminFixed(const double * const param, const double * const x){
    static _Bool once = 0;
    static double R_0 = 0;
    static double R_1 = 0;

    if (!once){
        once = 1;
        R_0 = Ri(param[0], param[1], param[2], param[3], param[4]);
        R_1 = Ri(param[5], param[6], param[7], param[8], param[9]);
    }

    return R_0 * exp(- pow(x[0] / R_1, 0.85));
}

//  param: constains alpha_0, beta_0, gamma_0, z, Mhost, alpha_1, beta_1, gamma_1, z, Mhost   
//  x: the respective eccentticity
double WetzeleFixed(const double * const param, const double * const x){
    static _Bool once = 0;
    static double C_0 = 0;
    static double C_1 = 0;

    if (!once){
        once = 1;
        C_0 = Ci(param[0], param[1], param[2], param[3], param[4]);
        C_1 = Ci(param[5], param[6], param[7], param[8], param[9]);
    }

    return C_0 * pow(1 - x[0] * x[0], 0.525) * pow(1 - pow(1 - x[0] * x[0], 0.5), C_1);
}

double Gaussian(const double * const param, const double * const x){
  return 1 / (pow(2 * M_PI, 0.5) * param[1]) * exp(- (x[0] - param[0]) * (x[0] - param[0]) / (2 * param[1] * param[1]) );
}

/*
Memory is allocated to the pointer kernel, that will save all the function. 
  Therefore, this one is passed by referece. Also, the correspondent functions
  of the constant kernel are defined.
*/  
int DefineConstantKernelFunctions(Kernel_of_Function **kernel, int n_functions){
  
  //Allocating memeory of kernel of functions to save N functions
  * kernel = (Kernel_of_Function *) malloc(sizeof(Kernel_of_Function) * n_functions);
  
  // Define each function as constant
  for(int i = 0; i < n_functions; i++)
    (*kernel + i)->f = Constant;

  return 0; 
}


/*
Memory is allocated to the pointer kernel, that will save all the function. 
  Therefore, this one is passed by referece. Also, the correspondent functions
  of the kernel are defined.
*/  
int DefineKernelFunctions(Kernel_of_Function **kernel, int n_functions, char ** function_names){
  
  //Allocating memeory of kernel of functions to save N functions
  * kernel = (Kernel_of_Function *) malloc(sizeof(Kernel_of_Function) * n_functions);

  // Define each function as constant
  for(int i = 0; i < n_functions; i++){
    (*kernel + i)->f = LookupDoubleFunc(function_names[i], double_func_montecarlo_table)->double_func;
  }

  return 0; 
}

// In this example several functions are considered, the default done has
//  a better preformance.
int InitConstantKernelExample(Kernel_Info *kernel){
  // ------------------------------------------------------------
  // TODO: do with general function, all input in param file,
  //   input from header file (all data for program to run), program can be run 
  //   in any units (-> G can be changed).

  // Init array to save the dimension of the parameters for each function
  kernel -> n_functions = 6; // This is the number of functions to be mapped

  kernel -> n_dim = 6; // Number of dim of the points; used to check

  // We assume by construction that the constant function returns 1. Then 
  //  no  parameters are needed for it, the dims on the parameter space of each 
  //  function is chosen to be 1 by default.
  kernel -> dim_of_param = (int *) malloc(sizeof(int) * kernel->n_functions);
  for(int i = 0; i < kernel->n_functions; i++)
    (kernel -> dim_of_param)[i] = 1; 

  // Allocate the array to save the parameters for each function and then 
  //   allocate memory for the parameters of each function; initialize to -1
  //   in this case to make clear that they do not affect the result.
  kernel -> function_param = (double **) malloc(sizeof(double *) * kernel -> n_functions);
  // Also, alocate the maximum value in proposed rejection method; as Constant
  //   return 1 by defult f_max_values are all 1.
  kernel -> f_max_values = (double *) malloc(sizeof(double ) * kernel -> n_functions);

  for(int i = 0; i < kernel->n_functions; i++)
  {
    (kernel -> function_param)[i] = (double *) malloc(sizeof(double) * (kernel -> dim_of_param)[i]);
    (kernel -> function_param)[i][0] = -1;
    (kernel -> f_max_values)[i] = 1;
  }

  // This would save the dimensional space of the random variables of each 
  //  function, in this case 6 functions are defined and each is uniform
  //  in its respective range.
  kernel -> dim_of_val = (int *) malloc(sizeof(int) * kernel->n_functions);
  
  // Again in constant kernel the dim_of_val is taken as 1 for each function.
  for(int i = 0; i < kernel->n_functions; i++)
    (kernel -> dim_of_val)[i] = 1; 

  // Allocate the memory for val_0 and val_f; the first index indicate the 
  //   function, the second one the initial or final value of the of the 
  //   variables to be mapped in question.
  kernel -> val_0 = (double **) malloc(sizeof(double *) * kernel->n_functions);
  kernel -> val_f = (double **) malloc(sizeof(double *) * kernel->n_functions);

  // Each val_i/f[function_index] will have dimension one as dim of value
  //  indicate.
  for(int i = 0; i < kernel->n_functions; i++){
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
  DefineConstantKernelFunctions(&(kernel -> kernel_fuction), kernel -> n_functions);
  
  return 0;
}


/*
input: 
  Kernel_Info * kernel: it has all teh information from the distribution to be
    mapped; in this case is more easily initialized since it is from a
    a constant frunction.
  val_0: Minimum values in function f(x1, x2, ...,  xn) to be mapped.
  val_f: Maximum values in function f(x1, x2, ...,  xn) to be mapped.

output:
  Kernel_Info * kernel correctly initialized.
*/
int InitConstantKernelFromInput(Kernel_Info *kernel, int n_dim, double * val_0,  double * val_f, char ** variables_name, char ** parameters_name){
  // Init array to save the dimension of the parameters for each function
  kernel -> n_functions = 1; // This is the number of functions to be mapped
  kernel -> n_dim = n_dim; // Number of dim of the points; used to check

  // Alocate the maximum value in proposed rejection method; as Constant
  //   return 1 by defult f_max_values are all 1.
  kernel -> f_max_values = (double *) malloc(sizeof(double ) * kernel -> n_functions);
  (kernel -> f_max_values)[0] = 1;
  
  // This would save the dimensional space of the random variables of each 
  //  function, in this case 6 functions are defined and each is uniform
  //  in its respective range.
  kernel -> dim_of_val = (int *) malloc(sizeof(int) * kernel->n_functions);
  (kernel -> dim_of_val)[0] = n_dim; 

  // Allocate the memory for val_0 and val_f; the first index indicate the 
  //   function, the second one the initial or final value of the of the 
  //   variables to be mapped in question.
  kernel -> val_0 = (double **) malloc(sizeof(double *) * kernel->n_functions);
  kernel -> val_f = (double **) malloc(sizeof(double *) * kernel->n_functions);

  // Each val_i/f[function_index] will have dimension one as dim of value
  //  indicate.
  (kernel -> val_0)[0] = val_0;
  (kernel -> val_f)[0] = val_f;
  
  // Now let us go for the name variables
  if (variables_name == NULL){
    kernel->allocate_variables_names = 1;
    printf("variables_name is NULL, so x1, ..., xn are set as default\n"); 
    CreateNthBaseNames(&variables_name, "x", n_dim, MAX_STRING_HEADER);
  }
  else{
    kernel->allocate_variables_names = 0; 
  }
  (kernel -> function_variables_names) = variables_name;

  CreateNthBaseNames(&(kernel -> function_names), "U", kernel->n_functions, MAX_STRING_HEADER);

  kernel -> dim_of_param = (int *) malloc(sizeof(int) * kernel->n_functions);
  (kernel -> dim_of_param)[0] = 0;

  // func_param is allocated altough its values are not use, since in 
  //   genpointfromkernel routine Constant is called with func_param[0].
  kernel -> function_param = (double **) malloc(sizeof(double *) * kernel->n_functions);
  kernel -> function_param_names = NULL;

  // Allocating memory for the kernel and defining it inside function
  DefineConstantKernelFunctions(&(kernel -> kernel_fuction), kernel -> n_functions);

  return 0;
}

/*
input: 
  Kernel_Info * kernel: it has all teh information from the distribution to be
    mapped; in this case is more easily initialized since it is from a
    a constant frunction.

output:
  Kernel_Info * kernel correctly initialized.
*/
int InitConstantKernelFromJsonFile(Kernel_Info *kernel, char * param_filename){
  char * str = 0;  // To save the input of the file as a string 
	struct json_object *jobj;  // Then str used to init this object
	char key[1000];  // Use it to indicate defuaul keys in input
  int result; // from extracting names array

  int n_dim;
  double * val_0, * val_f; 
  char ** variables_name, **params_name = NULL;

	// Get the sring from the file
	FileToStrting(param_filename, &str);

	// Parse input file and read arguments
	jobj = json_tokener_parse(str);

  // Read the data there
  sprintf(key, "n_dim"); 
	n_dim = ExtractJsonInt(jobj, key, !OPTIONAL);

	sprintf(key, "val_0");
	ExtractJsonDoubleArray(jobj, key, &val_0, !OPTIONAL);

	sprintf(key, "val_f");
	ExtractJsonDoubleArray(jobj, key, &val_f, !OPTIONAL);

  sprintf(key, "variables_name");
  result = ExtractJsonStringArray(jobj, key, &variables_name, OPTIONAL);
  if (result == NOT_EXISTENCE)
    variables_name = NULL;

  // Finally init
  InitConstantKernelFromInput(kernel, n_dim, val_0,  val_f, variables_name, params_name); 
  free(str);
  json_object_put(jobj);

  return 0;
}


// It will free thev variables that
int FreeConstantKernelDefault(Kernel_Info * kernel_info){
  int i = 0;

  free(kernel_info -> dim_of_val);
  free(kernel_info -> f_max_values);
  free(kernel_info -> val_0);
  free(kernel_info -> val_f);


  if (kernel_info->allocate_variables_names){
    for (i = 0; i < kernel_info->n_dim; i++){
      free(kernel_info->function_variables_names[i]);
    }
  free(kernel_info->function_variables_names);
  }

  for (i = 0; i < kernel_info->n_functions; i++){
    free(kernel_info->function_names[i]);
  }
  free(kernel_info->function_names);
  free(kernel_info->dim_of_param);  
  free(kernel_info->kernel_fuction);
  free(kernel_info -> function_param);

  return 0;
}

int FreeConstantKernelComplete(Kernel_Info * kernel_info){
  int i = 0;

  free(kernel_info -> dim_of_val);
  free(kernel_info -> f_max_values);
  
  for (i = 0; i < kernel_info->n_functions; i++)
  {
     free(kernel_info -> val_0[i]);
    free(kernel_info -> val_f[i]);
  }
  free(kernel_info -> val_0);
  free(kernel_info -> val_f);

  for (i = 0; i < kernel_info->n_dim; i++)
  {
    free(kernel_info->function_variables_names[i]);
  }
  free(kernel_info->function_variables_names);

  for (i = 0; i < kernel_info->n_functions; i++)
  {
    free(kernel_info->function_names[i]);
  }
  free(kernel_info->function_names);
  free(kernel_info->dim_of_param);  
  free(kernel_info->kernel_fuction);
  free(kernel_info->function_param);

  return 0;
}

int InitKernelFromJsonFile(Kernel_Info * kernel_info, char * param_filename){
  int read_result = 0;

  // Variables to read file content
  char *str;
  struct json_object *jobj;  // Then str used to init this object
  char key[MAX_STRING_HEADER];  // Use it to indicate defuaul keys in input

  Double2DArray dummy_array; // variable to extract 2D arrays more efficient

  // Variables to write file content
  FileToStrting(param_filename, &str);
  jobj = json_tokener_parse(str);

  strcpy(key, "function_param");
  ExtractJsonDouble2DArray(jobj, key, &dummy_array, !OPTIONAL);
  kernel_info -> function_param = dummy_array.array;
  kernel_info -> dim_of_param = dummy_array.dims;

  strcpy(key, "function_param_names");
  read_result = Extract2DJsonStringArray(jobj, key, &(kernel_info->function_param_names), OPTIONAL);
  // Set to NULL to later process defaults
  if (read_result == NOT_EXISTENCE)
      (kernel_info->function_param_names) = NULL;

  strcpy(key, "val_0");
  ExtractJsonDouble2DArray(jobj, key, &dummy_array, !OPTIONAL);
  kernel_info -> val_0 = dummy_array.array;
  kernel_info -> dim_of_val = dummy_array.dims;
  kernel_info -> n_functions = dummy_array.len; 

  kernel_info -> n_dim = 0;
  for (int i = 0; i < kernel_info->n_functions; i++)
    kernel_info -> n_dim += kernel_info -> dim_of_val[i]; 

  strcpy(key, "val_f");
  Extract2DJsonDoubleArray(jobj, key, &(kernel_info->val_f), !OPTIONAL);

  strcpy(key, "function_variables_names");
  read_result = ExtractJsonStringArray(jobj, key, &(kernel_info->function_variables_names), OPTIONAL);
  if (read_result == NOT_EXISTENCE)
      (kernel_info->function_variables_names) = NULL;

  strcpy(key, "function_names");
  read_result = ExtractJsonStringArray(jobj, key, &(kernel_info->function_names), !OPTIONAL);

  strcpy(key, "f_max_values");
  ExtractJsonDoubleArray(jobj, key, &(kernel_info->f_max_values), !OPTIONAL);

  // Init now the function names
  DefineKernelFunctions(&(kernel_info -> kernel_fuction), kernel_info -> n_functions, kernel_info -> function_names);

  InitKernelDefaults(kernel_info, kernel_info->function_param_names, kernel_info->function_variables_names, kernel_info->function_names);

  return 0;
}

// It will free thev variables that
int FreeKernelDefault(Kernel_Info * kernel_info){
  int i, j;

  if (kernel_info->allocate_variables_names){
    for (i = 0; i < kernel_info->n_dim; i++){
      free(kernel_info->function_variables_names[i]);
    }
  free(kernel_info->function_variables_names);
  }

  if (kernel_info->allocate_param_names){
    for (i = 0; i < kernel_info->n_functions; i++){
      for (j = 0; j < kernel_info->dim_of_param[i]; j++){
        free(kernel_info->function_param_names[i][j]);
      }
      free(kernel_info->function_param_names[i]);
    }
  free(kernel_info->function_variables_names);
  }

  free(kernel_info->kernel_fuction);

  return 0;
}

int FreeKernelComplete(Kernel_Info * kernel_info){
  int i; // General variables for initialization

  free(kernel_info->function_variables_names);
  free(kernel_info->f_max_values);
  free(kernel_info->function_names);
  free(kernel_info->dim_of_param);
  free(kernel_info->dim_of_val);


    for (i = 0; i < kernel_info->n_functions; i++){
      free(kernel_info->function_param[i]);
      free(kernel_info->function_param_names[i]);
      free(kernel_info->val_0[i]);
      free(kernel_info->val_f[i]);
    }

    free(kernel_info->function_param);
    free(kernel_info->function_param_names);
    free(kernel_info->val_0);
    free(kernel_info->val_f);

  return 0;
}


int InitKernelFromInput(Kernel_Info *kernel, int n_dim, int * dim_of_param, double ** function_param, char *** function_param_names,
                        char ** function_variables_names, char ** function_names, int * dim_of_val, double **val_0, double **val_f,
                        double * f_max_values, int n_functions){
  // Dimension of the result input function to be mapped, number of random var
  kernel-> n_dim = n_dim;

  // Dimension of parameters for each function; it is declared for
  //  initialization in memory. Size of vector para for each set of variables.
  kernel->dim_of_param = dim_of_param;

  // Save the parameters for the function i in function_param[i]; so it would 
  //   have dim_of_param[i] parameters.
  kernel->function_names = function_names;
  kernel->function_param = function_param;

  // Dimension that woud have each function in values space
  kernel->dim_of_val = dim_of_val;

  // Ranges of each variable to be mapped; first index refers to the function,
  //   the second one to the n-th dimension in that function. e.g. 
  //   probability = F(x1,x2)g(x3) -> dim(val_0) = 1. 
  kernel->val_0 = val_0; 
  kernel->val_f = val_f; 

  kernel->f_max_values = f_max_values;  
  kernel->n_functions = n_functions;

  DefineKernelFunctions(&kernel->kernel_fuction, n_functions, function_names);

  InitKernelDefaults(kernel, function_param_names, function_variables_names, function_names);

  return 0;
}

int InitKernelDefaults(Kernel_Info * kernel, char *** function_param_names, char ** function_variables_names, char ** function_names){
  char dummy[MAX_STRING_HEADER]; // dummy for base name
  int i; // General counter

  if (function_param_names == NULL){
    printf("function_param_names is NULL, so paramf11,...,paramf1N1, paramf21, ...., paramf2N2,.., paramnfNN are set as default\n");
    kernel->allocate_param_names = 1;
    function_param_names = (char ***) malloc(sizeof(char **) * kernel->n_functions);
    for (i = 0; i < kernel->n_functions; i++){
      function_param_names[i] =  (char **) malloc(sizeof(char *) * kernel->dim_of_param[i]); 
      sprintf(dummy, "paramf%d", i);
      CreateNthBaseNames(&function_param_names[i], dummy, kernel->dim_of_param[i], MAX_STRING_HEADER);
    }
  }
  else{
    kernel->allocate_param_names = 0; 
  }
  (kernel -> function_param_names) = function_param_names;  

  if (function_variables_names == NULL){
    printf("function_variables_names is NULL, so x1,...,xdim are set as default\n");
    kernel->allocate_variables_names = 1;
    function_variables_names = (char **) malloc(sizeof(char *) * kernel->n_dim);
    CreateNthBaseNames(&function_variables_names, "x", kernel->n_dim, MAX_STRING_HEADER);
  }
  else{
    kernel->allocate_variables_names = 0; 
  }
  (kernel -> function_variables_names) = function_variables_names;

  if (kernel -> function_names == NULL)
    DefineConstantKernelFunctions(&(kernel -> kernel_fuction), kernel -> n_functions);

  // There are some maxv_values that depend of many parameters of the input functions, and it
  //   is therefore valuable to automatize this calculation altough we demand them a input they
  //   will be changed internally for this special cases.
  for (int i = 0; i < kernel->n_functions; i++)
  {
    if ( (kernel -> kernel_fuction)[i].f == WetzelrminFixed || (kernel -> kernel_fuction)[i].f == WetzeleFixed){
      kernel->f_max_values[i] = Ri(kernel->function_param[i][0],
                                   kernel->function_param[i][1],
                                   kernel->function_param[i][2],
                                   kernel->function_param[i][3],
                                   kernel->function_param[i][4]);
    }  
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

