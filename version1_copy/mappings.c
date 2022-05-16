#include "mappings.h"

// This is provitional solution to the Visual Studio error
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

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


///////////////////////////////////////////////////////////////////////////////
///   Definition of functions to constrain for  posterior /////////////////////
///////////////////////////////////////////////////////////////////////////////

_Bool ChiSquareObservablesNPercentError(const double * const param, const double * const x){

  static double result; 
  static double d_value; 
  
  d_value = param[4]*param[4] * sqrt( param[0]*param[0] + param[1]*param[1] + param[2]*param[2] );
  //printf("%lf",x[6]); 
  result = x[6]<d_value ? 1: 0;
  
  return result; 
} 

//////////////////////////////////////////////////////////////////////////////////////
//// Examples of mappings made for some functions and systems ////////////////////////
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
/*
input: 
  result: pointer in which the result of the routine is saved.
  keyword: word to b written in the first part of result.
  ptr_ouput_file: file in which this is t bw written.
 
 This routine write a string with a very carfully defined format, which is
  MAX_STRING_HEADER characters and then a '\n' and finally a '\0'.
The idea of  this is to use it in order to do a header that is in asccii format
  and the each line has a very well defined forma that can make the header
  constant.
*/
int WriteLine(char * result, const char * const keyword, FILE * ptr_output_file){
  // Create array of chars of given size, this is maximum size of input message
  static char message[MAX_STRING_HEADER];
  // This ise the real size of the ouput message for the header, two additional
  //  spaces are added to result, in order to save '\n' and '\0'.
  int real_number_of_chars = MAX_STRING_HEADER + 2;

  // Init blank sting in result with size real_number_of_chars, last two indexes
  //   refer to '\n' and '\0' 
  InitBlankString(result, real_number_of_chars);
  // Make the keyword an input message of the size desired of the size desired
  sprintf(message, "%s", keyword);
  // Insert the message in the result string to be printed in the header
  Insert(result, MAX_STRING_HEADER, 0, strlen(message), message);
  // Output the result string in the header
  fprintf(ptr_output_file,"%s",result);

  return 0;
}

