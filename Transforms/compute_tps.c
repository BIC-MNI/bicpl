/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_tps.c
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: library of routines for warping/mapping transformations.
@METHOD     : The original source code for these routines are taken from
              program VOI, written by Weiqian Dai.
@GLOBALS    : 
@CALLS      : 
@CREATED    : Dec 2, 1991 LC
@MODIFIED   : Mon Apr  5 09:00:54 EST 1993 louis 
                - building new library routines, with prototypes
---------------------------------------------------------------------------- */



#include <internal_volume_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <recipes.h>
#include <geom.h>


/* prototype definitions: */

private void  makeL(float **bdefor, float **ML, int num_marks, int dim);

private float  return_r(float *cor1, float *cor2, int dim);

private float  FU(float r, int dim);

private void   calculate_coe(float **adefor, float **INVML, float **INVMLY, int num_marks, int dim);



/* This function will get coefficients of the warping function. */

public void  get_nonlinear_warp(float **bdefor, /* num_marks x dim */
                                float **adefor, /* num_marks x dim */
                                float **INVMLY, /* num_marks+1+dim x dim */
                                int num_marks,
                                int dim)
{

  
   float **ML,**INVML;

   ML    = matrix(1,num_marks+dim+1,1,num_marks+dim+1);
   INVML = matrix(1,num_marks+dim+1,1,num_marks+dim+1);

   /* This function will build the L matrix */
   makeL(bdefor,ML,num_marks,dim);

   invertmatrix(num_marks+dim+1, ML, INVML);

   /*  build the array of deformation vectors   */
   calculate_coe(adefor,INVML,INVMLY, num_marks, dim);


   free_matrix(ML,1,num_marks+dim+1,1,num_marks+dim+1);
   free_matrix(INVML,1,num_marks+dim+1,1,num_marks+dim+1);
}

				/* this function will build the L matrix for image deformation.
				 * ML  -   store the L matrix.	
				 * num_marks - is integer for number of landmarks.
				 * dim -  This is the dimension of the image.
				 *        dim = 2 for the 2-D image deformation.
				 *        dim = 3 for the 3-D image deformation.
				 */
private void makeL(float **bdefor, float **ML, int num_marks, int dim)
{
   int i,j;
   float r;
 
   /* initial matrix */
   for (i=1; i<=num_marks+dim+1; i++){
      for (j=1; j<=num_marks+dim+1; j++){
         ML[i][j]=0;
      }
   }
    
   /* set reset of the K matrix as follows */
   for (i=1;i<=num_marks;i++){
      for (j=i+1;j<=num_marks;j++){
         r = return_r(bdefor[i],bdefor[j],dim); 
         ML[j][i] = ML[i][j] = FU(r,dim);
      }
   }
 
   /* set the rest of the L matrix */
   for (i=1;i<=num_marks;i++){
      ML[num_marks+1][i] = ML[i][num_marks+1]   = 1;
      for (j=1;j<=dim;j++){
         ML[num_marks+j+1][i] = ML[i][num_marks+j+1] = bdefor[i][j];
      }
   }
}

private float return_r(float *cor1, float *cor2, int dim)
{
   float r1,r2,r3;


   if (dim == 1){
      r1 = cor1[1] - cor2[1]; 
      r1 = fabs(r1);
      return(r1);
   } else
   if (dim == 2){
      r1 = cor1[1] - cor2[1];
      r2 = cor1[2] - cor2[2];
      return(r1*r1+r2*r2);
   } else
   if (dim == 3){
      r1 = cor1[1] - cor2[1];
      r2 = cor1[2] - cor2[2];
      r3 = cor1[3] - cor2[3];
      return((float) sqrt((float) (r1*r1 + r2*r2 + r3*r3)) );
   } 
   else { 
      (void) fprintf(stderr,
                     " impossible error in mapping.c, dim = %d (return_r)\n",
                     dim);
      exit(-1); 
#ifdef lint
      return( 0.0 );
#endif
   }
}

private float FU(float r, int dim)
/* This function will calculate the U(r) function.
 * if dim = 1, the funtion returns |r|^3 
 * if dim = 2, the funtion retruns r^2*log r^2
 * if dim = 3, the funtion returns |r|
 */ 
{
   float z;

   if (dim==1){
      z = r*r*r;
      return(fabs(z));
   } else
   if (dim==2){/* r is stored as r^2 */
      z = r * log((double) r);
      return(z);
   } else
   if (dim==3){
      return(fabs(r));
   } else { 
      (void) fprintf(stderr,
                     " impossible error in mapping.c, dim = %d (FU)\n",
                     dim);
      exit(-1); 
#ifdef lint
      return( 0.0 );
#endif
   }
}
 
private void calculate_coe(float **adefor, float **INVML, float **INVMLY, int num_marks, int dim)
{
   int i,j,k;
   float temp,**YM;
   
   /* Y = ( V | 0 0 0)t */
   YM = matrix(1, num_marks+dim+1, 1, dim);
   for (i = 1; i<=num_marks; i++){ 
      for (j=1;j<=dim;j++){
         YM[i][j] = adefor[i][j];
      }
   }
 
   for (i = num_marks+1; i<=num_marks+dim+1; i++){
      for (j=1;j<=dim;j++){
         YM[i][j] = 0;
      }
   }
 
   /* L_{-1} Y = (W|a_{1}, a_{x}, a_{y})^T. */
   for (i=1;i<=dim;i++){
      for (j=1;j<= num_marks+dim+1;j++){ /* for one row of matrix */
         temp = 0;
         for (k = 1; k<= num_marks+dim+1; k++){
            temp = INVML[j][k]*YM[k][i] + temp;
         }
         INVMLY[j][i] = temp;
      }
   }
} 
