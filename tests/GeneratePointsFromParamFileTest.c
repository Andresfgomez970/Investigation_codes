#include "WriteMapHeaderTest.h"

double NormND(const double * const param_aux, const double * const x){
    double norm = 0;
    int i = 0;
    for (i = 0; i < param_aux[0]; i++)
        norm += x[i] * x[i];
    return sqrt(norm);
}

_Bool LessThanIndexi(const double * const param_constrain, const double * const x){
    // printf("%d, %lf\n",(int) param_constrain[0], x[(int) param_constrain[0]]);
    return x[(int) param_constrain[0]] < param_constrain[1];
}

double Gaussian(const double * const param, const double * const x){
    return 1 / (pow(2 * M_PI, 0.5) * param[1]) * exp(-((x[0] - param[0]) * (x[0] - param[0])) / (2. * param[1] * param[1]));
}

double DoubleGaussian(const double * const param, const double * const x){
    return Gaussian(&param[0], &x[0]) * Gaussian(&param[2], &x[1]);
}

int ConstantKernelTest(char * param_filename, int plot){
    int bytes_header_size;
    char data_file[MAX_STRING_HEADER], base_name[MAX_STRING_HEADER];
    char command[1000]; // To run plot of graphs
    FILE * ptr_file; 

    strcpy(data_file, "GeneratePointsFromParamFileTest_DoubleCircle.dat");
    ptr_file = ReadFileName(data_file, "r");

    strcpy(param_filename, "GeneratePointsFromParamFileTest_param.json"); 
    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_filename, JSON_FILE);

    strcpy(base_name, "GeneratePointsFromParamFileTest_DoubleCircle");
    ValueInLineN(ptr_file, "%d" ,&bytes_header_size, 2); 
    if (plot){
        sprintf(command, "gnuplot -c plotters/plot_2Dbin_function_skip.plt %s %d -p", base_name, bytes_header_size);
        system(command);
        printf("%s", command);
    }

    return 0;
}

int KernelTest(char * param_filename, int plot){
    int bytes_header_size;
    char data_file[MAX_STRING_HEADER];
    char base_name[MAX_STRING_HEADER];
    char command[1000]; // To run plot of graphs
    FILE * ptr_file; 
    
    strcpy(param_filename, "GeneratePointsFromParamFileTest_param2.json"); 
    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_filename, JSON_FILE);

    strcpy(base_name, "GeneratePointsFromParamFileTest_DoubleGaussianAndConstant");
    sprintf(data_file, "%s.dat", base_name);
    ptr_file = ReadFileName(data_file, "r");
    
    ValueInLineN(ptr_file, "%d" ,&bytes_header_size, 2); 
    if (plot){
        sprintf(command, "gnuplot -c plotters/plot_histograms.plt %s %d -p", base_name, bytes_header_size);
        system(command);
        sprintf(command, "eog %sHistrograms.png", base_name);
        system(command);
    }

    return 0;
} 

void InstallUsedFunctions(){
    InstallDoubleFunc("Norm", NormND, double_func_mappings_table);
    InstallBoolFunc("LessThanIndexi", LessThanIndexi, bool_func_mappings_table); 
    InstallDoubleFunc("Gaussian", Gaussian, double_func_montecarlo_table);
    InstallDoubleFunc("DoubleGaussian", DoubleGaussian, double_func_montecarlo_table);
}

int main(int argc, char const *argv[])
{
    SetUpMappingsDefaults();
    InstallUsedFunctions();

    char param_filename[MAX_STRING_HEADER];

    // Test 1
    ConstantKernelTest(param_filename, 1);
    // Test 2
    KernelTest(param_filename, 1);

    FreeDefaultMappingsFunctionsTable();

    return 0;
}