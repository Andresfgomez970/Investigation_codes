#ifndef _two_body_analytical_h
#define _two_body_analytical_h

#include<stdlib.h>
#include<math.h>
#include<stdio.h>
/*#include<gsl/gsl_blas.h>*/
#include<string.h>
#include"input_output.h"

#define DIM 3
/*
#define X 0
#define Y 1
#define Z 2
*/
#define RTD 180/M_PI
#define DTR M_PI/180


typedef struct
{
  double rmin;
  double f;
  double e; 

  double w;
  double i;
  double W;


} orbital_Param;

int Rotation3DAngle( double * const v, const double theta , const char axis);
int EulerAnglesRotation(double * const v, const double w, const double i, const double W);
int OrbitalParametersToStateVector(double * const v, const orbital_Param orbital_param, const double mu);


// Print functions
int PrintOrbitalState(const orbital_Param orbital_param);


#endif // ! _two_body_analytical
