#include "InitKernelTest.h"

_Bool eq_floats(double a, double b, double epsilon)
{
  return fabs(a - b) < epsilon;
}

double DoubleGaussian(const double * const param, const double * const x){
    return Gaussian(&param[0], &x[0]) * Gaussian(&param[2], &x[0]);
}

int Init2DDoubleArray(double *** array, int * dims, double array_values[dims[0]][dims[1]], _Bool verbose){
    *array = (double **) malloc(sizeof(double *)  * dims[0]);
    
    for (int i = 0; i < dims[0]; i++){
        (*array)[i] = (double *) malloc(sizeof(double)  * dims[i + 1]);
        for (int j = 0; j < dims[i + 1]; j++){
            (*array)[i][j] = array_values[i][j];
            if (verbose)
                printf("%lf ", (*array)[i][j]);
        }
        if (verbose)
            printf("\n");
    }

    return 0;
}

int Init2DCharArray(char *** array, int * dims, char array_values[dims[0]][dims[1]], _Bool verbose){
    *array = (char **) malloc(sizeof(char *)  * dims[0]);
    
    for (int i = 0; i < dims[0]; i++){
        (*array)[i] = strdup(array_values[i]);
        if (verbose)
            printf("%s\n", (*array)[i]);
    }

    return 0;
}

int Init3DCharArray(char **** array, int * dims, char array_values[dims[0]][dims[1]][dims[3]], _Bool verbose){
    *array = (char ***) malloc(sizeof(char **)  * dims[0]);

    for (int i = 0; i < dims[0]; i++){
    (*array)[i] = (char **) malloc(sizeof(char *)  * dims[i + 1]);
    for (int j = 0; j < dims[i + 1]; j++){
        (*array)[i][j] = array_values[i][j]; // Why working with both ways?
        if (verbose)
            printf("%s ", (*array)[i][j]);
        }
    if (verbose)
        printf("\n");
    }
 
    return 0;
}


int InitKernelFromInputTest(){
    Kernel_Info kernel_info;
    int n_dim  = 3; 

    double ** function_param;    
    int dim_of_param[3] = {2, 4, 1}; 
    double function_param_values[2][4] = {{0, 1, 0, 3}, {0}};

    double **val_0;
    double **val_f;
    int dim_of_val[3] = {2, 2, 1};
    double val_0_values[2][2] = {{-3,  -9}, {-1}};
    double val_f_values[2][2] = {{3,   9}, {1}};

    double f_max_values[3] = {1 / (pow(2 * M_PI, 0.5) * function_param_values[0][1]), 1 / (pow(2 * M_PI, 0.5) * function_param_values[0][3]), 1}; 

    char ** function_variables_names;
    int dim_variables_names[2] = {3, MAX_STRING_HEADER};
    char function_variables_names_values[3][MAX_STRING_HEADER] = {"x", "y", "f(x,y)"};

    char ** function_names;
    int dim_function_names[2] = {2, MAX_STRING_HEADER};
    char function_names_values[2][MAX_STRING_HEADER] = {"DoubleGaussian", "Constant"};

    int n_functions = 2; 

    char *** function_param_names;
    int dim_of_param_values[4] = {2, 4, 1, MAX_STRING_HEADER}; 
    char function_param_names_values[2][4][MAX_STRING_HEADER] = {{"mean_x", "sigma_x", "mean_y", "sigma_y"}, {"None"}}; 

    int i, j, verbose = 0, check_state = 1;  // General counters to free variables

    Init2DDoubleArray(&function_param, dim_of_param, function_param_values, verbose);
    Init2DDoubleArray(&val_0, dim_of_val, val_0_values, verbose);
    Init2DDoubleArray(&val_f, dim_of_val, val_f_values, verbose); 

    Init2DCharArray(&function_variables_names, dim_variables_names, function_variables_names_values, verbose);
    Init2DCharArray(&function_names, dim_function_names, function_names_values, verbose);

    Init3DCharArray(&function_param_names, dim_of_param_values, function_param_names_values, verbose);

    InitKernelFromInput(&kernel_info,  n_dim,  &dim_of_param[1],  function_param, function_param_names,
                         function_variables_names,  function_names,  &dim_of_val[1], val_0, val_f,
                         f_max_values, n_functions);

    check_state = check_state & (n_dim == kernel_info.n_dim);
    check_state = check_state & (n_functions == kernel_info.n_functions);
    for (i = 0; i < n_dim; i++){
        check_state = check_state & (strcmp(function_variables_names[i], kernel_info.function_variables_names[i]) == 0);
        check_state = check_state & eq_floats(f_max_values[i], kernel_info.f_max_values[i], 1e-6);
    }
    
    for (i = 0; i < dim_of_param[0]; i++){
        check_state = check_state && (strcmp(function_names_values[i], kernel_info.function_names[i]) == 0);
        check_state = check_state && (dim_of_param[i] == kernel_info.dim_of_param[i]);
        check_state = check_state && (dim_of_val[i] == kernel_info.dim_of_val[i]);
        for (j = 0; j < dim_of_param[i + 1]; j++){
            check_state = check_state & eq_floats(function_param[i][j], kernel_info.function_param[i][j] , 1e-6);
            check_state = check_state & (strcmp(function_param_names[i][j], kernel_info.function_param_names[i][j]) == 0);
        }
        for (j = 0; j < dim_of_val[i + 1]; j++){
            check_state = check_state & eq_floats(val_0[i][j], kernel_info.val_0[i][j] , 1e-6);
            check_state = check_state & eq_floats(val_f[i][j], kernel_info.val_f[i][j] , 1e-6);
        } 
    }

    for (i = 0; i < n_functions; i++){
        free(val_0[i]);
        free(val_f[i]);
        free(function_names[i]);
        free(function_param[i]);
        free(function_param_names[i]);
    }
    free(val_0);
    free(val_f);
    free(function_names);
    free(function_param);
    free(function_param_names);
    
    for (i = 0; i < n_dim; i++)
        free(function_variables_names[i]);
    free(function_variables_names);
    
    FreeKernelDefault(&kernel_info);

    return check_state;
}


