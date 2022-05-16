int ConstrainHeaderFromInput();
int WriteMapHeaderFromInput(const char * const filename, const char * const method, const size_t n_points, const int n_var, 
                   char ** variables_names, double ** var_0, double ** var_f, const int * const dim_dist, 
                   char ** distribution_names, const int n_dist, const int * const dim_params, double ** dist_params, char *** dist_param_names,
                   const double * const f_max_values, FILE * ptr_output_file);

int main(int argc, char const *argv[])
{
    ConstrainHeaderFromInput();
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
int WriteMapHeaderFromInput(const char * const filename, const char * const method, const size_t n_points, const int n_var, 
                   char ** variables_names, double ** var_0, double ** var_f, const int * const dim_dist, 
                   char ** distribution_names, const int n_dist, const int * const dim_params, double ** dist_params, char *** dist_param_names,
                   const double * const f_max_values, FILE * ptr_output_file){
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
            real_str_size * n_var + (DOUBLE_LEN + 1) * n_var * 2;
  int n_lines = 2 + 2 + 2 +
                2 + 2 + 2 +
                n_var + n_var * 2;

  // Calculating for the parameters associated with the distribution.
  // (distribution names) + (parameter names) + (distribution parameter, 16 sig fig) + (max values, 16 sig fig)
  for (int i = 0; i < n_dist; i++){
      len += real_str_size; // for distribution name
      n_lines += 1;

      len += real_str_size * dim_params[i]; // for parameter name
      len += (DOUBLE_LEN + 1) * dim_params[i]; // for parameter values
      n_lines += 2 * dim_params[i];

      len += real_str_size; // max value message
      len += (DOUBLE_LEN + 1); // max value
      n_lines += 2;
  }

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
  fprintf(ptr_output_file, "%10d\n", n_lines);
  
  WriteLine(dummy, "#Namefile----------", ptr_output_file);
  WriteLine(dummy, filename, ptr_output_file);
   
  WriteLine(dummy, "#Method------------", ptr_output_file);
  WriteLine(dummy, method, ptr_output_file);

  WriteLine(dummy,"#Npoints-----------", ptr_output_file);
  fprintf(ptr_output_file,"%20zu\n", n_points);

  WriteLine(dummy,"#Nvar--------------", ptr_output_file);
  fprintf(ptr_output_file,"%10d\n", n_var);

  
  int index = 0;
  for(i=0; i<n_dist;i++){
    sprintf(dummy2, "%s------------", distribution_names[i]);
    WriteLine(dummy, dummy2, ptr_output_file);
    for(j = 0; j < dim_dist[i]; j++){
      sprintf(dummy2, "#%s--", variables_names[index]);
      index += 1;
      WriteLine(dummy, dummy2, ptr_output_file);
      fprintf(ptr_output_file,"%16.15g\n", var_0[i][j]);
      fprintf(ptr_output_file,"%16.15g\n", var_f[i][j]); 
    }
    for (j = 0; j < dim_params[i]; j++){
      sprintf(dummy2, "%s--", dist_param_names[i][j]);      
      fprintf(ptr_output_file,"%16.15g\n", dist_params[i][j]);
      }
    WriteLine(dummy, "max_value", ptr_output_file);
    fprintf(ptr_output_file,"%16.15g\n", f_max_values[i]);
  }

  return 0;
}


int ConstrainHeaderFromInput(){
    char * filename = "WriteMapHeaderTest.dat";
    char * method = "Rejection";
    size_t n_points =  100000000;
    int n_var = 6;
    char * variables_names[6] = {"rmin", "f", "e", "w", "i", "W"};
    double var_0i[2*DIM] ={3 ,  0    , 0.8 , 0     , 0     , 0    };
    double var_fi[2*DIM] = {21, 2*M_PI, 1.4, 2*M_PI, M_PI, 2*M_PI};
    double ** var_0 = malloc(sizeof(double *) * 1),  ** var_f = malloc(sizeof(double *) * 1);
    var_0[0] = (double *) malloc(sizeof(double) * 2*DIM);
    var_f[0] = (double *) malloc(sizeof(double) * 2*DIM);
    for (int i = 0; i < 2* DIM; i++)
    {
        var_0[0][i] = var_0i[i];
        var_f[0][i] = var_fi[i];
    }
    int dim_dist[1] = {6};
    char ** dist_names;
    dist_names = (char **) malloc(sizeof(char *) * 1);
    dist_names[0] = strdup("U");
    int n_dist = 1;
    int dim_params[1] = {0};
    char *** dist_param_names = NULL;
    // -1 to signify lack of it 
    double dist_paramsi[2*DIM] = {-1 ,  -1  ,-1 , -1 , -1  , -1};
    double **dist_params =  malloc(sizeof(double *) * 2*DIM);
    for (int i = 0; i < 2* DIM; i++)
    {
        dist_params[i] = malloc(sizeof(double));
        dist_params[i][0] = dist_paramsi[i];
    }
    double f_max_values[2*DIM] = {1, 1, 1, 1, 1, 1};
    FILE * ptr_output_file = ReadFileName(filename, "rb+");


    WriteMapHeaderFromInput(filename, method, n_points, n_var, variables_names, var_0, var_f, dim_dist, dist_names, n_dist, dim_params, dist_params, dist_param_names, f_max_values, ptr_output_file);
     
    char * constrain_message = "Maximum resolution";

    char * aux_func_name = "ChiSquareObservables";
    char * aux_param_names[4] = {"x", "y", "vz", "mu"};
    int n_aux_param = 4;
    double mu = ( (0.16+70.19+5.59+2.39)+(0.03+3.05+0.11+0.03) ) * 43007.1;
    double aux_func_param[4] = {17.95,-17.47,192.0*1.023,mu};

    char * cond_func_name = "ChiSquareObservablesNPercentError";
    char * cond_param_names[4] = {"x", "y", "vz", "%p"};
    double cond_func_param[4] = {17.95,-17.47,192.0*1.023,0.1};
    int n_cond_param = 4;

    WriteMapConstrainHeader(ptr_output_file, ptr_output_file, constrain_message, aux_func_name, aux_param_names, aux_func_param, n_aux_param, 
                            cond_func_name, cond_param_names, cond_func_param, n_cond_param, filename);

    return 0;
} 