#include "mappings.h"

Double_Funct_Element * double_func_mappings_table[FUNCTIONHASHSIZE] = {NULL};
Bool_Funct_Element * bool_func_mappings_table[FUNCTIONHASHSIZE] = {NULL};

int SetUpMappingsDefaults(){
  // .h files defaults
  SetUpMontecarloDefaults();
  // here
  SetUpDefaultMappingsFunctionsTable();

  return 0;
}

int SetUpDefaultMappingsFunctionsTable(){
  InstallDoubleFunc("Constant", Constant, double_func_mappings_table);
  InstallDoubleFunc("ChiSquareObservables", ChiSquareObservables, double_func_mappings_table);
  InstallBoolFunc("ChiSquareObservablesNPercentError", ChiSquareObservablesNPercentError, bool_func_mappings_table);
  InstallBoolFunc("ChiSquareObservablesResolution", ChiSquareObservablesResolution, bool_func_mappings_table);
  InstallBoolFunc("Pass", Pass, bool_func_mappings_table);

  return 0;
}

// Respective free

int FreeDefaultMappingsFunctionsTable(){
  FreeDoubleFuncTable(double_func_mappings_table, FUNCTIONHASHSIZE);
  FreeBoolFuncTable(bool_func_mappings_table, FUNCTIONHASHSIZE);

  return 0;
}

int FreeMappingsDefaults(){
  FreeDefaultMappingsFunctionsTable();
  FreeMontecarloDefaults();
  
  return 0;
}


///////////////////////////////////////////////////////////////////////
// Definition of functions to be mapped for some purpose
///////////////////////////////////////////////////////////////////////

double ChiSquareObservables(const double * const param, const double* const x){
  //param == (x,y,vz,mu) observed

  static orbital_Param orbital_param;
  static double v[2*DIM]; //vector to be obtained from Obrbital_Parameters_To_State
  
  orbital_param.rmin = x[0];
  orbital_param.f    = x[1];
  orbital_param.e    = x[2];
  orbital_param.w    = x[3];
  orbital_param.i    = x[4];
  orbital_param.W    = x[5];
  OrbitalParametersToStateVector(v, orbital_param, param[3]);
  return sqrt ((v[0] - param[0]) * (v[0] - param[0]) + (v[2] - param[1]) * (v[2] - param[1]) + (v[5] - param[2]) *(v[5] - param[2]));
}


///////////////////////////////////////////////////////////////////////////////
///   Definition of functions to constrain for  posterior /////////////////////
///////////////////////////////////////////////////////////////////////////////

_Bool ChiSquareObservablesNPercentError(const double * const param, const double * const x){

  static int result; 
  static double d_value; 
  
  d_value = param[4] * sqrt( param[0]*param[0] + param[1]*param[1] + param[2]*param[2] );
  //printf("%lf",x[6]); 
  result = x[6]<d_value ? 1: 0;
  
  return result; 
} 

_Bool ChiSquareObservablesResolution(const double * const param, const double * const x){
  static int result; 
  static double d_value; 

  double x_res = param[0];
  double y_res = param[1];
  double v_res = param[2];

  d_value =  sqrt(x_res * x_res + y_res * y_res + v_res * v_res);
  result = x[6]<d_value ? 1: 0;
  return result;
}

_Bool Pass(const double * const param, const double * const x){
  return 1;
}

int InitConstrainInfoExample(Constrain_info * constrain_info){

  constrain_info -> AuxF = ChiSquareObservables;
  constrain_info -> ConstrainF = ChiSquareObservablesNPercentError;
  constrain_info -> param_constrain = (double *) malloc(sizeof(double) * 5);
  
  // constrain_info 
  double G = 43007.1;
  double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) )*G;
  (constrain_info -> param_constrain)[0] = 17.95;
  (constrain_info -> param_constrain)[1] = -17.47;
  (constrain_info -> param_constrain)[2] = 192.0*1.023;
  (constrain_info -> param_constrain)[3] = mu;
  (constrain_info -> param_constrain)[4] = 0.1;

  return 0; 
}