int InitKernelFromJsonTest(){
    int check_state = 1;

    char param_filename[MAX_STRING_HEADER];
    strcpy(param_filename, "InitKernelTest_params.json");
 
    Kernel_Info kernel_info;
    InitKernelFromJsonFile(&kernel_info, param_filename);
    
    int i, j; // general pointers for verification
    int n_dim  = 3; 
    int dim_of_param[2] = {4, 1}; 
    double function_param[2][4] = {{0, 1, 0, 3}, {0}};

    int dim_of_val[2] = {2, 1};
    double val_0[2][2] = {{-3,  -9}, {-1}};
    double val_f[2][2] = {{3,   9}, {1}};

    double f_max_values[3] = {0.57, 0.57, 1.0}; 
    char function_variables_names[3][MAX_STRING_HEADER] = {"x", "y", "f(x, y)"};
    char function_names[2][MAX_STRING_HEADER] = {"DoubleGaussian", "Constant"};
    int n_functions = 2; 

    char function_param_names[2][4][MAX_STRING_HEADER] = {{"mean_x", "sigma_x", "mean_y", "sigma_y"}, {"None"}}; 

    check_state = check_state & (kernel_info.n_dim == n_dim);
    check_state = check_state & (kernel_info.n_functions == n_functions);
    for (i = 0; i < n_dim; i++){
        check_state = check_state & (strcmp(function_variables_names[i], kernel_info.function_variables_names[i]) == 0);
        check_state = check_state & eq_floats(f_max_values[i], kernel_info.f_max_values[i], 1e-6);
    }
 
    for (i = 0; i < n_functions; i++){
        check_state = check_state && (strcmp(function_names[i], kernel_info.function_names[i]) == 0);
        check_state = check_state && (dim_of_param[i] == kernel_info . dim_of_param[i]);
        check_state = check_state && (dim_of_val[i] == kernel_info . dim_of_val[i]);
        for (j = 0; j < dim_of_param[i]; j++){
            check_state = check_state & eq_floats(function_param[i][j], kernel_info.function_param[i][j] , 1e-6);
            check_state = check_state & (strcmp(function_param_names[i][j], kernel_info.function_param_names[i][j]) == 0);
        }
        for (j = 0; j < dim_of_val[i]; j++){
            check_state = check_state & eq_floats(val_0[i][j], kernel_info.val_0[i][j] , 1e-6);
            check_state = check_state & eq_floats(val_f[i][j], kernel_info.val_f[i][j] , 1e-6);
        }       
    }
    
    FreeKernelComplete(&kernel_info);

    return check_state;
}


int main(int argc, char const *argv[])
{
    int check_state = 1;

    SetUpDefaultMontecarloFunctionsTable();
    
    InstallDoubleFunc("DoubleGaussian", DoubleGaussian, double_func_montecarlo_table);

    check_state = InitKernelFromInputTest();
    check_state = InitKernelFromJsonTest();

    printf("\n****************************** %s *****************************************\n", argv[0]);
    printf("%s pass the test? %d", argv[0], check_state);
    printf("\n****************************** %s *****************************************\n", argv[0]);

    FreeDoubleFuncTable(double_func_montecarlo_table, FUNCTIONHASHSIZE);

    return 0;
}



