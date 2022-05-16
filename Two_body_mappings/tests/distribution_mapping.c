#include "../montecarlo.c"
#include <math.h>
#include <time.h>

double Gaussian(double *param, double *x){
  return 1./(sqrt(2 * M_PI) * param[1])* exp(- (x[0]-param[0])*(x[0]-param[0])/(2*param[1]*param[1]) );
}

int main()
{    
  srand(time(NULL));
  double point[1];
  double param[2];
  double var_0[2];
  double var_f[2];
  int i = 0;

  param[0] = 0;
  param[1] = 1;

  var_0[0] = -3;
  var_0[1] = 0;

  var_f[0] = 3;
  var_f[1] = 1;

  int dim = 1;

  FILE * output_file;

  output_file = fopen("test.txt", "w");

  while (i < 10000)
  {
    GeneratePointFromInputFunction(point, Gaussian, param, var_0, var_f, dim);
    i++;
    printf("%d %lf\n",i, point[0]);
    fprintf(output_file, "%lf\n", point[0]); 
  }
  

  return 0;
}