// This is to init constrain kernel from input, it is defined in order to easily generalize the initialization from different param types, such that it became only a matter 
//   of  parameter extracting.
int InitConstrainInfoFromInput(Constrain_info * constrain_info, char * const aux_func_name, int n_param_aux, double * const param_aux, char ** param_aux_names,
                               double (* AuxF) (const double * const param_aux, const double * const x),
                               char * const constrain_func_name, int n_param_constrain, double * const param_constrain, char ** param_constrain_names,
                               _Bool (* ConstrainF) (const double * const param_constrain, const double * const x), char * const constrain_message){
  constrain_info -> AuxF = AuxF;
  constrain_info -> aux_func_name = aux_func_name;
  constrain_info -> n_param_constrain = n_param_constrain;
  constrain_info -> param_aux = param_aux;  

  if(param_aux_names == NULL){
    printf("param_aux_names is NULL, so param_aux1, ..., param_auxn are set as default\n"); 
    CreateNthBaseNames(&param_aux_names, "param_aux", n_param_aux, MAX_STRING_HEADER);
    constrain_info -> allocate_param_aux_names = 1;
  }
  else
    constrain_info -> allocate_param_aux_names = 0;

  constrain_info -> param_aux_names = param_aux_names;
  
  constrain_info -> ConstrainF = ConstrainF;
  constrain_info -> constrain_func_name = constrain_func_name;
  constrain_info -> n_param_aux = n_param_aux;
  constrain_info -> param_constrain = param_constrain;

  if (param_constrain_names == NULL){
    printf("param_constrain_names is NULL, so param_constrain1, ..., param_constrainn are set as default\n"); 
    CreateNthBaseNames(&param_constrain_names, "param_constrain", n_param_constrain, MAX_STRING_HEADER);
    constrain_info -> allocate_constrain_names = 1;
  }
  else
    constrain_info -> allocate_constrain_names = 0;
  constrain_info -> param_constrain_names = param_constrain_names;
  
  if (constrain_message == NULL)
    constrain_info -> constrain_message = strdup("Nothing specified");
  constrain_info -> constrain_message = constrain_message;

  return 0; 
}

int FreeDefaultConstrainInfo(Constrain_info * constrain_info){
  int i; // General counter

  if (constrain_info->allocate_param_aux_names){
    for (i = 0; i < constrain_info->n_param_aux; i++){
      free(constrain_info->param_aux_names[i]);
    }
  free(constrain_info->param_aux_names);
  }

  if (constrain_info->allocate_constrain_names){
    for (i = 0; i < constrain_info->n_param_constrain; i++){
      free(constrain_info->param_constrain_names[i]);
    }
    free(constrain_info->param_constrain_names);
  }

  return 0;
}

int InitConstrainInfoFromJsonFile(Constrain_info * constrain_info, char * param_filename){
  
  char * str = 0;  // To save the input of the file as a string 
	struct json_object *jobj;  // Then str used to init this object
	char key[1000];  // Use it to indicate defuaul keys in input

  // Parameters to be extracted
  char * df_name;
  int n_param_aux;
  double * param_aux;
  char ** param_aux_names = NULL;

  char * bf_name;
  int n_param_constrain;
  double * param_constrain;
  char ** param_constrain_names = NULL;

  char * constrain_message = NULL;

  int result = 1; // Indicat of succes to extract result of optional variables

  // Extract file as string
  FileToStrting(param_filename, &str);
  jobj = json_tokener_parse(str);

  // Extract all of AuxF
	sprintf(key, "AuxF");
	ExtractJsonString(jobj, key, &df_name, !OPTIONAL);

	sprintf(key, "param_aux");
	ExtractJsonDoubleArray(jobj, key, &param_aux, !OPTIONAL);
  n_param_aux = ExtractJsonArrayLen(jobj, key, !OPTIONAL);

  sprintf(key, "param_aux_names");
  result = ExtractJsonStringArray(jobj, key, &param_aux_names, OPTIONAL);
  if (result == NOT_EXISTENCE)
    param_aux_names = NULL;

  // Extract all of ConstrainF
	sprintf(key, "ConstrainF");
	ExtractJsonString(jobj, key, &bf_name, !OPTIONAL);

	sprintf(key, "param_constrain");
	ExtractJsonDoubleArray(jobj, key, &param_constrain, !OPTIONAL);
  n_param_constrain = ExtractJsonArrayLen(jobj, key, !OPTIONAL);

  sprintf(key, "param_constrain_names");
  result = ExtractJsonStringArray(jobj, key, &param_constrain_names, OPTIONAL);
  if (result == -1)
    param_constrain_names = NULL;
  
  // Constrain message
  sprintf(key, "constrain_message");
	ExtractJsonString(jobj, key, &constrain_message, !OPTIONAL);
  
  InitConstrainInfoFromInput(constrain_info, df_name, n_param_aux, param_aux, param_aux_names, LookupDoubleFunc(df_name, double_func_mappings_table)->double_func,
                             bf_name, n_param_constrain, param_constrain, param_constrain_names, LookupBoolFunc(bf_name, bool_func_mappings_table)->bool_func, constrain_message);
  free(str);
  json_object_put(jobj);

  return 0;
}

