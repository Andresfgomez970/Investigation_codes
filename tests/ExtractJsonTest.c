/*
 * Exploring the types of json-c.
 *
 * clang -Wall -I/usr/include/json-c/ -o json_types json_types.c -ljson-c
 */
#include "ExtractJsonTest.h"

int ExtractKernelInfoJson();

_Bool eq_floats(double a, double b, double epsilon)
{
  return fabs(a - b) < epsilon;
}

int main(int argc, char const *argv[]){
	int check_state;
	 
	check_state = ExtractKernelInfoJson();

	printf("\n****************************** %s *****************************************\n", argv[0]);
    printf("ExtractJsonTest pass the test? %d", check_state);
    printf("\n****************************** %s *****************************************\n\n", argv[0]);

	return 0;
}

int ExtractKernelInfoJson(){
	char * str = 0;
	char * filename = "ExtractJsonTest_params.json";

	struct json_object *jobj;
	int n_dim = 0;
	double * val_0 = NULL;
	double * val_f = NULL;
	char key[1000];

	int check_state = 1;
	
	// Get the sring from the file
	FileToStrting(filename, &str);
	printf("Read file:\n%s\n", str);

	// Parse input file and read arguments
	jobj = json_tokener_parse(str);
 
	sprintf(key, "n_dim"); 
	n_dim = ExtractJsonInt(jobj, key, !OPTIONAL);

	sprintf(key, "val_0");
	ExtractJsonDoubleArray(jobj, key, &val_0, !OPTIONAL);

	sprintf(key, "val_f");
	ExtractJsonDoubleArray(jobj, key, &val_f, !OPTIONAL);

	// Varify results
 	check_state = (n_dim==2) && eq_floats(val_0[0], 0, 1e-6) && eq_floats(val_0[1], -3, 1e-6);
	check_state = check_state && eq_floats(val_f[0], 1, 1e-6) && eq_floats(val_f[1], 3, 1e-6); 
	printf("Expected results? %d\n", check_state); 

	free(str);
	json_object_put(jobj);
	free(val_0);
	free(val_f);

	return check_state;
}