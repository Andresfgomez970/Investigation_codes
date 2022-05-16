#include "allvars.h"

void AM_2229_735SystemConstrainMap(size_t total_config){
  char input_filename[100];
  sprintf(input_filename,"data/mappings/AM_2229_735SystemUniformMontecarloMap%zuPoints",total_config);
  char constrain_message[100] = "Tenth percent of error";
  double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) )*43007.1;
  double p = 0.1; 
  double constrain_param[5] = {17.95,-17.47,192.0*1.023,mu,p};
  size_t j0=0;
  size_t ns_points=1E6;
  
  ConstrainMapFile(input_filename, constrain_message, ChiSquareObservablesNPercentError, constrain_param, j0, ns_points);
}

void Quiroga_system(){
  FILE *ptr_output_file;
  size_t j_0=0;
  size_t n_points=1E3;  //Total number of points to be mapped
  size_t ns_points=1E3; //When this number of points is reached, then saved
  int n_dim=7;
  const char* variables_names[6] = {"#rmin(min/max)", "#f(min/max)", "#e(min/max)", "#w(min/max)", "#i(min/max)", "#W(min/max)"};

  double var_0[2*DIM] ={3 ,  0    , 0.3, 0     , 0     , 0   };
  double var_f[2*DIM] ={21, 2*M_PI, 5, 2*M_PI, M_PI, 2*M_PI};

  double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) )*43007.1;
  /// Observational values
  double xval   =  17.95;
  double yval   =  -17.47;
  double vzval  =  192*1.023;
  double xplus =  0;
  double yplus =  0;
  double vzplus=  0; 

  for (vzplus = 0; vzplus<1; vzplus=1){ 
      
    double param[4] = {xval,yval,vzval+vzplus,mu};
    double param_constrain[5] = {xval,yval,vzval+vzplus,mu,0.1};
    
    char filename[500];
    char *method="UniformConstrainedMontecarloMap";
    char constrain_message[500];
    sprintf(constrain_message,"Tenth percent error / Plus: X%d Y%d VZ%d", (int) xplus,  (int) yplus,  (int) vzplus); 
    
    //  sprintf(filename,"data/mappings/AM_2229_735SystemPlusX%dY%dVZ%d%s%zuPoints.dat", (int) xplus, (int) yplus, (int) vzplus,
    //method, n_points);
    sprintf(filename,"data/mappings/AM_2229_735SystemCompareL%s%zuPoints.dat", method, n_points);
    
    
    ptr_output_file = fopen(filename,"wb+");
    
    double *variables = (double *) malloc( sizeof(double)*n_points*n_dim );
    
    WriteMapHeader(filename, method, n_points, variables_names, n_dim-1, var_0, var_f, ptr_output_file);
    WriteMapConstrainHeader(ptr_output_file, ptr_output_file, constrain_message, filename);  
    MontecarloUniformConstrainedSampleGetNPoints(n_points,j_0,ns_points, n_dim, var_0, var_f, variables, ChiSquareObservables, param ,
						 ptr_output_file, ChiSquareObservablesNPercentError,param_constrain); 

  
    
  }
  
  
}

void Laura_system(){
  FILE *ptr_output_file;
  size_t j_0=0;
  size_t n_points=5E2; 
  size_t ns_points=1E5;
  int n_dim=7;
  const char* variables_names[6] = {"#rmin(min/max)", "#f(min/max)", "#e(min/max)", "#w(min/max)", "#i(min/max)", "#W(min/max)"};

  double var_0[2*DIM] ={3 ,  0    , 0.4, 0     , 0     , 0   };
  double var_f[2*DIM] ={21, 2*M_PI, 5., 2*M_PI, M_PI, 2*M_PI};

  // double vz0 = 192*0.5; 
  // double x0 =  17.95;
  //double y0 =  -17.47; 
    
  double mu = ( (190+1.5+0.15+0.47) + (14.3+0.56+0.056+0.168)  )*43007.1;
  //double param[4] = {x0,y0,vz0*1.023,mu};
  //double param_constrain[5] = {x0,y0,vz0*1.023,mu,0.1};
  double param[4] = {-109.899140 , -523.171612 , 358.962868*1.023 ,mu};
  double param_constrain[5] = {-109.899140 , -523.171612 , 358.962868*1.023, mu, 0.1};
  
  char filename[500];
  char *method="UniformConstrainedMontecarloMap";
  char *constrain_message="Tenth percent error"; 
  sprintf(filename,"data/mappings/LauraSystem%s%zuPoints.dat",method, n_points);
  ptr_output_file = fopen(filename,"wb+");

  double *variables = (double *) malloc( sizeof(double)*n_points*n_dim );

  WriteMapHeader(filename, method, n_points, variables_names, n_dim-1, var_0, var_f, ptr_output_file);
  WriteMapConstrainHeader(ptr_output_file, ptr_output_file, constrain_message, filename);  
  MontecarloUniformConstrainedSampleGetNPoints(n_points,j_0,ns_points, n_dim, var_0, var_f, variables, ChiSquareObservables, param ,
  					       ptr_output_file, ChiSquareObservablesNPercentError,param_constrain); 
  
}


int main(int argc, char *argv[]){

  Quiroga_system(); 

  /*
  size_t total_config = 1; 
  AM_2229_735SystemMap(total_config);
  AM_2229_735SystemConstrainMap(total_config);  
  */
  
  return 0; 
}
