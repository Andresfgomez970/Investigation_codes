#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE * ptr_file;
    char dummy_string[1000];
    char * n_stars;
    int i;
    ptr_file = fopen("DeclareHere.c", "w+");

    ptr_file.write("#define STAR *\n\n");

    int n_dim = 3;
    int * dims = {3, 2, 1};
    n_stars = (char *) malloc(sizeof(char) * n_dim);
    for (i = 0; i < n_dim; i++)
    {
        n_stars[i] = '*';
    }
    sprintf(dummy_string, "%s%s\n", "double", n_stars);  
    ptr_file.write(dummy_string);

    ptr_file.write("");
    for (i = 0; i < n_dim; i++){
        ptr_file.write("for(i%d = 0; i%d < dims[%d]; i%d++)")
    }
    

    return 0;
}
