#include "mappings.h"

///////////////////////////////////////////////////////////////////////
// Definition of functions to be mapped for some purpose
///////////////////////////////////////////////////////////////////////

double ChiSquareObservables(const double * const param, const double* const x){
  //param == (x,y,vz,mu) observed

  static orbital_Param orbital_param;
  static double v[2*DIM] ={0,0,
			   0,0,
			   0,0}; //vector to be obtained from Obrbital_Parameters_To_State
  
  orbital_param.rmin = x[0];
  orbital_param.f    = x[1];
  orbital_param.e    = x[2];
  orbital_param.w    = x[3];
  orbital_param.i    = x[4];
  orbital_param.W    = x[5];
  OrbitalParametersToStateVector(v, orbital_param, param[3]);
  return sqrt ( (v[0]-param[0])*(v[0]-param[0]) + (v[2]-param[1])*(v[2]-param[1]) + (v[5]-param[2])*(v[5]-param[2]) );

}


//////////////////////////////////////////////////////////////////////////////////////
//// Examples of mappings made for some functions and systems/////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

int AM_2229_735SystemMap(size_t total_config){
  FILE *ptr_output_file;

  size_t j_0=0;
  size_t n_points=1E5;
  int n_var=6;
  const char* variables_names[6] = {"#rmin(min/max)", "#f(min/max)", "#e(min/max)", "#w(min/max)", "#i(min/max)", "#W(min/max)"};

  double var_0[2*DIM] ={3 ,  0    , 0.8, 0     , 0     , 0     };
  double var_f[2*DIM] ={21, 2*M_PI, 1.4, 2*M_PI, M_PI, 2*M_PI};

  double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) )*43007.1;
  double param[4] = {17.95,-17.47,192.0*1.023,mu};

  char filename[500];
  char *method="UniformMontecarloMap";
  sprintf(filename,"data/mappings/AM_2229_735System%s%zuPoints.dat",method,total_config);
  ptr_output_file = fopen(filename,"wb");

  double *variables = (double *) malloc( sizeof(double)* total_config*(n_var+1) );

  WriteMapHeader(filename, method, total_config, variables_names, n_var, var_0, var_f, ptr_output_file);
  MontecarloUniformSampleGetNPoints(total_config, j_0, n_points, n_var, var_0, var_f, variables,ChiSquareObservables,
				    param , ptr_output_file);

  fclose(ptr_output_file);

  return 0;
}


///////////////////////////////////////////////////////////////////////
/// Definition of some input_output functions /////////////////////////
///////////////////////////////////////////////////////////////////////

int WriteLine(char * result, const char * const keyword, FILE * ptr_output_file){
  static char message[MAX_STRING_HEADER-3];

  InitString(result,MAX_STRING_HEADER);
  sprintf(message,"%s",keyword);
  Insert(result, 0, strlen(message), message);
  fprintf(ptr_output_file,"%s",result);

  return 0;
}

