#include "two_body_analytical.h"
#include "input_output.h"
#include "montecarlo.h"
  
///////////////////////////////////////// Functions that interact with two_body_analytical.h //////////////////////////////////

//// Function that calculates the "distance" between observables and vector state modelated
// ------- Input ---------
// param = [x_o, y_o, vz_0, mu]; where o states for observed
// x = [rmin,f,e,w,i,W]
// ------- output -------
// d = sqrt( x**2 + y**2 + vz**2 ); where x=xo-xm, .., vz = vzo-vzm ; where m stated for modeled
double ChiSquareObservables(const double * const param, const double * const x); 


///////////////////////////////////////// Functions that interact with only  mappings.h /////////////////////////////////////////

//// Functions to evalueate conditions
// ------- Input ---------
// param = [x_o, y_o, vz_0, mu, p]; where o states for observed an p is the relative error;  mu is given at moment to future implementations
// x = [rmin,f,e,w,i,W, chi_square];  chi_square is x[0:-2] and param evaluated in ChiSquareObservables

// ------- output -------
// 1 if chi_square < p*p(xo*xo+yo*yo+vzo*vzo)**(0.5); that is the distance is less thant the maximum possible error
_Bool ChiSquareObservablesNPercentError(const double * const param, const double * const x);


/////////////////////////////////////////  Functions that interact with any input_output.h //////////////////////////////////////// 
#define MAX_STRING_HEADER 101 //In order to have 80 spaces when delcared as a[MAX_STRING_HEADER]; last one reserved for \0

int WriteLine(char * result, const char * const keyword, FILE * ptr_output_file);
int WriteMapHeader(const char* const filename, const char * const method, const size_t n_points, const char ** const variables_names, const int n_var, const double * var_0, const double * var_f, FILE * ptr_output_file);

///////////////////////////////////////// Functions that interact with input_output.h /////////////////////////////////////////

int WriteMapConstrainHeader(FILE * ptr_input_file, FILE * ptr_output_file, const char * const constrain_message, const char * const
			    out_filename);

int ConstrainMapFile(char * const input_filename, const char * const constrain_message, _Bool (*ConstrainF)(const double * const param,const
													    double * const x) , const double * const constrain_param, size_t j0, size_t ns_points);


///////////////////////////////////////// Functions that interact with two_body_analytical.h and montecarlo.h //////////////////////////////
////  This function make a map of the AM_2229_735 system with function MontecarloUniformSampleGetNPoints in montecarlo.h
int AM_2229_735SystemMap(size_t total_config);
