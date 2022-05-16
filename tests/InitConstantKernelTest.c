#include "InitConstantKernelTest.h"

_Bool eq_floats(double a, double b, double epsilon)
{
  return fabs(a - b) < epsilon;
}

int InitConstantKernelFromInputTest(void);
int InitConstantKernelFromInputTest2(void);
int InitConstantKernelFromJsonFiletTest(void);

Kernel_Info kernel_info;
int n_functions = 1;
int n_dim = 2;
int f_max_value = 1;
double val_0[2] = {0, -3};
double val_f[2] = {1, 3}; 
_Bool check_state = 1;
char ** variable_names = NULL;
char ** param_names = NULL;

int main(int argc, char const *argv[])
{
    int check_state = 1;
    printf("%s", "Manual initialization for Kernel\n");
  
    check_state = InitConstantKernelFromInputTest();
    FreeConstantKernelDefault(&kernel_info);

    printf("\n\n%s", "Manual initialization for Kernel\
 initalization by default when variables_names same as default\n");

    check_state = check_state && InitConstantKernelFromInputTest2();
    FreeConstantKernelDefault(&kernel_info);

    printf("\n\n%s", "Json initialization for Kernel, \
initalization by default when variables_names not specified\n");

    check_state = check_state && InitConstantKernelFromJsonFiletTest();
    FreeConstantKernelComplete(&kernel_info);

    printf("\n****************************** %s *****************************************\n", argv[0]);
    printf("%s pass the test? %d", argv[0], check_state);
    printf("\n****************************** %s *****************************************\n", argv[0]);

    printf("\n****************************** notes *****************************************\n");
    printf("Remember that the correct election of the vector(x) belonging to\
    the interval is left to user; this provides more flexibility\n");

    return 0;
}

int CheckStrutcture(){
  int check_state = 1;
  
  check_state = kernel_info.n_dim==n_dim;
  printf("Do we have a 2D data points? %d\n", check_state);

  check_state = check_state && (kernel_info.n_functions == n_functions);
  printf("Only one 2D function? %d\n", check_state);

  int f1 = 0, x1 = 0, x2 = 1;
  check_state = check_state && eq_floats(kernel_info.val_0[f1][x1], val_0[x1], 1e-6) && eq_floats(kernel_info.val_f[f1][x1], val_f[x1], 1e-6);
  printf("Are ranges correct for x1? %d \n", check_state);
  check_state = check_state && eq_floats(kernel_info.val_0[f1][x2], val_0[x2], 1e-6) && eq_floats(kernel_info.val_f[f1][x2], val_f[x2], 1e-6);
  printf("Are ranges correct for x2? %d \n", check_state);

  check_state = check_state && eq_floats(kernel_info.f_max_values[0], f_max_value, 1e-6);
  printf("Is f_max_value by default 1? %d \n", check_state);

  double * param = NULL;
  double x[2] = {0.5, 0};
  check_state = check_state && (kernel_info.kernel_fuction[f1].f(param, x) == 1);
  printf("Does evaluated function is 1? %d \n", check_state);

  return check_state;
}

int InitConstantKernelFromInputTest(void){
  InitConstantKernelFromInput(&kernel_info, n_dim, val_0, val_f, variable_names, param_names);
  check_state = CheckStrutcture();

  return check_state;
}

int InitConstantKernelFromInputTest2(void){
  char * variable_names2[MAX_STRING_HEADER] = {"x0", "x1"};

  InitConstantKernelFromInput(&kernel_info, n_dim, val_0, val_f, variable_names2, param_names);
  check_state = CheckStrutcture();

  char dummy[4];
  for (int i = 0; i < kernel_info.n_dim; i++)
  {
    sprintf(dummy, "x%d", i); 
    printf("%s", kernel_info.function_variables_names[i]);
    check_state = check_state && (strcmp(dummy, kernel_info.function_variables_names[i]) == 0);
  }

  return check_state;
}

int InitConstantKernelFromJsonFiletTest(void){
	char * filename = "InitConstantKernelTest_params.json";
  int check_state = 1;

  InitConstantKernelFromJsonFile(&kernel_info, filename);
  check_state = CheckStrutcture();

  char dummy[3];
  for (int i = 0; i < kernel_info.n_dim; i++)
  {
    sprintf(dummy, "x%d", i); 
    printf("%s", kernel_info.function_variables_names[i]);
    check_state = check_state && (strcmp(dummy, kernel_info.function_variables_names[i]) == 0);
  }

  return check_state;
}