int FreeConstrainInfoComplete(Constrain_info * constrain_info){
  int i; // General counter

  free(constrain_info->aux_func_name);
  free(constrain_info->param_aux);

  for (i = 0; i < constrain_info->n_param_aux; i++){
    free(constrain_info->param_aux_names[i]);
  }
  free(constrain_info->param_aux_names);

  free(constrain_info->constrain_func_name);
  free(constrain_info->param_constrain);

  for (i = 0; i < constrain_info->n_param_constrain; i++){
    free(constrain_info->param_constrain_names[i]);
  }
  free(constrain_info->param_constrain_names);

  free(constrain_info->constrain_message);
  return 0;
}

int InitRunInfoExample(Run_info * run_info){

  run_info -> total_config = 1000; 
  run_info -> j_0 = 0; 
  run_info -> n_points = 1000; 

  return 0; 
}

int InitRunInfoFromInput(Run_info * run_info, size_t total_config, size_t j_0, size_t n_points, char * out_filename, char * methodology){    
    run_info -> total_config = total_config;
    run_info -> j_0 = j_0;
    run_info -> n_points = n_points;
    run_info -> out_filename = out_filename;
    run_info -> methodology = methodology;
    return 0;
}

int InitRunInfoFromJsonFile(Run_info * run_info, char * param_filename){
  char * str = 0;  // To save the input of the file as a string 
  struct json_object *jobj;  // Then str used to init this object
  char key[1000];  // Use it to indicate defuaul keys in input
  
  size_t total_config, j_0, n_points; 
  char * out_filename, * method;

  FileToStrting(param_filename, &str);

  jobj = json_tokener_parse(str);

  sprintf(key, "total_config");
	total_config = (size_t) ExtractJsonInt64(jobj, key, !OPTIONAL);

  sprintf(key, "j_0");
	j_0 = (size_t) ExtractJsonInt64(jobj, key, !OPTIONAL);

  sprintf(key, "n_points");
	n_points = (size_t) ExtractJsonInt64(jobj, key, !OPTIONAL);

  sprintf(key, "out_filename");
	ExtractJsonString(jobj, key, &out_filename, !OPTIONAL);

  sprintf(key, "methodology");
	ExtractJsonString(jobj, key, &method, !OPTIONAL);

  InitRunInfoFromInput(run_info, total_config, j_0, n_points, out_filename, method);
  
  free(str);
  json_object_put(jobj);

  return 0;
}

int FreeRunInfoComplete(Run_info * run_info){
  free(run_info->methodology);
  free(run_info->out_filename);

  return 0;
}

int InitUniformMontecarloMappingStructuresFromJson(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info, char * param_filename){

  InitRunInfoFromJsonFile(run_info, param_filename);
  InitConstrainInfoFromJsonFile(constrain_info, param_filename);
  InitConstantKernelFromJsonFile(kernel_info, param_filename);

  return 0;
}

