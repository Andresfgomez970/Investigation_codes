#include "wetzel.h"

/*  This code is intended to do the following: will suppose a distribution for 
  the Euler angles, true anomaly, and periapsis. For this, a point will be 
  generated with GeneratePointFromInputFunction. Then it will pass the 
  obtained point to the phase space, which will represent the fiducual data.

Later, it will identify all possible orbital parameters with 
  MontecarloUniformConstrainedSampleGetNPoints, then save 
  them and repeat until a fit can be identified in the parameters.

The idea in this is to evaluate the consistency of the algorithm and a possible 
  bias it could have doing the mapping, thanks to observational error for 
  example. This is overall a very creative way to adjust the "how near function 
  that we are in fact supposing to be 10 percent".
*/

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Example 1 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void UniformDefaultTest(){
  // Define the structures that constanins all the info for the mapping
  Kernel_Info kernel_info;
  Constrain_info constrain_info;
  Run_info run_info;

  InitConstantKernelExample(&kernel_info);
  InitConstrainInfoExample(&constrain_info);
  InitRunInfoExample(&run_info);

  FILE * ptr_file; 
  ptr_file = fopen("Defualt.dat", "wb+");

  MontecarloStrcuturesConstrainedSampleGetNPoints(ptr_file, run_info, 
      kernel_info, constrain_info);

  fclose(ptr_file);
}

int main()
{ 
  // UniformDefaultTest();
  return 0;
}
