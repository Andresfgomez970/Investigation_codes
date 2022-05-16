#include <../montecarlo.c>

#include <../mappings.c>
#include <../input_output.c>
#include <../two_body_analytical.c> // things in mapping use this

_Bool  Constrain1DFInterval(const double * const param, const double * const x); 

int main(){
  char input_filename[MAX_STRING_HEADER];
  char out_filename[MAX_STRING_HEADER];
  sprintf(input_filename,"%s","data/Mappings/gaussian10");
  sprintf(out_filename,"%sConstraned.dat",input_filename); 

  char * constrain_message = "-3<x<3";
  double constrain_param[2] = {-3,3};
  size_t ns_points = 1E5; 
  size_t j0 = 0; 

  ConstrainMapFile(input_filename, constrain_message, Constrain1DFInterval, constrain_param, j0, ns_points); 
  
  return 0; 
}


_Bool Constrain1DFInterval(const double * const constrain_param, const double * const x){
  return ( x[0]>constrain_param[0] && x[0]<constrain_param[1]) ?  1:0; 
}