int InitStandartMontecarloMappingStructuresFromJson(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info, char * param_filename){

  InitRunInfoFromJsonFile(run_info, param_filename);
  InitConstrainInfoFromJsonFile(constrain_info, param_filename);
  InitKernelFromJsonFile(kernel_info, param_filename);

  return 0;
}

int FreeUniformMontecarloMappingStructuresComplete(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info){

  FreeConstantKernelComplete(kernel_info);
  FreeConstrainInfoComplete(constrain_info);
  FreeRunInfoComplete(run_info);

  return 0;
}

int MontecarloStrcuturesConstrainedSampleGetNPoints(FILE * ptr_output_file,
    Run_info * run_info, Kernel_Info * kernel_info, Constrain_info * constrain_info){ 
  
  size_t j, l; // General counter, counter for ns_points
  int i = 0;
  // x array over which the condition is applied, and also saves the result of
  //  the metric.
  double * x = (double *) malloc( (kernel_info->n_dim + 1) * sizeof(double)); 
 
  // Create and initialize variables array in order to fill it in the routine
  double * variables = (double *) malloc(run_info->n_points * (kernel_info->n_dim + 1) * sizeof(double));
 
  l = 0;
  j = run_info->j_0;
  while(j < run_info->total_config){
    // printf("%ld\n", j);
    // Generate point according to kernel and save in variables the found point; 
    //   later will be evaluated if it satisfy further posteriors.

    GenPointForKernel(x, kernel_info);
  
    for (i = 0; i < kernel_info->n_dim; i++){
      variables[l * (kernel_info->n_dim + 1) + i] = x[i];
      }
    
    // Calculate a metric to the condition
    x[i] = constrain_info->AuxF(constrain_info->param_aux, x);

    // Save its values in the mean time to probably save in the future 
    variables[l * (kernel_info->n_dim + 1) + i] = x[i]; 


    if (x[i] < 5)
      printf("%lf\n", x[i]);
    // Evaluate saving condition
    if(constrain_info->ConstrainF(constrain_info->param_constrain, x)){
      l++;
      j++;
      printf("------------------%ld", j);
    }

    if (l == run_info->n_points){
      printf("j: %zu, total_config:%ld \n", j, run_info->total_config); 
      printf("l==run_info->n_points: %d\n", l==run_info->n_points);
      printf("rmin: %.1lf\n\n", kernel_info->val_f[0][0]);
      SaveVariablesInFile(variables, kernel_info->n_dim + 1, run_info->n_points,
                      ptr_output_file);

      l = 0; 
    }
    
  }

  if(l !=0 )
    SaveVariablesInFile(variables, kernel_info->n_dim + 1, l, ptr_output_file);

  free(variables);
  free(x);
  return 0;
}

///////////////////////////////////////////////////////////////////////
/// Definition of some input_output functions /////////////////////////
///////////////////////////////////////////////////////////////////////
/*
input: 
  result: pointer in which represents the resulting string to be written.
  keyword: message to be writen in the line, it refers to the parameter that
    will go just below.
  ptr_ouput_file: file in which this is to be written.
 
 Write well verified string with remination '\n''\0' and MAX_STRING_HEADER
   len for the message, just in the line in which it is called.
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
  fprintf(ptr_output_file, "%s" ,result);

  return 0;
}

/*
input:
  filename: name of file in which the header is to be witten, and output will
    be saved.
  method: name of the method of the type of mapping made.
  n_points: total number of points mapped.
  variable_names: name of the variables to be mapped, in same order than are
    internally mapped.
  functions: array of names of the functions used to map each parameter, the
    order correspond with the one variables name.
  var_0: initial values in th ranges of the variables to be mapped.
  var_f: final   values .....
  n_var: number of dimension of the function mapped.
  ptr_ouput_file: File in which the header is desired to be saved
result:
  header of the type of mapping to be made.
*/