int WriteMapHeader(const char* const filename, const char * const method, const size_t n_points, const char ** const variables_names,const int n_var, const double * var_0, const double * var_f, FILE * ptr_output_file){
  fseek(ptr_output_file, 0L, SEEK_SET);

  char dummy[MAX_STRING_HEADER];
  int len=(MAX_STRING_HEADER-1)*2 + 11*2 + 5*(MAX_STRING_HEADER-1) + 21 + (MAX_STRING_HEADER-1) + 11 + n_var*(MAX_STRING_HEADER-1) + 17*n_var*2;

  WriteLine(dummy,"#FixHeaderLen(bytes)----------",ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", len);
  
  WriteLine(dummy,"#FixNumberOfLines----------",ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", 12+n_var*3);

  
  WriteLine(dummy,"#Namefile----------",ptr_output_file);
  WriteLine(dummy,filename,ptr_output_file);

  
  WriteLine(dummy,"#Method------------", ptr_output_file);
  WriteLine(dummy,method,ptr_output_file);

  
  WriteLine(dummy,"#Npoints-----------", ptr_output_file);
  fprintf(ptr_output_file,"%20zu\n", n_points);

  WriteLine(dummy,"#Nvar--------------", ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", n_var);

  
  for(int i=0; i<n_var;i++){
    WriteLine(dummy,variables_names[i],ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", var_0[i]);
    fprintf(ptr_output_file,"%16.15g\n", var_f[i]);
  }
  //  fseek(ptr_output_file,len,SEEK_SET); 
  //  fprintf(ptr_output_file,"%s","Here\n");

  return 0;
}


int WriteMapConstrainHeader(FILE *ptr_input_file, FILE *ptr_output_file, const char * const constrain_message, const char * const out_filename){
  // Getting size of header
  int bytes_header_size;
  int n_lines;
  
  ValueInLineN(ptr_input_file, "%d" ,&bytes_header_size, 2); 
  ValueInLineN(ptr_input_file, "%d" ,&n_lines, 4); 
  
  // copying header to constained one
  char * header_string = malloc( sizeof(char) * bytes_header_size ); 
  fseek(ptr_input_file,0L,SEEK_SET);

  fread(header_string, sizeof(char), bytes_header_size, ptr_input_file);

  fseek(ptr_output_file,0L,SEEK_SET); // in case same file is used
  fwrite(header_string, sizeof(char), bytes_header_size, ptr_output_file);

  char dummy[MAX_STRING_HEADER];
  WriteLine(dummy,"#ConstrainMessage----------",ptr_output_file);
  WriteLine(dummy,constrain_message,ptr_output_file);

  // Modify all the variables that changed
  
  bytes_header_size += (MAX_STRING_HEADER-1)*2;
  n_lines +=2; 

  GoToFileLine(ptr_output_file,2);
  fprintf(ptr_output_file,"%10d\n", bytes_header_size);
  
  GoToFileLine(ptr_output_file,4);
  fprintf(ptr_output_file,"%10d\n", n_lines);

  fseek(ptr_output_file,bytes_header_size,SEEK_SET); // Go to last postion
  
  return 0; 
}

///////////////////////////////////////////////////////////////////////////////
/// Definition of routines to constrain data in a file/////////////////////////
///////////////////////////////////////////////////////////////////////////////

int ConstrainMapFile(char * const input_filename, const char * const constrain_message, _Bool (*ConstrainF)(const double * const param, const double * const x), const double * const constrain_param, size_t j0, size_t ns_points){

  int nvar, ndim, byte_size_header_in, byte_size_header_out, i_var;
  size_t total_config, j_con, n_save=0, new_total_config=0;
  char output_filename[100]; 
  
  sprintf(output_filename,"%s%s.dat",input_filename,"Constrained"); 
  sprintf(input_filename,"%s.dat",input_filename); 
  printf("%s %zu\n", input_filename, strlen(input_filename) );
  printf("%s %zu\n", output_filename, strlen(output_filename) );

  FILE * ptr_input_file =ReadFileName(input_filename,"rb");
  FILE * ptr_output_file=ReadFileName(output_filename,"wb+"); 
  
  WriteMapConstrainHeader(ptr_input_file, ptr_output_file, constrain_message, output_filename);

  ValueInLineN(ptr_input_file, "%d" ,&byte_size_header_in, 2);
  ValueInLineN(ptr_output_file, "%d" ,&byte_size_header_out, 2);
  ValueInLineN(ptr_input_file, "%zu" ,&total_config, 10);
  ValueInLineN(ptr_input_file, "%d" ,&nvar, 12);
  ndim = nvar + 1;
  double * x = (double *) malloc( ndim*sizeof(double) );
  double * var_to_save = (double *) malloc(ndim*nvar*sizeof(double) );

  fseek(ptr_input_file,byte_size_header_in,SEEK_SET); 
  fseek(ptr_output_file,byte_size_header_out,SEEK_SET); 
  
  for(j_con = j0; j_con < total_config; j_con++){

    fread(x,sizeof(x[0]),ndim,ptr_input_file); 
    
    if ( ConstrainF(constrain_param,x) ){
      for (i_var=0; i_var<ndim; i_var++)
	var_to_save[i_var+n_save*ndim] = x[i_var];  
      n_save++; 
    }

    if(n_save==ns_points){
      fwrite(var_to_save, sizeof(var_to_save[0]), ndim*n_save, ptr_output_file );
      new_total_config += n_save; 
      n_save=0;
    }

    
  }

  new_total_config += n_save;
  fwrite(var_to_save, sizeof(var_to_save[0]), ndim*n_save, ptr_output_file ); 

  //// Writing in the file the total of configurations 
  GoToFileLine(ptr_output_file,10);
  fprintf(ptr_output_file,"%20zu\n", new_total_config );

  fclose(ptr_input_file);
  fclose(ptr_output_file);
  
  return 0; 
}


///////////////////////////////////////////////////////////////////////////////
///   Definition of functions to constrain            /////////////////////////
///////////////////////////////////////////////////////////////////////////////
_Bool ChiSquareObservablesNPercentError(const double * const param, const double * const x){

  static double result; 
  static double d_value; 
  
  d_value = param[4]*param[4] * sqrt( param[0]*param[0] + param[1]*param[1] + param[2]*param[2] );
  //printf("%lf",x[6]); 
  result = x[6]<d_value ? 1: 0;
  
  return result; 
} 
