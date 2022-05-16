#include "allvars.h"
#include <pthread.h>
#include <unistd.h>

#define NSIZES_RMIN_D 9
#define NPERCENT_ERRORS_D 25

int RangesRmin(char * default_param_name);
int ObservationalErrors(char * default_param_name);
int RunMontecarloStrcuturesConstrainedSampleGetNPointsFromKeyInputFile(int argc, char *argv[]);

int SrtingLen(char * string){
  int i;
  for (i = 0; string[i] != '\0'; i++);

  return i;
}

int main(int argc, char *argv[]){
  int min_param = 3;

  if(argc < min_param){
    printf("Usage: ./main.out option json_parameter_file\n");
    if (strcmp("--help", argv[1]) == 0){
      printf("\nOptions:\n");
      printf("%-35s : map points according to input json_parameter_file\n", "single_mapping");
      printf("%-35s : map points according to input json_parameter_file changing only perigee distances to default values\n", "rmin_vary_default");
      printf("%-35s : map points according to input json_parameter_file changing only (x, y, vz) by diferent percentages\n", "observational_errors_vary_default");
    }
    exit(0);
  }

  if (strcmp("single_mapping", argv[1]) == 0)
    RunMontecarloStrcuturesConstrainedSampleGetNPointsFromKeyInputFile(argc, argv);
  else if (strcmp("rmin_vary_default", argv[1]) == 0)
    RangesRmin(argv[2]);
  else if (strcmp("observational_errors_vary_default", argv[1]) == 0)
    ObservationalErrors(argv[2]);

  return 0; 
}

int ObservationalErrors(char * default_param_name){
  // Defaults functions dictinaries of functions allocation, and other things
  SetUpMappingsDefaults();

  // Here //
  // Structures initiated in order to allocate parameter for the mappings,
  //  an array of the number of threads is neccesary in order to make all of 
  //  them at the same time.
  Kernel_Info * kernel_info = (Kernel_Info *) malloc(sizeof(Kernel_Info) * NPERCENT_ERRORS_D *  DIM);
  Run_info * run_info = (Run_info *) malloc(sizeof(Run_info) * NPERCENT_ERRORS_D *  DIM); 
  Constrain_info * constrain_info = (Constrain_info *) malloc(sizeof(Constrain_info) * NPERCENT_ERRORS_D *  DIM);
  Mappings_info * mappings_info = (Mappings_info *) malloc(sizeof(Mappings_info) * NPERCENT_ERRORS_D *  DIM);
  
  pthread_t threads[NPERCENT_ERRORS_D *  DIM];
  int i;

  /// Here ///
  // rmin sizes to be applied
  double percent[NPERCENT_ERRORS_D] = {    -0.01, -0.02, -0.03, -0.04, -0.05, -0.06, -0.07, -0.08, -0.09, -0.10, -0.30, -0.50,
                                       0.0, 0.01,  0.02,  0.03,  0.04,  0.05,  0.06,  0.07,  0.08,  0.09,  0.10,  0.30,  0.50};
  // Names of variables to be alter
  char names[DIM][MAX_STRING_HEADER] = {"x", "y", "z"};
  // dummy variable to change name
  char out_filename[MAX_STRING_HEADER];

  // Name to initialize all the default info for kernels
  char param_filename[MAX_STRING_HEADER];
  sprintf(param_filename, "%s", default_param_name);

  int c = 0; // General counter
  for (i = 0; i < NPERCENT_ERRORS_D; i++){
    // Init kernel accordingto defaults and save in mappings_info (structure 
    //  created to allow the parallelization).
    for (int j = 0; j < DIM; j++){
      InitUniformMontecarloMappingStructuresFromJson(&run_info[c], &kernel_info[c], &constrain_info[c], param_filename);
      mappings_info[c].ptr_kernel_info = &kernel_info[c];
      mappings_info[c].ptr_constrain_info = &constrain_info[c];
      mappings_info[c].ptr_run_info = &run_info[c];

      /// Here ///
      // Actuaize new values in Kernel (This is the par that needs to be changed)
      //  in other cases
      constrain_info[c].param_aux[j] *= (1 + percent[i]);
      free(run_info[c].out_filename);  // must be free before reassigned

      sprintf(out_filename, "%s", default_param_name);
      sprintf(&out_filename[SrtingLen(out_filename) - 5], "%s%0.0lfp.dat", names[j], percent[i] * 100);
      printf("%s\n", out_filename);
      run_info[c].out_filename = strdup(out_filename);

      printf("spawning thread %d\n", c);
      pthread_create(&threads[c], NULL, MontecarloStrcuturesConstrainedSampleWriteThread, (void *) &mappings_info[c]);
      sleep(1);
      c++;

    }
  }

  /* wait for threads to finish */
  for (i=0; i<NPERCENT_ERRORS_D *  DIM; ++i) {
    pthread_join(threads[i], NULL);
  }

  FreeMappingsDefaults();
  free(kernel_info);
  free(run_info);
  free(constrain_info);
  free(mappings_info);
  pthread_exit(NULL);
  
  return 0;
}


