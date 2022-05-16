#include "GeneratePointFromInputFunctionTest.h"

/*
input
  *param: parameters of the distribution.
  x: N dimensional array in which it will be evaluated. 

result
  evaluation of the gaussian function

This routine calculates the gaussian with parameters param in the point x.
*/
double Gaussian(const double * const param, const double * const x){
  return 1./(sqrt(2 * M_PI) * param[1])* exp(- (x[0]-param[0])*(x[0]-param[0])/(2*param[1]*param[1]) );
}

/*
input
  *param: parameters of the distribution.
  x: N dimensional array in which it will be evaluated. 

result
  evaluation of the gaussian function

This routine calculates the gaussian with parameters param in the point x.
*/
double Double_Gaussian(const double * const param, const double * const x){
  return Gaussian(param, x) * Gaussian(param + 2, x + 1);
}


/* This is an example for the generation of points of a point that can
    corrspond to a general n-dimensional distribution. */
int main(int argc, char const *argv[])
{    
  srand(time(NULL));
  int check_state = 1;
  double point[1];
  double param[2];
  double val_0[2];
  double val_f[2];
  int i = 0;
  double f_max_value;
  int dim = 1;
  FILE * output_file;
  char * file_name = "GeneratePointFromInputFunctionTest_GaussianPoints.txt";
  char command[1000];
  int n_points = 10000; 

  param[0] = 0;
  param[1] = 1;

  val_0[0] = -3;
  val_0[1] = 0;

  val_f[0] = 3;
  val_f[1] = 1;

  f_max_value = 1./(sqrt(M_PI)); 

  output_file = fopen(file_name, "w");

  while (i < n_points)
  {
    GeneratePointFromInputFunction(param, point, Gaussian, val_0, val_f, f_max_value, dim);
    i++;
    fprintf(output_file, "%lf\n", point[0]); 
  }
  
  sprintf(command, "gnuplot -c plotters/plot_simple_hist.plt %s %d -p", file_name, n_points);
  system(command);

  printf("\nPlease check tha graph resembles a gaussian histogram");

  printf("\n****************************** %s *****************************************\n", argv[0]);
  printf("%s pass the test? %d", argv[0], check_state);
  printf("\n****************************** %s *****************************************\n\n", argv[0]);

  fclose(output_file);

  return 0;
}
