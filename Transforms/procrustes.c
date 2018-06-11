/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              Peter Neelin, Louis Collins,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

/* ----------------------------- MNI Header -----------------------------------
@NAME       : procrustes.c
@DESCRIPTION: File containing routines for doing procrustes calculations.
@METHOD     : Contains routines :
                 procrustes
@CALLS      : 
@CREATED    : January 29, 1992 (Peter Neelin)
@MODIFIED   : February 7, 1992 (Peter Neelin)
                 - added routine transformations_to_homogeneous
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

#include "bicpl_internal.h"

static  VIO_Real  trace_of_matrix(
    int    size,
    VIO_Real   **the_matrix );

static  void  translate_points(
    int    npoints,
    int    ndim,
    VIO_Real   **points, 
    VIO_Real   translation[],
    VIO_Real   **newpoints);

static  void  calc_centroid(
    int     npoints,
    int     ndim,
    VIO_Real    **points, 
    VIO_Real    centroid[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : procrustes
@INPUT      : npoints - number of input point pairs
              ndim    - number of dimensions for each point
              Apoints - Matrix of point set 1, size npoints by ndim
              Bpoints - Matrix of point set 2, size npoints by ndim
@OUTPUT     : translation - vector, size ndim,
                 specifies the translation to be applied to Bpoints to line
                 up the centroid with that of Apoints. Calling routine must
                 allocate space for this vector.
              centre_of_rotation - vector, size ndim, that
                 specifies the centre of rotation and scaling (this is 
                 in fact only the centroid of Apoints). Calling routine must
                 allocate space for this vector.
              rotation - matrix, size ndim by ndim,
                 to rotate translated Bpoints so that they line up with 
                 Apoints. Calling routine must allocate space for this 
                 matrix.
              scale - Scalar value giving global scaling to be applied to
                 translated and rotated Bpoints to match Apoints.
@RETURNS    : (nothing)
@DESCRIPTION: Calculates n-dimensional linear transformation from one set 
              of points to another, minimizing distance between equivalent
              points. Transformation from Bpoints to Apoints is calculated.
@METHOD     : See Matrix Computations, Golub and Van Loan, pp. 425-426 and
              paper by Sibson, Robin, J.R.Statist.Soc. B(1978), Vol. 40,
              No. 2, pp 234-238.
              Steps of calculations are as follows :
                 1) Calculate translation that aligns the centroids of the
                    two point sets.
                 2) Calculate rotation/reflexion that minimizes residual.
                 3) Calculate scaling of points to minimize residual.
              The process can be broken into independent steps because the
              best translation aligns centroids independently of the choice
              of rotation/reflexion and scaling and the best rotation/reflexion
              can be found independently of scale (after the best translation
              has been found). (See Sibson for more).
@GLOBALS    : (none)
@CALLS      : calc_centroid
              translate_points
              transpose
              matrix_multiply
              trace_of_matrix
@CREATED    : Long time ago (Sean Marrett)
@MODIFIED   : Some time later (Shyan Ku)
              Feb. 26, 1990 (Weiqian Dai)
              January 30, 1992 (Peter Neelin)
                 - complete rewrite for roughly NIL-abiding code. Modified
                 name and calling parameters.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

BICAPI  void  procrustes(
    int         npoints,
    int         ndim, 
    VIO_Real        **Apoints,
    VIO_Real        **Bpoints,
    VIO_Real        translation[],
    VIO_Real        centre_of_rotation[],
    VIO_Transform   *rotation_transform,
    VIO_Real        *scale_ptr )
{
    int   i, j;
    VIO_Real  *Atranslation, *Btranslation, *svd_W;
    VIO_Real  **Ashift, **Bshift, **Atranspose, **Btranspose, **rotation;
    VIO_Real  **svd_V, **svd_VT;
    VIO_Real  **Brotated, **product;
    VIO_Real  trace1, trace2;
    VIO_Real  **svd_U;
                                   
    /* Get the vectors for centroids */

    ALLOC( Atranslation, ndim );
    ALLOC( Btranslation, ndim );
    ALLOC( svd_W, ndim );

    /* Get various matrices */

    VIO_ALLOC2D( rotation, ndim, ndim );
    VIO_ALLOC2D( Ashift, npoints, ndim );
    VIO_ALLOC2D( Bshift, npoints, ndim );
    VIO_ALLOC2D( Atranspose, ndim, npoints );
    VIO_ALLOC2D( Btranspose, ndim, npoints );
    VIO_ALLOC2D( svd_U, ndim, ndim );
    VIO_ALLOC2D( svd_V, ndim, ndim );
    VIO_ALLOC2D( svd_VT, ndim, ndim );
    VIO_ALLOC2D( Brotated, npoints, ndim );

    /* Calculate the centroids, remove them from A and B points and
       save the translation */

    calc_centroid( npoints, ndim, Apoints, centre_of_rotation ); 

    for_less( i, 0, ndim )
        Atranslation[i] = -centre_of_rotation[i];

    translate_points( npoints, ndim, Apoints, Atranslation, Ashift );
    calc_centroid( npoints, ndim, Bpoints, Btranslation ); 

    for_less( i, 0, ndim )
        Btranslation[i] *= -1.0;

    translate_points( npoints, ndim, Bpoints, Btranslation, Bshift );

    for_less( i, 0, ndim )
        translation[i] = Btranslation[i] - Atranslation[i];

    /* Calculate the rotation/reflexion matrix */

    transpose( npoints, ndim, Bshift, Btranspose );
    matrix_multiply( ndim, npoints, ndim, Btranspose, Ashift, svd_U );
    (void) singular_value_decomposition( ndim, ndim, svd_U, svd_W, svd_V );
    transpose( ndim, ndim, svd_V, svd_VT );
    matrix_multiply( ndim, ndim, ndim, svd_U, svd_VT, rotation );

    /* Calculate the scale */

    matrix_multiply( npoints, ndim, ndim, Bshift, rotation, Brotated );
    transpose( npoints, ndim, Ashift, Atranspose );

    trace1 = 0.0;
    for_less( i, 0, npoints )
    {
      for_less( j, 0, ndim )
      {
	trace1 += Brotated[i][j]*Atranspose[j][i];
      }
    }

    trace2 = 0.0;
    for_less( i, 0, npoints )
    {
      for_less( j, 0, ndim )
      {
	trace2 += Bshift[i][j]*Btranspose[j][i];
      }
    }

    

    if (trace2 != 0.0)
        *scale_ptr = trace1 / trace2;
    else
        *scale_ptr = 0.0;

    make_identity_transform( rotation_transform );

    for_less( i, 0, VIO_N_DIMENSIONS )
        for_less( j, 0, VIO_N_DIMENSIONS )
            Transform_elem( *rotation_transform, i, j ) = rotation[j][i];

    /* Free vectors */

    FREE( Atranslation );
    FREE( Btranslation );
    FREE( svd_W );

    /* Free matrices */

    VIO_FREE2D( rotation );
    VIO_FREE2D( Ashift );
    VIO_FREE2D( Bshift );
    VIO_FREE2D( Atranspose );
    VIO_FREE2D( Btranspose );
    VIO_FREE2D( svd_U );
    VIO_FREE2D( svd_V );
    VIO_FREE2D( svd_VT );
    VIO_FREE2D( Brotated );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calc_centroid
