#include "two_body_analytical.h"

//////////////////////////////////////////// Process functions /////////////////////////////////////////////////////////////


/*------------------------
This rotinte receive a  
--------------------------*/
int Rotation3DAngle( double * const v, const double theta , const char axis)
{

  int i; // counter for assignations 
  double c, s; //sin and cos

  c = cos(theta);
  s = sin(theta);
  
  double vo[] = {0,0,
		 0,0,
		 0,0};

  double rz[] = {c,-s,0,
		 s,c,0,
		 0,0,1};

  double ry[] = {c,0,s,
		0,1,0,
		-s,0,c};

  double rx[] = {1,0,0,
		0,c,-s,
		0,s,c};
  
  double r[] = {0,0,0,
		0,0,0,
		0,0,0};

  if (axis=='z')
    {
      for (i=0; i<3*DIM; i++)
	r[i] = rz[i];
    }

  else if (axis=='y')
    {
      for (i=0; i<3*DIM; i++)
	r[i] = ry[i];
    }

  else if (axis=='x')
    {
      for (i=0; i<3*DIM; i++)
	r[i] = rx[i];
    }
  
  else
    {
      printf("%s","The option enter for the rotation is invalid"); 
      exit(0);
    }

  
  
  for(i=0; i<2*DIM; i++)
    {
      vo[i] = v[i];
    }  
  
  /*
  gsl_matrix_view R = gsl_matrix_view_array(r, 3, 3);
  gsl_matrix_view Vn = gsl_matrix_view_array(v, 3, 2);  
  gsl_matrix_view Vo = gsl_matrix_view_array(vo, 3, 2);
  
  gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
		  1.0, &R.matrix, &Vo.matrix,
		  0.0, &Vn.matrix);
  */

  
  //Other option for the matrix multiplication
  int j, k;
  int Ncol=2, Nrows=3; 
  for (i = 0;  i < Nrows; i++)
    {
      for (j=0; j < Ncol; j++)
	{
	  v[i*Ncol+j] = 0; 
	  for(k=0; k < Nrows; k++)
	    {
	      v[i*Ncol+j] += r[i*Nrows+k] * vo[k*Ncol+j];
	    }
	}
    }

  
return 0; 
}



/////////////////////////
// 2.
////////////////////////
int EulerAnglesRotation(double * const v, const double w, const double i, const double W)
{
  Rotation3DAngle( v, w, 'z');
  Rotation3DAngle( v, i, 'x');
  Rotation3DAngle( v, W, 'z');

  return 0; 
}


/////////////////////////
// 3.
////////////////////////
int OrbitalParametersToStateVector(double * const v, const orbital_Param orbital_param, const double mu)
{
  double r;
  double hop =  pow( mu / ( orbital_param.rmin*(1+orbital_param.e) )  ,  0.5);
  
  r = (1+orbital_param.e)*orbital_param.rmin/( 1 + orbital_param.e*cos(orbital_param.f) );
  
  double v_orbit_plane[2*DIM] = {r*cos(orbital_param.f), -hop*sin(orbital_param.f),
				 r*sin(orbital_param.f), hop*( orbital_param.e + cos(orbital_param.f) ),
				 0,0};  

  if ( isinf(v_orbit_plane[0]) || isinf(v_orbit_plane[2]) ){
    printf("%s","isinf condition \n" );  
    // Do nothing; they are then returned as inf and then rejected
  }

  else{
    EulerAnglesRotation(v_orbit_plane, orbital_param.w, orbital_param.i, orbital_param.W); 
  }
  
  for (int i = 0 ; i<2*DIM; i++)
    v[i] = v_orbit_plane[i];  

  return 0; 
}

///////////////////////////////////////////////// Print functions //////////////////////////////////////////////////////

int PrintOrbitalState(const orbital_Param orbital_param)
{

  printf("The orbital parameters are: rmin %lf , f %lf, e %lf, w %lf, i %lf, W %lf",orbital_param.rmin,orbital_param.f*RTD,
	 orbital_param.e,orbital_param.w*RTD,orbital_param.i*RTD,orbital_param.W*RTD);

  return 0;
}
