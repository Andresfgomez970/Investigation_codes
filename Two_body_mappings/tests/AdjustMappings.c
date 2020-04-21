#include <stdio.h>
#include <gsl/gsl_multifit.h>

// Structure of only one function in order to do an array of strucutures and 
//  have a kernel or a set of function that do something in general
typedef struct{

  double (*f) (double * variables);
  
}Function;

// Function to verify that the general pointer is well readed
FILE* ReadFileName(const char * const filename,const char * const readmode); 
// Function to allocate the variables to solve the problem
int AllocateForLeastSquare(gsl_matrix **X, gsl_matrix **cov, gsl_vector **y, gsl_vector **w, gsl_vector **c, int n_par, int n_var, size_t n_points);
// Function to define the kernel to be used in the problem
int DefineGaussianKernel(Function **kernel); 
int DefineParaboloidKernel(Function **kernel);
// Fill the data in the binary file to the variables used in gsl for the adjustmetn
int FillModel(gsl_matrix *X, gsl_vector *y, gsl_vector *w, int n_var, int n_par, size_t n_points,Function *kernel, FILE *input_stream);
// Functions to print the results 
int Printgslvector(const gsl_vector * const C, const int columns); 
int Printgslmatrix(const gsl_matrix *const COV, const int rows,const int columns);
// Function to solve the problem in general and print the results: chisqr, cov, c
int LeastSquare(int (*SetKernelFunction)(Function **Kernel), int n_var ,int n_par, int n_points, const char * const namefile,  const char * const readmode); 

// Like in line functions; inline not working to construct kernel (check)
double Constant(double *variables){
  return 1; 
}

double Normal(double *variables){
  return exp( -(variables[0])*(variables[0])/2. );
} 

double Paraboloid(double *variables){
  return (variables[0]*variables[0] + variables[1]*variables[1]); 
}

//// Examples to be adjusted
void GaussianAdjustmentExamples(); 
void ParaboloidAdjustmentExamples();

int main(){  

  GaussianAdjustmentExamples();

  printf("%s", "-----------------------------------------------------------------------------------------------------------------\n\n");
  printf("%s", "-----------------------------------------------------------------------------------------------------------------\n\n");
  printf("%s", "-----------------------------------------------------------------------------------------------------------------\n\n");
  printf("%s", "-----------------------------------------------------------------------------------------------------------------\n\n\n\n");
  
  ParaboloidAdjustmentExamples(); 
  
  return 0; 
}

FILE * ReadFileName(const char * const filename, const char * const readmode){

  FILE * stream = fopen(filename,readmode);
  
  if (stream == NULL){
    printf("It was not possible to read %s\n",filename);
    exit(-1);    
  }
  
  return stream; 
}


int AllocateForLeastSquare(gsl_matrix **X, gsl_matrix **cov, gsl_vector **y, gsl_vector **w, gsl_vector **c, int n_par, int n_var, size_t n_points){ 
  *X = gsl_matrix_alloc (n_points, n_par);
  *y = gsl_vector_alloc (n_points);
  *w = gsl_vector_alloc (n_points);
  
  *c = gsl_vector_alloc (n_par);
  *cov = gsl_matrix_alloc (n_par, n_par);
  
  return 0; 
}



int FillModel(gsl_matrix *X, gsl_vector *y, gsl_vector *w, int n_var, int n_par, size_t n_points, Function *kernel ,FILE * input_stream){
  int count,i_par; 
  size_t i;
  double variables[n_var+1]; 
  
  for (i = 0; i < n_points; i++){

    count = fread(variables, sizeof(double), n_var+1, input_stream); //here reading variables and their result    

    if (count != n_var+1){
      fprintf (stderr, "error reading file\n");
      exit (-1);
    }
    
    for(i_par=0; i_par<n_par; i_par++){
      gsl_matrix_set (X, i, i_par, kernel[i_par].f(variables) );
    }
 
    gsl_vector_set (y, i, variables[n_var]);
    gsl_vector_set (w, i, 1.0); //The errors are by the moment set to one by default
  }
  
  return 0; 
}


int DefineParaboloidKernel(Function **kernel){

  *kernel = (Function *) malloc( sizeof(Function) * 2); 
  (*kernel)->f = Constant;
  (*kernel+1)->f  = Paraboloid;
  return 0;
}

