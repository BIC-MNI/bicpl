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
#include <geom.h>


/* prototype definitions: */

private  void  calculate_coe(
    Real    **adefor,
    Real    **INVML,
    float   **INVMLY,
    int     n_points,
    int     n_dims );

private  void  makeL(
    Real   **bdefor,
    Real   **ML,
    int    n_points,
    int    n_dims );

/* This function will get coefficients of the warping function. */

public  void  get_nonlinear_warp(
   Real     **bdefor,   /* n_points x n_dims */
   Real     **adefor,   /* n_points x n_dims */
   float    **INVMLY,   /* n_points+1+n_dims x n_dims */
   int      n_points,
   int      n_dims )
{
   Real    **ML,**INVML;

   ALLOC2D( ML, n_points+n_dims+1, n_points+n_dims+1 );
   ALLOC2D( INVML, n_points+n_dims+1, n_points+n_dims+1 );

   /* This function will build the L matrix */

   makeL( bdefor, ML, n_points, n_dims );

   (void) invert_square_matrix( n_points+n_dims+1, ML, INVML );

   /*  build the array of deformation vectors   */

   calculate_coe( adefor, INVML, INVMLY, n_points, n_dims );

   FREE2D( ML );
   FREE2D( INVML );
}

		/* this function will build the L matrix for image deformation.
		 * ML  -   store the L matrix.	
		 * n_points - is integer for number of landmarks.
		 * n_dims -  This is the dimension of the image.
		 *        n_dims = 2 for the 2-D image deformation.
		 *        n_dims = 3 for the 3-D image deformation.
		 */
private  void  makeL(
    Real   **bdefor,
    Real   **ML,
    int    n_points,
    int    n_dims )
{
    int    i,j;
    Real   fu;
 
    /* initialize matrix to zero */

    for_less( i, 0, n_points+n_dims+1 )
    {
        for_less( j, 0, n_points+n_dims+1 )
            ML[i][j]=0;
    }
    
    /* set reset of the K matrix as follows */

    for_less( i, 0, n_points )
    {
        for_less( j, i+1, n_points )
        {
            fu = thin_plate_spline_U( bdefor[i], bdefor[j], n_dims );
            ML[j][i] = ML[i][j] = fu;
        }
    }
 
    /* set the rest of the L matrix */

    for_less( i, 0, n_points )
    {
        ML[n_points][i] = ML[i][n_points]   = 1.0;
        for_less( j, 0, n_dims )
            ML[n_points+1+j][i] = ML[i][n_points+1+j] = bdefor[i][j];
    }
}

private  void  calculate_coe(
    Real    **adefor,
    Real    **INVML,
    float   **INVMLY,
    int     n_points,
    int     n_dims )
{
    int      i,j,k;
    Real     temp, **YM;
   
    /* Y = ( V | 0 0 0)t */

    ALLOC2D( YM, n_points+n_dims+1, n_dims );

    for_less( i, 0, n_points )
    {
        for_less( j, 0, n_dims )
            YM[i][j] = adefor[i][j];
    }
 
    for_less( i, n_points, n_points + n_dims + 1 )
    {
        for_less( j, 0, n_dims )
            YM[i][j] = 0;
    }
 
    /* L_{-1} Y = (W|a_{1}, a_{x}, a_{y})^T. */

    for_less( i, 0, n_dims )
    {
        for_less( j, 0, n_points + n_dims + 1 )   /* for one row of matrix */
        {

            temp = 0;
            for_less( k, 0,  n_points + n_dims + 1 )
                temp += INVML[j][k]*YM[k][i];

            INVMLY[j][i] = (float) temp;
        }
    }

    FREE2D( YM );
}
