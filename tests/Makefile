# General compile options
CC=gcc
LIBS_UBICATION = /home/$(USER)/locally_install/
# General flags
CFLAGS= -Wall -g -I. -I$(LIBS_UBICATION)include/gsl/ -I$(LIBS_UBICATION)include/json-c/
LFLAGS= -L$(LIBS_UBICATION)lib/ -lgsl -lgslcblas -lm -ljson-c
# Main routine programs dependences
OBJ_input_output = ../input_output.o
OBJ_two_body_analytical = ../two_body_analytical.o ../input_output.o
OBJ_montecarlo = ../montecarlo.o ../input_output.o
OBJ_mappings = ../montecarlo.o ../input_output.o ../two_body_analytical.o ../mappings.o
# Test programs dependences
OBJ_Rotation3DAngleTest = $(OBJ_two_body_analytical) Rotation3DAngleTest.o
OBJ_OrbitalParametersToStateVectorTest = $(OBJ_two_body_analytical) OrbitalParametersToStateVectorTest.o
OBJ_GeneratePointFromInputFunctionTest = $(OBJ_montecarlo) GeneratePointFromInputFunctionTest.o
OBJ_ExtractJsonTest = $(OBJ_input_output) ExtractJsonTest.o
OBJ_InitConstantKernelTest = $(OBJ_montecarlo) InitConstantKernelTest.o
OBJ_InitConstrainInfoTest = $(OBJ_mappings) InitConstrainInfoTest.o
OBJ_InitUniformMontecarloMappingStructuresTest = $(OBJ_mappings) InitUniformMontecarloMappingStructuresTest.o
OBJ_WriteMapHeaderTest = $(OBJ_mappings) WriteMapHeaderTest.o
OBJ_GeneratePointsFromParamFileTest = $(OBJ_mappings) GeneratePointsFromParamFileTest.o
OBJ_VectorStateToOrbitalParametersTest = $(OBJ_mappings) VectorStateToOrbitalParametersTest.o
OBJ_InitKernelTest = $(OBJ_montecarlo) InitKernelTest.o
OBJ_WetzelFunctions = $(OBJ_mappings) WetzelFunctions.o

All: Rotation3DAngleTest OrbitalParametersToStateVectorTest GeneratePointFromInputFunctionTest ExtractJsonTest InitConstantKernelTest InitConstrainInfoTest InitUniformMontecarloMappingStructuresTest WriteMapHeaderTest GeneratePointsFromParamFileTest VectorStateToOrbitalParametersTest InitKernelTest WetzelFunctions
	echo "All .out test files are done"

WetzelFunctions: $(OBJ_WetzelFunctions)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "WetzelFunctions.out" done

InitKernelTest: $(OBJ_InitKernelTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "InitKernelTest.out done"

VectorStateToOrbitalParametersTest: $(OBJ_VectorStateToOrbitalParametersTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "VectorStateToOrbitalParametersTest.out done"

GeneratePointsFromParamFileTest: $(OBJ_GeneratePointsFromParamFileTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "GeneratePointsFromParamFileTest.out done"

WriteMapHeaderTest: $(OBJ_WriteMapHeaderTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "WriteMapHeaderTest.out done"

InitUniformMontecarloMappingStructuresTest: $(OBJ_InitUniformMontecarloMappingStructuresTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "InitUniformMontecarloMappingStructuresTest.out done"

InitConstrainInfoTest: $(OBJ_InitConstrainInfoTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "InitConstrainInfoTest.out done"

ExtractJsonTest: $(OBJ_ExtractJsonTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "ExtractJsonTest.out done"

InitConstantKernelTest: $(OBJ_InitConstantKernelTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "InitConstantKernelTest.out done"

GeneratePointFromInputFunctionTest: $(OBJ_GeneratePointFromInputFunctionTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "GeneratePointFromInputFunctionTest.out done"

Rotation3DAngleTest: $(OBJ_Rotation3DAngleTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "Rotation3DAngleTest.out done"

OrbitalParametersToStateVectorTest: $(OBJ_OrbitalParametersToStateVectorTest)
	$(CC) -Wall -o $@.out $^ $(CFLAGS) $(LFLAGS)
	echo "OrbitalParametersToStateVectorTest.out done"

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LFLAGS)

clean:
	rm *.o* ../*.o *.~ ../*.~ 