int RangesRmin(char * default_param_name){
  // Defaults functions dictinaries of functions allocation, and other things
  SetUpMappingsDefaults();

  // Here //
  // Structures initiated in order to allocate parameter for the mappings,
  //  an array of the number of threads is neccesary in order to make all of 
  //  them at the same time.
  Kernel_Info * kernel_info = (Kernel_Info *) malloc(sizeof(Kernel_Info) * NSIZES_RMIN_D);
  Run_info * run_info = (Run_info *) malloc(sizeof(Run_info) * NSIZES_RMIN_D); 
  Constrain_info * constrain_info = (Constrain_info *) malloc(sizeof(Constrain_info) * NSIZES_RMIN_D);
  Mappings_info * mappings_info = (Mappings_info *) malloc(sizeof(Mappings_info) * NSIZES_RMIN_D);
  
  pthread_t threads[NSIZES_RMIN_D];
  int i;

  /// Here ///
  // rmin sizes to be applied
  double max_rmin[NSIZES_RMIN_D] = {1, 5, 10, 20, 50, 100, 500, 2000, 4000};
  // dummy variable to change name
  char out_filename[MAX_STRING_HEADER];

  // Name to initialize all the default info for kernels
  char param_filename[MAX_STRING_HEADER];
  sprintf(param_filename, "%s", default_param_name);

  for (i = 0; i < NSIZES_RMIN_D; i++){
    // Init kernel accordingto defaults and save in mappings_info (structure 
    //  created to allow the parallelization).
    InitUniformMontecarloMappingStructuresFromJson(&run_info[i], &kernel_info[i], &constrain_info[i], param_filename);
    mappings_info[i].ptr_kernel_info = &kernel_info[i];
    mappings_info[i].ptr_constrain_info = &constrain_info[i];
    mappings_info[i].ptr_run_info = &run_info[i];

    /// Here ///
    // Actuaize new values in Kernel (This is the par that needs to be changed)
    //  in other cases
    kernel_info[i].val_f[0][0] = max_rmin[i];
    free(run_info[i].out_filename);  // must be free before reassigned
    sprintf(out_filename, "%s%.0lf.dat", "data/rmin_sizes/quiroga/rmin_max", max_rmin[i]);
    run_info[i].out_filename = strdup(out_filename);

    printf("spawning thread %d\n", i);
    pthread_create(&threads[i], NULL, MontecarloStrcuturesConstrainedSampleWriteThread, (void *) &mappings_info[i]);
    sleep(1);
  }

  /* wait for threads to finish */
  for (i=0; i<NSIZES_RMIN_D; ++i) {
    pthread_join(threads[i], NULL);
  }

  FreeMappingsDefaults();
  free(kernel_info);
  free(run_info);
  free(constrain_info);
  free(mappings_info);
  pthread_exit(NULL);
  
  return 0;
}

int RunMontecarloStrcuturesConstrainedSampleGetNPointsFromKeyInputFile(int argc, char *argv[]){
  int n_param = 3;

  if(argc != n_param){
    if(argc < n_param){
      printf("Please enter the path to the .json file\n");
      printf("Use example: ./main.out single_mapping pat_file/name_of_json.json\n");
      exit(0);
    }
    else
      printf("Note that you are entering more parameters than needed; enter only %d param", n_param);
  }

  char param_filename[MAX_STRING_HEADER];
  sprintf(param_filename, "%s", argv[2]);

  SetUpMappingsDefaults();

  MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_filename, JSON_FILE);

  FreeMappingsDefaults();

  return 0;
}