@INPUT      : npoints - number of points
              ndim    - number of dimensions
              points  - points matrix, ndim by npoints
@OUTPUT     : centroid - vector of centroid of points, size ndim
@RETURNS    : (nothing)
@DESCRIPTION: Calculates the centroid of a number of points in ndim dimensions.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code

---------------------------------------------------------------------------- */

static  void  calc_centroid(
    int     npoints,
    int     ndim,
    VIO_Real    **points, 
    VIO_Real    centroid[] )
{
    int d, p;

    /* Loop over each dimension */

    for_less( d, 0, ndim )
    {
         /* Add up points and divide by number of points */

         centroid[d] = 0.0;
         for_less( p, 0, npoints )
             centroid[d] += points[p][d];

         if( npoints > 0 )
             centroid[d] /= (VIO_Real) npoints;
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : translate_points
@INPUT      : npoints - number of points
              ndim    - number of dimensions
              points  - points matrix, size npoints by ndim
              translation - translation vector, size ndim
@OUTPUT     : newpoints - translated points matrix (see points). This matrix
                 can be the original points matrix.
@RETURNS    : (nothing)
@DESCRIPTION: Translates a set of points by a given translation.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

static  void  translate_points(
    int    npoints,
    int    ndim,
    VIO_Real   **points, 
    VIO_Real   translation[],
    VIO_Real   **newpoints)
{
    int p, d;

    for_less( p, 0, npoints )
    {
        for_less( d, 0, ndim )
            newpoints[p][d] = points[p][d] + translation[d];
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : trace_of_matrix
@INPUT      : size   - size of the_matrix (the_matrix should be square)
              the_matrix - matrix for which trace should be calculated
@OUTPUT     : (none)
@RETURNS    : trace of matrix
@DESCRIPTION: Calculates the trace of a matrix, product of diagonal elements.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

static  VIO_Real  trace_of_matrix(
    int    size,
    VIO_Real   **the_matrix )
{
    int  i;
    VIO_Real sum;

    sum = 0.0;

    for_less( i, 0, size )
        sum += the_matrix[i][i];

    return( sum );
}