int WriteMapHeaderFromKernels(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info){
  FILE * ptr_output_file = ReadFileName(run_info->out_filename, "w");

  // Go to first line of file, this is done becuse changes can be made.
  fseek(ptr_output_file, 0L, SEEK_SET);

  int i, j; // General counters
  // Pointer of character to save the result to be printed in each line of
  //   header.
  char dummy[MAX_STRING_HEADER], dummy2[MAX_STRING_HEADER];
  // Size of titles in header; e. g. ~ #FixHeaderLen(bytes)----------     \n\0
  int real_str_size = MAX_STRING_HEADER + 2;
  // Size of the header in bytes, note that the '\0' is not included in the sum
  //  thanks to the fact that it reflects lack of character in asscii, this 
  //  '\0', so it to calulate the len of the Header it must be left out.
  real_str_size -= 1;

  // The sum can be decomposed as follows (note + 1 of '\n'):
  //        len of header / int             + N_lines header / int            + filename / str
  //        mapping method / str  + points_mapped / zu              + Nvar_mapped / int
  //        var_names / str       + mapping inter, 16 sig figs.   
  int len = (real_str_size + INT32_LEN + 1) + (real_str_size + INT32_LEN + 1) + real_str_size  *  2 +
            real_str_size  *  2  + (real_str_size + SIZE_T_LEN + 1) + (real_str_size + INT32_LEN + 1) +
            real_str_size * kernel_info->n_dim + (DOUBLE_LEN + 1) * kernel_info->n_dim * 2;
  int n_lines = 2 + 2 + 2 +
                2 + 2 + 2 +
                kernel_info->n_dim + kernel_info->n_dim * 2;

  // Calculating for the parameters associated with the distribution.
  // (distribution names) + (parameter names) + (distribution parameter, 16 sig fig) + (max values, 16 sig fig)
  for (int i = 0; i < kernel_info->n_functions; i++){
      len += real_str_size; // for distribution name
      n_lines += 1;

      len += real_str_size * kernel_info->dim_of_param[i]; // for parameter name
      len += (DOUBLE_LEN + 1) * kernel_info->dim_of_param[i]; // for parameter values
      n_lines += 2 * kernel_info->dim_of_param[i];

      len += real_str_size; // max value message
      len += (DOUBLE_LEN + 1); // max value
      n_lines += 2;
  }
  ////////////////////////////////////////////////////////////////////
  // note that for string the number of that can contain each one is 
  //  MAX_STRING_HEADER + 2, the last 2 characters are fixed.
  // also the significan figures of a intger in the best case are 10, for a zu
  // are 20, and for doubles 16; each time one of this is written a space is 
  // left for a \n character.
  WriteLine(dummy, "#FixHeaderLen(bytes)----------", ptr_output_file);
  fprintf(ptr_output_file, "%10d\n", len);

  WriteLine(dummy, "#FixNumberOfLines----------", ptr_output_file);
  fprintf(ptr_output_file, "%10d\n", n_lines);
  
  WriteLine(dummy, "#Namefile----------", ptr_output_file);
  WriteLine(dummy, run_info->out_filename, ptr_output_file);
   
  WriteLine(dummy, "#Method------------", ptr_output_file);
  WriteLine(dummy, run_info->methodology, ptr_output_file);

  WriteLine(dummy,"#Npoints-----------", ptr_output_file);
  fprintf(ptr_output_file,"%20zu\n", run_info->total_config);

  WriteLine(dummy,"#Nvar--------------", ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", kernel_info->n_dim);
  
  int index = 0;
  for(i=0; i < kernel_info->n_functions;i++){
    sprintf(dummy2, "%s------------", kernel_info->function_names[i]);
    WriteLine(dummy, dummy2, ptr_output_file);
    for(j = 0; j < kernel_info->dim_of_val[i]; j++){
      sprintf(dummy2, "%s--", kernel_info->function_variables_names[index]);
      index += 1;
      WriteLine(dummy, dummy2, ptr_output_file);
      fprintf(ptr_output_file,"%16.15g\n", kernel_info->val_0[i][j]);
      fprintf(ptr_output_file,"%16.15g\n", kernel_info->val_f[i][j]); 
    }
        
    for (j = 0; j < kernel_info->dim_of_param[i]; j++){
      sprintf(dummy2, "%s--", kernel_info->function_param_names[i][j]);      
      WriteLine(dummy, dummy2, ptr_output_file);
      fprintf(ptr_output_file,"%16.15g\n", kernel_info->function_param[i][j]);
    }
    WriteLine(dummy, "max_value", ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", kernel_info->f_max_values[i]);
  }
  fclose(ptr_output_file);
 
  return 0;
}

/*
This routine will write the header of the output file along with the type of 
  constrain that was applied. For that a new out_file_name can be specified if
  both data are needed.
*/

int WriteMapConstrainHeaderFromKernels(Run_info *run_info, Kernel_Info *kernel_info, Constrain_info *constrain_info, const char * const out_filename){
  // Variables to be changed in header and to read base file in run_info
  int bytes_header_size;
  int n_lines;
  char * header_string;
  // General counter
  int i;
  // General variables for writing purposes
  char dummy[MAX_STRING_HEADER], dummy2[MAX_STRING_HEADER];
  // To calculate the writing space
  int real_str_size = MAX_STRING_HEADER + 1; // The '\0' is not count in writing

  // Open base header file declare aimed write pointer
  FILE * ptr_input_file = ReadFileName(run_info->out_filename, "r+");
  FILE * ptr_output_file;

  // Reading size of input header and its number of lines
  ValueInLineN(ptr_input_file, "%d" ,&bytes_header_size, 2); 
  ValueInLineN(ptr_input_file, "%d" ,&n_lines, 4); 

  // In case pointer are different write base version in the other.
  if (strcmp(run_info->out_filename, out_filename)){  
    // Copying input header to constained one
    ptr_output_file = ReadFileName(out_filename, "w+");
    header_string = malloc(sizeof(char) * bytes_header_size); 
    fseek(ptr_input_file,0L,SEEK_SET);
    fread(header_string, sizeof(char), bytes_header_size, ptr_input_file);

    // Write the header without being changed
    fseek(ptr_output_file,0L,SEEK_SET);
    fwrite(header_string, sizeof(char), bytes_header_size, ptr_output_file);
  
    free(header_string);
    fclose(ptr_input_file);
  }
  else{
    fclose(ptr_input_file);
    ptr_output_file = ReadFileName(run_info->out_filename, "r+");
  }

  // GoToFileLine(ptr_output_file,n_lines + 1);
  fseek(ptr_output_file,bytes_header_size * 1L, SEEK_SET);

  // Finally, write the cosntrained messages and all the info of the constrai
  //  functions.
  WriteLine(dummy,"#ConstrainMessage----------",ptr_output_file);
  WriteLine(dummy,constrain_info->constrain_message, ptr_output_file);

  sprintf(dummy2, "#%s------------", constrain_info->aux_func_name);
  WriteLine(dummy,dummy2,ptr_output_file);

  // Write parameters of function
  for (i = 0; i < constrain_info->n_param_aux; i++)
  {
    WriteLine(dummy, constrain_info->param_aux_names[i], ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", constrain_info->param_aux[i]);
  }   
  
  //--------------
  sprintf(dummy2, "#%s------------", constrain_info->constrain_func_name);
  WriteLine(dummy,dummy2,ptr_output_file);

  for ( i = 0; i < constrain_info->n_param_constrain; i++)
  {
    WriteLine(dummy, constrain_info->param_constrain_names[i], ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", constrain_info->param_constrain[i]);
  }

  // Modify all the variables that changed and write them in new constrained
  //   file. 
  bytes_header_size += real_str_size * 2 +
                       real_str_size * 1 + real_str_size * constrain_info->n_param_aux +  (DOUBLE_LEN + 1) * constrain_info->n_param_aux +
                       real_str_size * 1 + real_str_size * constrain_info->n_param_constrain +  (DOUBLE_LEN + 1) * constrain_info->n_param_constrain;
  n_lines += 2 + 1 + constrain_info->n_param_aux * 2 + 1 + constrain_info->n_param_constrain * 2; 

  GoToFileLine(ptr_output_file, 2);
  fprintf(ptr_output_file,"%10d\n", bytes_header_size);

  GoToFileLine(ptr_output_file, 4);
  fprintf(ptr_output_file,"%10d\n", n_lines);

  // fseek(ptr_output_file,bytes_header_size,SEEK_SET);
  // fprintf(ptr_output_file, "here\n");
  // exit(0);

  fclose(ptr_output_file);

  return 0;
}


/*
This routine will write the header of the output file along with the type of 
  constrain that was applied.
*/
int WriteMapConstrainHeader(FILE *ptr_input_file, FILE *ptr_output_file, const char * const constrain_message, 
                            char * aux_func_name, char ** aux_param_names, double * aux_func_param, int n_aux_param, 
                            char * cond_func_name, char ** cond_param_names, double * cond_func_param, int n_cond_param,
                            const char * const out_filename){
  // Getting size of header
  int bytes_header_size;
  int n_lines;
  char* header_string;
  int i;
  int real_str_size = MAX_STRING_HEADER + 1; // The '\0' is not count in writing

  char dummy[MAX_STRING_HEADER], dummy2[MAX_STRING_HEADER];

  // Reading size of input header and its number of lines
  ValueInLineN(ptr_input_file, "%d" ,&bytes_header_size, 2); 
  ValueInLineN(ptr_input_file, "%d" ,&n_lines, 4); 

  if (ptr_input_file != ptr_output_file){  
    // Copying input header to constained one
    header_string = malloc( sizeof(char) * bytes_header_size ); 
    fseek(ptr_input_file,0L,SEEK_SET);
    fread(header_string, sizeof(char), bytes_header_size, ptr_input_file);

    // Write the header without being changed
    fseek(ptr_output_file,0L,SEEK_SET);
    fwrite(header_string, sizeof(char), bytes_header_size, ptr_output_file);
  }
  // GoToFileLine(ptr_output_file,n_lines + 1);
  fseek(ptr_output_file,bytes_header_size * 1L, SEEK_SET);

  // Finally, write the cosntrained messages and modify the size of the header
  //   and the number of lines to be changed.
  WriteLine(dummy,"#ConstrainMessage----------",ptr_output_file);
  WriteLine(dummy,constrain_message,ptr_output_file);

  sprintf(dummy2, "#%s------------", aux_func_name);
  WriteLine(dummy,dummy2,ptr_output_file);

  // Write parameters of function
  for ( i = 0; i < n_aux_param; i++)
  {
    WriteLine(dummy, aux_param_names[i], ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", aux_func_param[i]);
  }
  
  sprintf(dummy2, "#%s------------", cond_func_name);
  WriteLine(dummy,dummy2,ptr_output_file);

  for ( i = 0; i < n_cond_param; i++)
  {
    WriteLine(dummy, cond_param_names[i], ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", cond_func_param[i]);
  }

  // Modify all the variables that changed and write them in new constrained
  //   file. 
  bytes_header_size += real_str_size * 2 +
                        real_str_size * 1 + real_str_size * n_aux_param +  (DOUBLE_LEN + 1) * n_aux_param +
                        real_str_size * 1 + real_str_size * n_cond_param +  (DOUBLE_LEN + 1) * n_cond_param;
  n_lines +=2 + 2 + n_aux_param * 2 + 2 + n_cond_param * 2; 

  GoToFileLine(ptr_output_file,2);
  fprintf(ptr_output_file,"%10d\n", bytes_header_size);
  
  GoToFileLine(ptr_output_file,4);
  fprintf(ptr_output_file,"%10d\n", n_lines);

  fseek(ptr_output_file,bytes_header_size,SEEK_SET);
  // Go to last postion to later write the ponits.
  
  return 0; 
}

void * MontecarloStrcuturesConstrainedSampleWriteThread(void * mappings_info){
  mappings_info = (Mappings_info *) mappings_info;
  MontecarloStrcuturesConstrainedSampleWrite(mappings_info);

  return NULL;
}

int MontecarloStrcuturesConstrainedSampleWrite(Mappings_info * mappings_info){
  WriteMapHeaderFromKernels(mappings_info->ptr_run_info, mappings_info->ptr_kernel_info, mappings_info->ptr_constrain_info);
  WriteMapConstrainHeaderFromKernels(mappings_info->ptr_run_info, mappings_info->ptr_kernel_info, mappings_info->ptr_constrain_info,
                                     mappings_info->ptr_run_info->out_filename);  
  FILE * ptr_output_file = ReadFileName(mappings_info->ptr_run_info->out_filename, "ab+");
  int bytes_header_size;
  ValueInLineN(ptr_output_file, "%d" ,&bytes_header_size, 2); 
  fseek(ptr_output_file, bytes_header_size * 1L, SEEK_SET);
  MontecarloStrcuturesConstrainedSampleGetNPoints(ptr_output_file, mappings_info->ptr_run_info, mappings_info->ptr_kernel_info, 
                                                  mappings_info->ptr_constrain_info);
  FreeUniformMontecarloMappingStructuresComplete(mappings_info->ptr_run_info, mappings_info->ptr_kernel_info, 
                                                 mappings_info->ptr_constrain_info);
  fclose(ptr_output_file);

  return 0;
}

// This routine will run an individual trial on a mapping condition.... 
int MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(char * param_file, int file_type){
  Kernel_Info kernel_info;
  Run_info run_info;
  Constrain_info constrain_info;

  switch (file_type){
  case JSON_FILE:
    // Get info about if it is a uniform or specified mapping
    if (IsUniformMappingType(param_file)){
      InitUniformMontecarloMappingStructuresFromJson(&run_info, &kernel_info, &constrain_info, param_file);
    }
    else{
      InitStandartMontecarloMappingStructuresFromJson(&run_info, &kernel_info, &constrain_info, param_file);
    }

    FILE * ptr_output_file;
    int bytes_header_size;

    if (access(run_info.out_filename, W_OK) == -1){ 
      // file doesn't exist: Write new Header in file.dat and skip header
      WriteMapHeaderFromKernels(&run_info, &kernel_info, &constrain_info);
      WriteMapConstrainHeaderFromKernels(&run_info, &kernel_info, &constrain_info, run_info.out_filename);
      ptr_output_file = ReadFileName(run_info.out_filename, "ab+");
      ValueInLineN(ptr_output_file, "%d" ,&bytes_header_size, 2); 
      fseek(ptr_output_file, bytes_header_size * 1L, SEEK_SET);
    }
    else{ 
      // When exist follow mapping process if total_config has been increased, otherwise simply finish.
      printf("file %s already exist, it is assumed (not checked) that only changed total_config\n", run_info.out_filename);
      ptr_output_file = ReadFileName(run_info.out_filename, "rb+");
      ValueInLineN(ptr_output_file, "%d", &(run_info.j_0), 10);
      ValueInLineN(ptr_output_file, "%d" ,&bytes_header_size, 2);
      fseek(ptr_output_file, 0, SEEK_END);
      // fseek(ptr_output_file, bytes_header_size * 1L, SEEK_SET);
      if (run_info.j_0 >= run_info.total_config){
        printf("total_config: %ld points have been already mapped\n", run_info.j_0);
        exit(0);
      }
    }

    MontecarloStrcuturesConstrainedSampleGetNPoints(ptr_output_file, &run_info, &kernel_info, &constrain_info);
    FreeUniformMontecarloMappingStructuresComplete(&run_info, &kernel_info, &constrain_info);
    fclose(ptr_output_file);
    break;
  default:
    break;
  }  

  return 0;
}

_Bool IsUniformMappingType(char * param_filename){
  _Bool check_state = 1;

  char * str = 0;  // To save the input of the file as a string 
  struct json_object *jobj;  // Then str used to init this object
  char key[1000];  // Use it to indicate defuaul keys in input

  char ** function_names;

  FileToStrting(param_filename, &str);
  jobj = json_tokener_parse(str);

  sprintf(key, "function_names");
  check_state = check_state & (ExtractJsonStringArray(jobj, key, &function_names, OPTIONAL) == NOT_EXISTENCE);

  return check_state;
} 