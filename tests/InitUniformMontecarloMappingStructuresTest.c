#include "InitUniformMontecarloMappingStructuresTest.h"

int InitRunInfoFromInputTest();
int InitRunInfoFromJsonFileTest();
int InitUniformMontecarloMappingStructuresFromJsonTest();

int main(int argc, char const *argv[])
{
    int check_state = 1;

    check_state = InitRunInfoFromInputTest();
    check_state = check_state && InitRunInfoFromJsonFileTest();
    check_state = check_state && InitUniformMontecarloMappingStructuresFromJsonTest();

    printf("\n****************************** %s *****************************************\n", argv[0]);
    printf("%s pass the test? %d", argv[0], check_state);
    printf("\n****************************** %s *****************************************\n", argv[0]);
     
    return 0;
}

size_t total_config = 10000, j_0 = 0, n_points = 1000;
char * out_filename = "InitUniformMontecarloMappingStructuresTest.dat";
char * methodology = "Rejection";

int InitRunInfoFromInputTest(){
    int check_state = 1;

    Run_info run_info;
    InitRunInfoFromInput(&run_info, total_config, j_0, n_points, out_filename, methodology);

    check_state = check_state && (run_info.total_config == total_config);
    check_state = check_state && (run_info.j_0 == j_0);
    check_state = check_state && (run_info.n_points == n_points);
    check_state = check_state && !strcmp(methodology, run_info.methodology);
    check_state = check_state && !strcmp(out_filename, run_info.out_filename);

    return check_state;
}

int InitRunInfoFromJsonFileTest(){
    int check_state = 1;

    Run_info run_info;
    char * filename = "InitUniformMontecarloMappingStructuresTest_params.json";
    InitRunInfoFromJsonFile(&run_info, filename);

    check_state = check_state && (run_info.total_config == 10000L);
    check_state = check_state && (run_info.j_0 == 0L);
    check_state = check_state && (run_info.n_points == 1000L);
    check_state = check_state && !strcmp(methodology, run_info.methodology);
    check_state = check_state && !strcmp(out_filename, run_info.out_filename);

    FreeRunInfoComplete(&run_info);
    
    return check_state;
}

int InitUniformMontecarloMappingStructuresFromJsonTest(){
    int check_state = 1;

    Run_info run_info;
    Kernel_Info kernel_info;
    Constrain_info constrain_info;
    char * param_filename = "InitUniformMontecarloMappingStructuresTest_params.json";

    SetUpDefaultMappingsFunctionsTable();
    InitUniformMontecarloMappingStructuresFromJson(&run_info, &kernel_info, &constrain_info, param_filename);
    FreeUniformMontecarloMappingStructuresComplete(&run_info, &kernel_info, &constrain_info);
    FreeDefaultMappingsFunctionsTable();
    
    // The parameters obtained are checked already using InitContrain and
    //   InitConstantKernel by apart.

    return check_state;
}