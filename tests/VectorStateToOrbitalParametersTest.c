#include "VectorStateToOrbitalParametersTest.h"

// Here we test the correspondence between 100 generate random vector states
//  and 100 random orbital paramete vectors. 
// For that we transfrom from on to the other and check that the inverse
//  transfro indepedently programmed lead to the same value.


int CheckGeneratedVectorStates();
int CheckGeneratedOrbitalElements();

int main(int argc, char const *argv[])
{
/*     double state_vector[2*DIM] = {17.95, 0, -17.47, 0, 0.0001, 192.00};
    double mu = ((0.16 + 70.19 + 5.59 + 2.39) + (0.03 + 3.05 + 0.11 + 0.03)) * 43007.1;
    orbital_Param * orbital_param = (orbital_Param *) malloc(sizeof(orbital_Param) * 1);

    VectorStateToOrbitalParameters(mu, state_vector, orbital_param);
    PrintOrbitalState(orbital_param[0]);
    printf("\n");

    free(orbital_param);
*/
    // Calcularing a 0 difference
    double x[2*DIM] = {3.797121, 179.999918 * M_PI / 180., 0.736724, 180.005186 * M_PI / 180., 90.000000 * M_PI / 180., 315.776406 * M_PI / 180.};
    double param[4] = {17.95, -17.47, 192, 3507229.005};

    printf("%lf", ChiSquareObservables(param, x));
    return 0;
}
