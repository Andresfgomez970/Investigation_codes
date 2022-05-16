#include "WriteMapHeaderTest.h"

int ConstrainHeaderFromInput();

int main(int argc, char const *argv[])
{
    Kernel_Info kernel_info;
    Run_info run_info;
    Constrain_info constrain_info;
    char * param_filename = "WriteMapHeaderTest_param.json"; 

    SetUpDefaultMappingsFunctionsTable();
    
    InitUniformMontecarloMappingStructuresFromJson(&run_info, &kernel_info, &constrain_info, param_filename);
    WriteMapHeaderFromKernels(&run_info, &kernel_info, &constrain_info);
    WriteMapConstrainHeaderFromKernels(&run_info, &kernel_info, &constrain_info, run_info.out_filename);
    
    FreeUniformMontecarloMappingStructuresComplete(&run_info, &kernel_info, &constrain_info);
    FreeDefaultMappingsFunctionsTable();

    return 0;
}