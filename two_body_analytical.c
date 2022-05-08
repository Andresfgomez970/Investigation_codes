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
  double hop =  pow(mu / (orbital_param.rmin * (1 + orbital_param.e)),  0.5);

  r = (1 + orbital_param.e) * orbital_param.rmin / (1 + orbital_param.e * cos(orbital_param.f));

  double v_orbit_plane[2*DIM];
  v_orbit_plane[0] = r * cos(orbital_param.f);
  v_orbit_plane[1] = -hop * sin(orbital_param.f);
  v_orbit_plane[2] = r * sin(orbital_param.f);
  v_orbit_plane[3] = hop * (orbital_param.e + cos(orbital_param.f));
  v_orbit_plane[4] = 0;
  v_orbit_plane[5] = 0;


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


// The following functions are not part of the principal program but are useful
//  to verify some result and values.
/////////////////////////
// 4.
////////////////////////
int VectorStateToOrbitalParameters(double mu, double *v, orbital_Param * orbital_param)
{
  double i_u[DIM] = {1,0,0};
  double j_u[DIM] = {0,1,0};
  double k_u[DIM] = {0,0,1};
  double h[DIM], n[DIM], pos[DIM], vel[DIM], ev[DIM], dummy1[DIM], dummy2[DIM];
  double dummy3;

  // This is the heuristic significant figures within minimum precision is
  //  expected to be obtained; it is done to do not prone in numerical errors
  //  in possible calculations.
  int fig_precision = 12;

  AssignVector(v, pos, vel);

  // Specific angular momentum and node vector //
  CrossProduct(pos, vel, h);
  dummy3 = Norm(h);
  RoundToNDecimal (&dummy3, fig_precision);
  
  // This means that the orbit is in the plane xy and we asumen node vector in x
  if (dummy3 < pow(10, -fig_precision) && dummy3 > pow(-10, -fig_precision)){
      n[X]=1; n[Y]=0; n[Z]=0;
  }
  else
    CrossProduct(k_u, h, n);

  //////////////////////////
  /// Eccentricity: ecc vecor
  //////////////////////////
  // calcualtion of (v x h) / mu
  CrossProduct(vel, h, dummy1);
  MultiplyVectorByC(dummy1, 1. / mu, dummy1);
  // calculation of r/norm(r)
  MultiplyVectorByC(pos, -1. / Norm(pos) , dummy2);
  // e vector and norm
  SumVectors(dummy1, dummy2, ev);
  orbital_param[0].e  = Norm(ev);

  /////////////////////
  // Ascending node
  ////////////////////
  dummy3 = DotProduct(n, j_u);
  RoundToNDecimal(&dummy3, fig_precision);

  // Check case of W  0 or 180 separately to avoid computational problems
  if (dummy3 < pow(10, -fig_precision) && dummy3 > pow(-10, -fig_precision)){
      if (n[X] > 0){
        orbital_param[0].W = 0; // Orbit in the plane xy
      }
      else
        orbital_param[0].W = M_PI;
  }
  else if (DotProduct(n, j_u)  >= 0)
    {
      dummy3 = DotProduct(n, i_u)/Norm(n);
      orbital_param[0].W = acos ( dummy3 );
    }

  else
    {
      dummy3 = DotProduct(n,i_u)/Norm(n);
      orbital_param[0].W = 2*M_PI - acos( dummy3 );
    }

  /////////////////////
  // Orbit inclination
  ////////////////////
  orbital_param[0].i = acos( DotProduct(h, k_u) / Norm(h));

  /////////////////////
  // Argument of periapsis
  ////////////////////
  dummy3 = Norm(ev);
  RoundToNDecimal(&dummy3, fig_precision);

  if (dummy3 < 1e-11 && dummy3 > -1e11){
      orbital_param[0].w = 0;
  }
  else{
    dummy3 = DotProduct(ev, n);
    dummy3 = dummy3/ (Norm(ev) * Norm(n));
    RoundToNDecimal(&dummy3, fig_precision);
    dummy3 = acos (dummy3);

    if (DotProduct(ev, k_u) >= 0){
      orbital_param[0].w = dummy3;
    }
    else
      orbital_param[0].w = 2*M_PI - dummy3;
  }

  ///////////////////
  // True anomaly
  //////////////////
  dummy3 = DotProduct(pos, vel);
  RoundToNDecimal(&dummy3 , fig_precision);
  dummy1[0] = DotProduct(ev, pos);
  dummy1[0] = dummy1[0] / (Norm(ev) * Norm(pos));
  
  if (dummy3 >= 0)
      orbital_param[0].f = acos(dummy1[0]);
  else
      orbital_param[0].f = 2*M_PI - acos(dummy1[0]);

  //////////////////
  // Minimum distance between bodies
  /////////////////

  orbital_param[0].rmin = Norm(h) * Norm(h) / mu / (1 + orbital_param[0].e);

  //Print_Orbital_State(orbital_param);

  return 0;
}

/////////////////////////
// 5.
////////////////////////
int RoundToNDecimal(double *r,int n)
{

  *r = roundf(*r * pow(10 ,n) )/ pow(10,n);

  return 0;
}


/////////////////////////
// 6.
////////////////////////
int AssignVector(double *v, double *pos, double *vel)
{
  for (int i= 0; i < DIM; i++)
    {
      pos[i] = v[i*2];
      vel[i] = v[i*2+1];
    }

  return 0;
}


/////////////////////////
// 6.
////////////////////////
int CrossProduct(double r[DIM],double v[DIM], double h[DIM])
{

  h[X] = r[Y]*v[Z] - r[Z]*v[Y];
  h[Y] = r[Z]*v[X] - r[X]*v[Z];
  h[Z] = r[X]*v[Y] - r[Y]*v[X];

  return 0;
}


/////////////////////////
// 7.
////////////////////////
int MultiplyVectorByC(double v[DIM], double c, double r[DIM])
{

  for (int i= 0; i < DIM; i++)
    r[i] = v[i]*c;


  return 0;
}

/////////////////////////
// 8.
////////////////////////
int SumVectors(double v[DIM],double w[DIM], double sum[DIM] )
{

  for (int i= 0; i < DIM; i++)
    sum[i] = v[i] + w[i];

  return 0;
}

/////////////////////////
// 9.
////////////////////////
double DotProduct(double v[DIM],double w[DIM])
{
  double r = 0;

  for(int i = 0; i<DIM ; i++)
    r += v[i]*w[i];

  return r;
}


/////////////////////////
// 10.
////////////////////////
double Norm(double v[DIM])
{
  return sqrt( v[X]*v[X] + v[Y]*v[Y] + v[Z]*v[Z] );
}


///////////////////////////////////////////////// Print functions //////////////////////////////////////////////////////

/////////////////////////
// 11.
////////////////////////
int PrintOrbitalState(const orbital_Param orbital_param)
{
  printf("The orbital parameters are: rmin %lf , f %lf, e %lf, w %lf, i %lf, W %lf",orbital_param.rmin,DoubleModule(orbital_param.f*RTD, 360),
	 orbital_param.e,DoubleModule(orbital_param.w*RTD, 360),orbital_param.i*RTD,DoubleModule(orbital_param.W*RTD, 360));

  return 0;
}

/////////////////////////
// 12.
////////////////////////
int PrintVectorState(double *v)
{

  for (int i= 0; i < DIM; i++)
    {
      for (int j = 0; j < 2; j++)
        {
          printf("%lf ", v[i*2+j] );
        }
      printf("\n");
    }

  return 0;
}
