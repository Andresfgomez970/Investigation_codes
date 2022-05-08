#ifndef _mappings
#define _mappings

#include <json.h>
#include <unistd.h>
#include "two_body_analytical.h"
#include "input_output.h"
#include "montecarlo.h"

// This is provitional solution to the Visual Studio error
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define INT32_LEN 10
#define SIZE_T_LEN 20
#define DOUBLE_LEN 16

// This will be the default tables of functions that ther user can use
//   passing strings from a paramter file.
extern Double_Funct_Element * double_func_mappings_table[FUNCTIONHASHSIZE];
extern Bool_Funct_Element * bool_func_mappings_table[FUNCTIONHASHSIZE];

int SetUpMappingsDefaults();
int SetUpDefaultMappingsFunctionsTable();

int FreeDefaultMappingsFunctionsTable();
int FreeMappingsDefaults();
/*
Structure that encompass all variables important for the constrain over the 
mapping
*/  
typedef struct Constrain_info{
  // As the following function defines how near was the point over a possible
  //  observation, it was included here. In other case some meaningful value
  //  to study what happens will also be used.
  // In general terms it intends to be some type of measure that could
  //  be of interest to be saved or useful for ConstrainF.
  double (* AuxF)(const double * const param, const double * const x);
  char * aux_func_name;
  int  n_param_aux;
  double * param_aux;
  char ** param_aux_names;
  _Bool allocate_param_aux_names;
  // Function to evaluate the posterior condition
  _Bool (* ConstrainF) (const double * const param, const double * const x);
  char * constrain_func_name;
  int  n_param_constrain;
  double * param_constrain;
  char ** param_constrain_names;
  _Bool allocate_constrain_names;
  // General constrain message 
  char * constrain_message;
}Constrain_info;

/*
Structure of all parameters of the running information in a mapping of points
*/  
typedef struct Run_info{

  size_t total_config;
  size_t j_0;  // Actual number of configurations
  size_t n_points; // Number of points to be saved one they are full
  char * out_filename;
  char * methodology;
}Run_info;

/*
Structure that will have all the parameters
*/  
typedef struct Mappings_info{
  Kernel_Info * ptr_kernel_info;
  Run_info * ptr_run_info;
  Constrain_info * ptr_constrain_info;
}Mappings_info;


///////////////////////////////////////////////////////////////////////////////
// Functions that interact with two_body_analytical.h 
///////////////////////////////////////////////////////////////////////////////

/*
input  
  param = [x_o, y_o, vz_o, mu]: o states for original and coord names as usual; 
    the mu parameter states for the reduced mass.  
  x = [rmin, f, e, w, i, W]; orbital parameters of the simulated orbit.
output:
  d = sqrt(x ** 2 + y ** 2 + vz ** 2); where x=xo-xm, .., vz = vzo-vzm and 
    _m states for modelated

Function that calculates the distance between observables and vector state 
  modelated; the standard cartesian metric is used.
*/
double ChiSquareObservables(const double * const param, const double * const x); 


///////////////////////////////////////////////////////////////////////////////
// Functions that interact with only  mappings.h 
///////////////////////////////////////////////////////////////////////////////

/*
input  
  param = [x_o, y_o, vz_o, mu]: o states for original and coord names as usual; 
    the mu parameter states for the reduced mass.  
  x = [rmin, f, e, w, i, W]; orbital parameters of the simulated orbit.
output:
   (chi_square < p * p( xo * xo + yo * yo + vzo * vzo) ** (0.5)): that is the 
   value of conditions "the distance is  less thant the maximum possible error".

Routine to evalueate the condition to tell of the point mapped satisfy the 
  sought conditions.
*/
_Bool ChiSquareObservablesNPercentError(const double * const param, const double * const x);

/*
input  
  param = [res_x, res_y, res_vz]:
  x = [rmin, f, e, w, i, W, AuxF]; orbital parameters of the simulated orbit.
output:
   Designed to work with ChiSquareObservables, it will use the metric obtained
   for observables to accept the poin under given resolution in each coord.
*/
_Bool ChiSquareObservablesResolution(const double * const param, const double * const x);
_Bool Pass(const double * const param, const double * const x);

///////////////////////////////////////////////////////////////////////////////
// Functions that interact with only  mappings.h 
///////////////////////////////////////////////////////////////////////////////0

/* In order to have MAX_STRING_HEADER spaces in an intuitive way, that is, the 
  number of characters for the name. Inside the program arrays for names that
  are used for the header have  2 additional spaces. In name[MAX_STRING_HEADER + 1] 
  '\n' is saved and in name[MAX_STRING_HEADER + 1] the '/0' special character 
  to indicatewhere it finishes. */
int WriteLine(char * result, const char * const keyword, FILE * ptr_output_file);

///////////////////////////////////////////////////////////////////////////////
// Functions that interact with only input_output.h 
///////////////////////////////////////////////////////////////////////////////
int WriteMapHeaderFromKernels(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info);
int WriteMapConstrainHeaderFromKernels(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info, const char * const out_filename);
int WriteMapConstrainHeader(FILE *ptr_input_file, FILE *ptr_output_file, const char * const constrain_message, 
                            char * aux_func_name, char ** aux_param_names, double * aux_func_param, int n_aux_param, 
                            char * cond_func_name, char ** cond_param_names, double * cond_func_param, int n_cond_param,
                            const char * const out_filename);
int ConstrainMapFile(char * const input_filename, const char * const constrain_message, _Bool (*ConstrainF)(const double * const param, const double * const x), const double * const constrain_param, size_t j0, size_t ns_points);



///////////////////////////////////////////////////////////////////////////////
// Functions that interact with two_body_analytical.h and montecarlo.h
///////////////////////////////////////////////////////////////////////////////

int MontecarloStrcuturesConstrainedSampleGetNPoints(FILE * ptr_output_file,
    Run_info * run_info, Kernel_Info * kernel_info, Constrain_info * constrain_info);

// Default constrani and run_info; kernel defults are a montecarlo matter.
int InitConstrainInfoExample(Constrain_info * constrain_info);
int InitConstrainInfoFromInput(Constrain_info * constrain_info, char * const aux_func_name, int n_param_aux, double * const param_aux, char ** param_aux_names,
                               double (* AuxF) (const double * const param_aux, const double * const x),
                               char * const constrain_func_name, int n_param_constrain, double * const param_constrain, char ** param_constrain_names,
                               _Bool (* ConstrainF) (const double * const param_constrain, const double * const x), char * const constrain_message);
int InitConstrainInfoFromJsonFile(Constrain_info * constrain_info, char * param_filename);
int FreeDefaultConstrainInfo(Constrain_info * constrain_info);
int FreeConstrainInfoComplete(Constrain_info * constrain_info);

int InitRunInfoExample(Run_info * run_info);
int InitRunInfoFromInput(Run_info * run_info, size_t total_config, size_t j0, size_t n_points, char * out_filename, char * methodology);
int InitRunInfoFromJsonFile(Run_info * run_info, char * filename);
int FreeRunInfoComplete(Run_info * run_info);

_Bool IsUniformMappingType(char * param_filename);
int InitUniformMontecarloMappingStructuresFromJson(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info, char * filename);
int FreeUniformMontecarloMappingStructuresComplete(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info);

int MontecarloStrcuturesConstrainedSampleWrite(Mappings_info * mappings_info);
int MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(char * param_file, int file_type);
void * MontecarloStrcuturesConstrainedSampleWriteThread(void * mappings_info);

#endif // _mappings