/*
input:
  filename: name of file in which the header is to be witten.
  method: name of th method of the type of mapping made.
  n_points: number of points to be mapped.
  variable_names: name of the variables to be mapped, in same order than are
    expected to be save or already saved.
  n_var: number of variables to be mapped.
  var_0: initial values in th ranges of the variables to be mapped.
  var_f: final   values .....
  ptr_ouput_file: File in which the header is desired to be saved
result:
  header of the type of mapping to be made.
*/
int WriteMapHeader(const char* const filename, const char * const method, const size_t n_points, 
                   const char ** const variables_names, const int n_var, const double * var_0, 
                   const double * var_f, FILE * ptr_output_file){
  // Go to first line of file, this is done becuse changes can be made.
  fseek(ptr_output_file, 0L, SEEK_SET);

  // Pointer of character to save the result to be printed in each line of
  //   header.
  char dummy[MAX_STRING_HEADER];
  // Size of titles in header; e. g. ~ #FixHeaderLen(bytes)----------     \n\0
  int real_str_size = MAX_STRING_HEADER + 2;
  // Size of the header in bytes, note that the '\0' is not included in the sum
  //  thanks to the fact that it reflects lack of character in asscii, this 
  //  '\0' was defined convenience inside the program.

  real_str_size -= 1;
  // The sum can be decomposed as follows:
  //        len of header / int  + N_lines header / int + filename / str
  //        mapping method / str + points_mapped / zu   + Nvar_mapped / int
  //        var_names / n_var str         + mapping intervals with 16 significant figures.
  int len = (real_str_size + 11) + (real_str_size + 11) + real_str_size  *  2 +
            real_str_size  *  2  + (real_str_size + 21) + (real_str_size + 11) +
            real_str_size * n_var  + 17 * n_var * 2;
  real_str_size += 1; // For calculations '\0' is  taken into account.

  ////////////////////////////////////////////////////////////////////
  // note that for string the number of that can contain each one is 
  //  MAX_STRING_HEADER + 2, the last 2 characters are fixed.
  // also the significan figures of a intger in the best case are 10, for a zu
  // are 20, and for doubles 16; each time one of this is written a space is 
  // left for a \n character.
  WriteLine(dummy, "#FixHeaderLen(bytes)----------", ptr_output_file);
  fprintf(ptr_output_file, "%10d\n", len);
  
  WriteLine(dummy, "#FixNumberOfLines----------", ptr_output_file);
  fprintf(ptr_output_file, "%10d\n", 12 + n_var * 3);

  WriteLine(dummy, "#Namefile----------", ptr_output_file);
  WriteLine(dummy, filename, ptr_output_file);

  WriteLine(dummy, "#Method------------", ptr_output_file);
  WriteLine(dummy, method, ptr_output_file);

  WriteLine(dummy,"#Npoints-----------", ptr_output_file);
  fprintf(ptr_output_file,"%20zu\n", n_points);

  WriteLine(dummy,"#Nvar--------------", ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", n_var);

  for(int i=0; i<n_var;i++){
    WriteLine(dummy, variables_names[i], ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", var_0[i]);
    fprintf(ptr_output_file,"%16.15g\n", var_f[i]);
  }
  ////////
  
  //  fseek(ptr_output_file,len,SEEK_SET); 
  //  fprintf(ptr_output_file,"%s","Here\n");

  return 0;
}

/*
This routine will write the header of the output file along with the type of 
  constrain that was applied.
*/
int WriteMapConstrainHeader(FILE *ptr_input_file, FILE *ptr_output_file, const char * const constrain_message, const char * const out_filename){
  // Getting size of header
  int bytes_header_size;
  int n_lines;
  
  // Reading size of input header and its number of lines
  ValueInLineN(ptr_input_file, "%d" ,&bytes_header_size, 2); 
  ValueInLineN(ptr_input_file, "%d" ,&n_lines, 4); 
  
  // Copying input header to constained one
  char * header_string = malloc( sizeof(char) * bytes_header_size ); 
  fseek(ptr_input_file,0L,SEEK_SET);
  fread(header_string, sizeof(char), bytes_header_size, ptr_input_file);

  // Write the header without being changed
  fseek(ptr_output_file,0L,SEEK_SET);
  fwrite(header_string, sizeof(char), bytes_header_size, ptr_output_file);

  // Finally, write the cosntrained messages and modify the size of the header
  //   and the number of lines to be changed.
  char dummy[MAX_STRING_HEADER];
  WriteLine(dummy,"#ConstrainMessage----------",ptr_output_file);
  WriteLine(dummy,constrain_message,ptr_output_file);

  // Modify all the variables that changed and write them in new constrained
  //   file.
  bytes_header_size += (MAX_STRING_HEADER-1)*2;
  n_lines +=2; 

  GoToFileLine(ptr_output_file,2);
  fprintf(ptr_output_file,"%10d\n", bytes_header_size);
  
  GoToFileLine(ptr_output_file,4);
  fprintf(ptr_output_file,"%10d\n", n_lines);

  fseek(ptr_output_file,bytes_header_size,SEEK_SET); 
  // Go to last postion to later write the ponits.
  
  return 0; 
}

///////////////////////////////////////////////////////////////////////////////
/// Definition of routines to constrain data in a file/////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
This function will apply the constrain directly over a file where the map
  function and its respective points has been saved. 
*/
int ConstrainMapFile(char * const input_filename, const char * const constrain_message, _Bool (*ConstrainF)(const double * const param, const double * const x), const double * const constrain_param, size_t j0, size_t ns_points){
  //////////////////////////////////
  //// Variables used description //
  //////////////////////////////////
  
  // nvar: number of free variables of the function to be mapped; in f(x1,x2) 
  //    will be 3.
  
  // ndim: number of dimensions of the function, for f(x1,x2) will be three.
  
  // byte_size_header_in, byte_size_header_out: header sizes of the mappings, 
  //  it will be used to skip this part and write the real data i binary.

  // i_var: counter to go over the points to be saved, in the above example
  //  x1, x2, f; so it will go between 0-2.
  int nvar, ndim, byte_size_header_in, byte_size_header_out, i_var;
  // total_config: total of points x1,..,xn,f mapped initially
  // j_con: indicates until what number has been saved in a random number
  //   sequence
  // n_save: number of points to be saved once all of them satisfy ConstrainF
  // new_total_config: new number of points mapped under the ConstrainF
  //   condition
  size_t total_config, j_con, n_save=0, new_total_config=0;
  char output_filename[100]; 
  
  // Obtain the names of the input and ouput file.
  sprintf(output_filename,"%s%s.dat",input_filename,"Constrained"); 
  sprintf(input_filename,"%s.dat",input_filename); 
  printf("%s %zu\n", input_filename, strlen(input_filename) );
  printf("%s %zu\n", output_filename, strlen(output_filename) );

  // Open both files, the input one to constult the the header and change it in
  //    then new mapped file.
  FILE * ptr_input_file =ReadFileName(input_filename,"rb");
  FILE * ptr_output_file=ReadFileName(output_filename,"wb+"); 
  
  // Write constrained header.
  WriteMapConstrainHeader(ptr_input_file, ptr_output_file, constrain_message, output_filename);

  // Read variables in initial mapping unconstrained
  ValueInLineN(ptr_input_file, "%d" ,&byte_size_header_in, 2);
  ValueInLineN(ptr_input_file, "%zu" ,&total_config, 10);
  ValueInLineN(ptr_input_file, "%d" ,&nvar, 12);

  // Read header size, and define variable to save a point and one to save
  //  each n_points.
  ValueInLineN(ptr_output_file, "%d" ,&byte_size_header_out, 2);
  ndim = nvar + 1;
  double * x = (double *) malloc( ndim*sizeof(double) );
  double * var_to_save = (double *) malloc(ndim*nvar*sizeof(double) );
  
  // Skip the headers and being to read, verify condition, and write point by
  //  point.
  fseek(ptr_input_file,byte_size_header_in,SEEK_SET); 
  fseek(ptr_output_file,byte_size_header_out,SEEK_SET); 
  
  for(j_con = j0; j_con < total_config; j_con++){

    fread(x,sizeof(x[0]),ndim,ptr_input_file); 
    
    if ( ConstrainF(constrain_param,x) ){
      for (i_var=0; i_var<ndim; i_var++)
	var_to_save[i_var+n_save*ndim] = x[i_var];  
      n_save++; 
    }

    // Save each n_points
    if(n_save==ns_points){
      fwrite(var_to_save, sizeof(var_to_save[0]), ndim*n_save, ptr_output_file );
      new_total_config += n_save; 
      n_save=0;
    }

    
  }

  // Write las points that likely did not reach the n_points and 
  //  calculate the total of points in the constrained case to save this
  //  in the header.
  new_total_config += n_save;
  fwrite(var_to_save, sizeof(var_to_save[0]), ndim*n_save, ptr_output_file ); 

  //// Writing in the file the total of configurations 
  GoToFileLine(ptr_output_file,10);
  fprintf(ptr_output_file,"%20zu\n", new_total_config );

  // Close alls
  fclose(ptr_input_file);
  fclose(ptr_output_file);
  
  return 0; 
}
