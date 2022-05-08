#include "Rotation3DAngleTest.h"

int main(){

  double v[2*DIM] = {0,0, //vector on which rotations are done
		     0,0,
		     0,0};
  
  double v0[2*DIM] = {0,0, //always equal to cero to modify values of v
		      0,0,
		      0,0};

  char axis[DIM] = "xyz";  
  double angle[DIM] = {M_PI/6. , M_PI/4., M_PI/3.}; 
  
    
  int i,j,k; 
  for(i=0; i<DIM; i++){
    printf("%s","------------------------------------------------------------------------\n");
    printf("Rotation about %c\n",axis[i]);
    for(j=0; j<DIM; j++){
      printf("%s","\t----------------------------------------------------------------\n");
      printf("\tR%c(%.0lf)\n",axis[i],angle[j]*RTD); 
      PrintRotationMatrix(angle[j],axis[i]);
      for(k=0;k<DIM; k++){
	printf("\t\tR%c(e%c, e%c)\n",axis[i],'x'+k,'x'+k); 
	v[2*k]=1; v[2*k+1]=1;
	Rotation3DAngle(v , angle[j], axis[i]);
	PrintMatrix(2,3,2,v);
	for (int s=0; s<2*DIM; s++)v[s] = v0[s]; 

      }
    }
    printf("%s","\n\n"); 
  }
  
  
  
  return 0; 
}

int PrintMatrix(const int columns, const int rows, const int tabs,const double * const v){
  int i, j; 
  for (i=0; i<rows; i++){
    for(int t = 0; t<tabs; t++)printf("\t");
    for(j=0; j<columns; j++){
      printf("%lf ",v[i*columns+j]); 
    }
    printf("%s","\n"); 
  }

  return 0;
} 

int PrintRotationMatrix(const double theta, const char axis){


  double c=cos(theta), s=sin(theta); 
  
  double rz[DIM*DIM] = {c,-s,0,
			s,c,0,
			0,0,1};
  
  double ry[DIM*DIM] = {c,0,s,
			0,1,0,
			-s,0,c};
  
  double rx[DIM*DIM] = {1,0,0,
			0,c,-s,
			0,s,c};

  
  if ('x'==axis)
    PrintMatrix(DIM,DIM,1,rx);
  else if('y'==axis)
    PrintMatrix(DIM,DIM,1,ry);
  else
    PrintMatrix(DIM,DIM,1,rz);
  

  return 0; 
}
