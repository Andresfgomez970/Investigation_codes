#include "two_body_analytical.h"
#include "input_output.h"
#include "montecarlo.h"

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


///////////////////////////////////////////////////////////////////////////////
// Functions that interact with only  mappings.h 
///////////////////////////////////////////////////////////////////////////////0

/* In order to have MAX_STRING_HEADER spaces in an intuitive way, that is, the 
  number of characters for the name. Inside the program arrays for names that
  are used for the header have  2 additional spaces. In name[MAX_STRING_HEADER + 1] 
  '\n' is saved and in name[MAX_STRING_HEADER + 1] the '/0' special character 
  to indicatewhere it finishes. */
#define MAX_STRING_HEADER 100 // In memory will indicate 102 size

int WriteLine(char * result, const char * const keyword, FILE * ptr_output_file);
int WriteMapHeader(const char* const filename, const char * const method, const size_t n_points, const char ** const variables_names, const int n_var, const double * var_0, const double * var_f, FILE * ptr_output_file);

///////////////////////////////////////////////////////////////////////////////
// Functions that interact with only input_output.h 
///////////////////////////////////////////////////////////////////////////////

int WriteMapConstrainHeader(FILE * ptr_input_file, FILE * ptr_output_file, 
							const char * const constrain_message,
							const char * const out_filename);

int ConstrainMapFile(char * const input_filename, const char * const constrain_message, _Bool (*ConstrainF)(const double * const param,const
													    double * const x) , const double * const constrain_param, size_t j0, size_t ns_points);


///////////////////////////////////////////////////////////////////////////////
// Functions that interact with two_body_analytical.h and montecarlo.h
///////////////////////////////////////////////////////////////////////////////

/* This function make a map of the AM_2229_735 system with function 
     MontecarloUniformSampleGetNPoints in montecarlo.h */
int AM_2229_735SystemMap(size_t total_config);
