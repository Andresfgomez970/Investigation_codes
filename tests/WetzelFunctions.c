#include "WetzelFunctions.h"

void evaluate_point(){
    double param[10] = {3.14, 0.152, 0.410, 0, 78e10,
                    0.450, -0.395, 0.109, 0, 78e10};
    double x[1] = {0};

    WetzelrminFixed(param, x);
}

void pLot_histograms(char * base_name){
    char command[MAX_STRING_HEADER * 10];
    sprintf(command, "gnuplot -c plotters/plot_histogram_columns.plt %s -p", base_name);
    system(command);
    sprintf(command, "eog %sHistrograms.png", base_name);
    system(command);
}

void plot_function(char * base_name){
    char command[MAX_STRING_HEADER * 10];
    
    sprintf(command, "gnuplot -c plotters/plot_1Dbin_function.plt %s -p", base_name);
    system(command);
    sprintf(command, "eog %s1Dplot.png", base_name);
    system(command);
}

// It consist of circularity and periapsis distirbution in wetzel
int TestExample1(){
    char param_file[MAX_STRING_HEADER] = "WetzelFunctions_param1.json";

    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_file, JSON_FILE);

    char base_name[MAX_STRING_HEADER];
    strcpy(base_name, "WetzelrminFixed1");
    pLot_histograms(base_name);

    return 0;
}

// 
int TestExample2(){
    char param_file[MAX_STRING_HEADER] = "WetzelFunctions_param2.json";

    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_file, JSON_FILE);

    char base_name[MAX_STRING_HEADER];
    strcpy(base_name, "WetzelrminFixed2");
    pLot_histograms(base_name);

    return 0;
}

int TestExample3(){
    char param_file[MAX_STRING_HEADER] = "WetzelFunctions_param3.json";

    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_file, JSON_FILE);

    char base_name[MAX_STRING_HEADER];
    strcpy(base_name, "WetzelrminFixed3");
    pLot_histograms(base_name);

    return 0;
}

int TestExample4(){
    char param_file[MAX_STRING_HEADER];
    strcpy(param_file, "WetzelFunctions_rmin.json");

    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_file, JSON_FILE);

    char base_name[MAX_STRING_HEADER];
    strcpy(base_name, "WetzelFunctions_rmin");
    plot_function(base_name);
 
    return 0;
}

int TestExample5(){
    char param_file[MAX_STRING_HEADER] = "WetzelFunctions_e.json";
    strcpy(param_file, "WetzelFunctions_e.json");

    MontecarloStrcuturesConstrainedSampleGetNPointsFromFile(param_file, JSON_FILE);

    char base_name[MAX_STRING_HEADER];
    strcpy(base_name, "WetzelFunctions_e");
    plot_function(base_name);
 
    return 0;
}

void clean(){
    char command[MAX_STRING_HEADER * 3];
    strcpy(command, "rm WetzelrminFixed1.dat");
    system(command);
    strcpy(command, "rm WetzelrminFixed2.dat");
    system(command);
    strcpy(command, "rm WetzelrminFixed3.dat");
    system(command);
    strcpy(command, "rm WetzelFunctions_rmin.dat");
    system(command);
    strcpy(command, "rm WetzelFunctions_e.dat");
    system(command);
}

int main(int argc, char const *argv[])
{
    clean();
    SetUpMappingsDefaults();

    InstallDoubleFunc("WetzelrminFixed", WetzelrminFixed, double_func_mappings_table);
    InstallDoubleFunc("WetzeleFixed", WetzeleFixed, double_func_mappings_table);

    // Let us remove files in case they have been created
    TestExample1();
    TestExample2();
    TestExample3();
    TestExample4();
    TestExample5();

    FreeMappingsDefaults();

    return 0;
}



// double C0, C1;
// double alpha_c = 0.38, beta_c = 0.567, gamma_c = 0.152, Mhost = 78e10, z = 0;
// C0 = Ci(alpha_c, beta_c, gamma_c, Mhost, z);
// double C_0, C_1; 