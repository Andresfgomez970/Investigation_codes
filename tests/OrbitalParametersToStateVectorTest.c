#include "OrbitalParametersToStateVectorTest.h"

orbital_Param or_par;
double mu;
double v[DIM*3];
double v_expected[DIM*3];

int main(){

  //example1();
  //example2(); 
  example3(); 

  return 0; 
}

void example1(){
  mu = 2.9591230378107436E-04; //au^3/d^2; also from page
  or_par.rmin = 1.381298336447610E+00; //QR= 1.381298336447610E+00 ; in au
  or_par.e = 9.344675738214817E-02; //EC= 9.344675738214817E-02
  or_par.f = 1.598142048039505E+02*DTR;  //TA= 1.598142048039505E+02

  or_par.w = 2.866935928454845E+02*DTR; //w = 2.866935928454845E+02
  or_par.i = 1.848055550084186E+00*DTR; //IN= 1.848055550084186E+00
  or_par.W = 4.950274768153412E+01*DTR; //OM= 4.950274768153412E+01

  //PrintOrbitalState(or_par); 
  
  // in au and au/day
  v_expected[0] =-1.192813057069849E+00; //X =-1.192813057069849E+00
  v_expected[2] = 1.156903896057365E+00; //Y = 1.156903896057365E+00
  v_expected[4] = 5.327519536043217E-02; //Z = 5.327519536043217E-02
  v_expected[1] =-9.201393931257292E-03; //VX=-9.201393931257292E-03
  v_expected[3] =-8.874238964848019E-03; //VY=-8.874238964848019E-03 
  v_expected[5] = 3.983194436854145E-05; //VZ= 3.983194436854145E-05
     
  
  printf("Take Orbital Parameters for MARS retrieved from https://ssd.jpl.nasa.gov/horizons.cgi#results and verify with the data in \n https://ssd.jpl.nasa.gov/horizons.cgi#top for 2019-Jul-11 00:00:00.0000 TDB\n");

  OrbitalParametersToStateVector(v,or_par,mu); 

  printf("%s","Obtained result\n"); 
  PrintMatrixArray(v,2,3); 
  printf("distaince: %lf\n", (1+or_par.e)*or_par.rmin/( 1 + or_par.e*cos(or_par.f) ) ); 

  
  printf("%s","Expected result\n"); 
  PrintMatrixArray(v_expected,2,3); 
  printf("distaince: %lf\n", sqrt(v_expected[0]*v_expected[0]+v_expected[2]*v_expected[2]+v_expected[4]*v_expected[4]) ); 
  
  printf("%s","The result has considerable error, perhaps from the fact the sun was used as a focus and the position of Mars were \n treated as the relative coordinates in a two body problem\n\n"); 
}

void example2(){
  // Example with analytical simple 
  printf("%s", "Simpler example from analytical calculation: let us suppose:\n");

  or_par.rmin = 1; //QR= 1.381298336447610E+00 ; in au
  or_par.e = 0.87; //EC= 9.344675738214817E-02
  or_par.f = 0;  //TA= 1.598142048039505E+02

  or_par.w = 0; //w = 2.866935928454845E+02
  or_par.i = 0; //IN= 1.848055550084186E+00
  or_par.W = 0; //OM= 4.950274768153412E+01

  mu = 1.87; 

  printf("%.100f\n",mu); 
  
  PrintOrbitalState(or_par);
  printf("%s","\n"); 

  OrbitalParametersToStateVector(v,or_par,mu); 
  
  printf("%s","Obtained result\n"); 
  PrintMatrixArray(v,2,3); 
    
}

void example3(){
  // Example with analytical simple 
  printf("%s", "Simpler example from analytical calculation: let us suppose:\n");

  or_par.rmin = 1; //QR= 1.381298336447610E+00 ; in au
  or_par.e = 0.87; //EC= 9.344675738214817E-02
  or_par.f = 0;  //TA= 1.598142048039505E+02

  or_par.w = M_PI/6; //w = 2.866935928454845E+02
  or_par.i = M_PI/3; //IN= 1.848055550084186E+00
  or_par.W = M_PI/4; //OM= 4.950274768153412E+01

  mu = 1.87; 

  printf("%.100f\n",mu); 
  
  PrintOrbitalState(or_par);
  printf("%s","\n"); 

  OrbitalParametersToStateVector(v,or_par,mu); 
  
  printf("%s","Obtained result\n"); 
  PrintMatrixArray(v,2,3); 
}
