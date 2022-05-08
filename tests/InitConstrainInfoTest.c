#include "../mappings.h"

_Bool eq_floats(double a, double b, double epsilon)
{
  return fabs(a - b) < epsilon;
}

int InitConstrainInfoFromInputTest();
int InitConstrainInfoFromJsonFileTest();
int CheckStructure(Constrain_info * constrain_info);

Constrain_info constrain_info;

int i = 0; // For to verify initialization

// Doing it for example in http://protean.io:3002/ with orbital parameters
//  below.
double x[7] = {1.498 / (1 + 0.43), 42 * M_PI / 180., 0.43, 
                61 * M_PI / 180.,  31 * M_PI / 180., 201 * M_PI / 180., 0.0004};

double param_aux[4] = {0.578, -0.794, -0.007, 1};
double max_err_in_chi_square = pow(3, 0.5) * 5 * 1e-4;
double param_constrain[5] = {0.578, -0.794, -0.007, 1, 0.1};
int n_param_aux = 4;
int n_param_constrain = 5;
double dummy1;
_Bool dummy2 = 1;
char ** param_aux_names = NULL;
char ** param_constrain_names = NULL;
char * aux_func_name = {"ChiSquareObservables"};
char * constrain_func_name = {"ChiSquareObservablesNPercentError"};
char * constrain_message = {"This is a test"};

int main(int argc, char const *argv[])
{
    int check_state = 1;
    
    check_state = InitConstrainInfoFromInputTest();
    FreeDefaultConstrainInfo(&constrain_info);

    check_state = check_state && InitConstrainInfoFromJsonFileTest();
    FreeConstrainInfoComplete(&constrain_info);
  
    printf("\n****************************** %s *****************************************\n", argv[0]);
    printf("%s pass the test? %d", argv[0], check_state);
    printf("\n****************************** %s *****************************************\n", argv[0]);

    return 0;
}
 
int InitConstrainInfoFromJsonFileTest(){
  // To varify here
  int check_state = 1;
  
  // Real Input
  char * param_filename = "InitConstrainInfoTest_params.json";

  SetUpDefaultMappingsFunctionsTable();
  InitConstrainInfoFromJsonFile(&constrain_info, param_filename);
  check_state = check_state && CheckStructure(&constrain_info); 
  FreeDefaultMappingsFunctionsTable();

  return check_state;
}

int InitConstrainInfoFromInputTest(){
    int check_state = 1;

    InitConstrainInfoFromInput(&constrain_info, aux_func_name, n_param_aux, param_aux, param_aux_names, ChiSquareObservables,
                               constrain_func_name, n_param_constrain, param_constrain, param_constrain_names, ChiSquareObservablesNPercentError, constrain_message);
    check_state = CheckStructure(&constrain_info);

    return check_state;
}


int CheckStructure(Constrain_info * constrain_info){
  int check_state = 1;

  // First check correct values
  for (i = 0; i < n_param_aux; i++)
    check_state = check_state && eq_floats(constrain_info->param_aux[i], param_aux[i], 1e-6);
  for (i = 0; i < n_param_constrain; i++)
    check_state = check_state && eq_floats(constrain_info->param_constrain[i], param_constrain[i], 1e-6);

  dummy1 = constrain_info->AuxF(constrain_info->param_aux, x);
  check_state = check_state && eq_floats(dummy1, 0, max_err_in_chi_square);  
  
  double xp[7];
  // Consider error of 8 percent in values x,y and vz
  xp[0] = 0.826 / (1 + 0.82);
  xp[1] = 113.98 * M_PI/ 180.;
  xp[2] = 0.82;
  xp[3] = 24.928 * M_PI/ 180.;
  xp[4] = 135.585 * M_PI/ 180.;
  xp[5] = 337.982 * M_PI/ 180.;
  xp[6] = constrain_info->AuxF(constrain_info->param_aux, xp);
  
  // Consider out case
  dummy2 = constrain_info->ConstrainF(constrain_info->param_constrain, xp);
  check_state = check_state && !dummy2;

  char dummy3[100];
  
   for (i = 0; i < n_param_aux; i++){
    sprintf(dummy3, "param_aux%d", i);
    check_state = check_state && (strcmp(constrain_info->param_aux_names[i], dummy3) == 0);
  }

  for (i = 0; i < n_param_constrain; i++){
    sprintf(dummy3, "param_constrain%d", i);
    check_state = check_state && (strcmp(constrain_info->param_constrain_names[i], dummy3) == 0);
  }

  return check_state;
}