//Memory is allocated to the pointer of kernel, therefore, this one is passed by referece
int DefineGaussianKernel(Function **kernel){
  
  //Defining some kernel of functions for the adjustement
  *kernel = (Function *) malloc( sizeof(Function) * 2); 
  (*kernel)->f = Constant;
  (*kernel+1)->f  = Normal; 

  return 0; 
} 

int Printgslmatrix(const gsl_matrix *const cov, const int rows, const int columns){
  int i, j;
  printf("%s\t\n","{");
  for (i=0; i<rows; i++){
    for(j=0; j<columns; j++){
      printf("\t%+.5e", gsl_matrix_get(cov,i,j) );
    }
    printf("%s","\n");
  }
  printf("%s","\t}\n");
  return 0;
}

int Printgslvector(const gsl_vector *const vector, const int columns){
  int i;
  printf("%s","\t{\n");
  for(i=0; i<columns; i++){
    printf("\t%lf ", gsl_vector_get(vector,i) );
  }
  printf("%s","\n\t}\n");
  
  return 0;
}


int LeastSquare(int (*SetKernelFunction)(Function **Kernel), int n_var ,int n_par, int n_points, const char * const namefile,  const char * const readmode){
  // Variables to be changes that are problem depenedent  
  Function *kernel; 
  FILE *input_stream;

  double chisq=0;
  gsl_matrix *X, *cov; 
  gsl_vector *y, *w, *c;
   
  //////// Reading data
  input_stream = ReadFileName(namefile,readmode);  //note that the data in gaussian10.dat is readed in general

  //////// Allocating memory
  AllocateForLeastSquare(&X, &cov, &y, &w, &c, n_par, n_var, n_points);

  // Defining  the kernel to be used
  SetKernelFunction(&kernel); 
  
  //////// Reading data to be adjusted
  FillModel(X, y, w, n_var, n_par, n_points, kernel,input_stream);
  fclose(input_stream); 
  
  //////// Fitting the data
  gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (n_points, n_par);
  gsl_multifit_wlinear (X, w, y, c, cov, &chisq, work);
  gsl_multifit_linear_free (work);

  //////// Printing the results
  printf ("# covariance matrix:\n");
  Printgslmatrix(cov,n_par,n_par);

  printf("# vector of parameters:\n");
  Printgslvector(c,n_par); 
  
    
  printf ("# chisq = %g\n", chisq);


  /////// Deallocating memory
  gsl_matrix_free (X);
  gsl_vector_free (y);
  gsl_vector_free (w);
  gsl_vector_free (c);
  gsl_matrix_free (cov);

  
  return 0; 
}


void GaussianAdjustmentExamples(){
  int n_par=2; //number of parameters that are to be determined
  int n_var=1; //number of varibales of which depends the model to be adjusted
  char name[400];
  size_t n_points; //number of points that were mapped to be adjusted
  
  for(n_points=10; n_points<101; n_points+=10){
    printf("%s", "---------------------------------------------------------\n"); 
    printf("%s","\t\tModel: c0 + c1 exp(-x*x/2)\n");
    printf("%s", "\t\tExpected values: c0=0, c1=1\n"); 
    printf("\t\tFor n_points = %zu\n",n_points);
    sprintf(name,"data/Mappings/gaussian%zu.dat",n_points); 
    LeastSquare(DefineGaussianKernel,n_var,n_par,n_points,name,"rb");
    printf("%s", "----------------\n\n\n\n\n"); 
  }
  
}

void ParaboloidAdjustmentExamples(){
  int n_par=2; //number of parameters that are to be determined
  int n_var=2; //number of varibales of which depends the model to be adjusted
  char name[400];
  size_t n_points; //number of points that were mapped to be adjusted
  
  for(n_points=50; n_points<201; n_points+=50){
    printf("%s", "---------------------------------------------------------\n");
    printf("%s","\t\tModel: c0 + c1 (x*x + y*y) \n");
    printf("%s", "\t\tExpected values: c0=16, c1=-1\n");
    printf("\t\tFor n_points = %zu\n",n_points);
    sprintf(name,"data/Mappings/paraboloid%zu.dat",n_points);
    LeastSquare(DefineParaboloidKernel,n_var,n_par,n_points,name,"rb");
    printf("%s", "----------------\n\n\n\n\n");
  }  

  
}
