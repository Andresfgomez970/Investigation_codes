#include <stdio.h>
#include <stdlib.h>

int try_cast_of_double_pointer_function(const double ** x){

    return 0;
}

int other_cast_of_double_pointer_function(double * const * const x){
    x[0][0] = 1;
    return 0;
}

int other2_cast_of_double_pointer_function(const double * const x){
    return 0;
}


/* 
int example1(){
    double ** x = (double **) malloc(sizeof(double *) * 1);
    x[0] = (double *) malloc(sizeof(double)  * 1);

     x[0][0] = 1;
    printf("%lf\n", x[0][0]);
    try_cast_of_double_pointer_function(x);
    x[0][0] = 4; // No problem, the nature of the origina pointer remains
    printf("%lf\n", x[0][0]);

    free(x);
    free(x[0]);
} */

int main(int argc, char const *argv[])
{
    double * const * const xp;
    double ** x;
    other_cast_of_double_pointer_function(x);
    other2_cast_of_double_pointer_function(*x);
    other_cast_of_double_pointer_function(x);


/*     const int * a;
    const int * b;
    int * const c;

    printf("%p, %p\n", b, c);
    b = c;
    printf("%p, %p", b, c); */
/* 
    int ** a;
    const int ** b;
    int ** c;
    printf("%p, %p, %p\n", a, b, c);
    c = b;
    c = a;
    printf("%p, %p, %p",a, b, c); */





    return 0;
}
