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

#include <internal_volume_io.h>
#include <trans.h>
#include <numerical.h>

private  Real  trace_of_matrix(
    int    size,
    Real   **the_matrix );

private  void  translate_points(
    int    npoints,
    int    ndim,
    Real   **points, 
    Real   translation[],
    Real   **newpoints);

private  void  calc_centroid(
    int     npoints,
    int     ndim,
    Real    **points, 
    Real    centroid[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : procrustes
@INPUT      : npoints - number of input point pairs
              ndim    - number of dimensions for each point
              Apoints - Matrix of point set 1 (in numerical recipes
                 form). The dimensions of this matrix should be defined
                 to be 1 to npoints and 1 to ndim (when calling the numerical
                 recipes routine matrix).
              Bpoints - Matrix of point set 2 (in numerical recipes
                 form). The dimensions of this matrix should be defined
                 to be 1 to npoints and 1 to ndim (when calling the numerical
                 recipes routine matrix).
@OUTPUT     : translation - Numerical recipes vector (1 to ndim) that 
                 specifies the translation to be applied to Bpoints to line
                 up the centroid with that of Apoints. Calling routine must
                 allocate space for this vector.
              centre_of_rotation - Numerical recipes vector (1 to ndim) that
                 specifies the centre of rotation and scaling (this is 
                 in fact only the centroid of Apoints). Calling routine must
                 allocate space for this vector.
              rotation - Numerical recipes matrix (1 to ndim by 1 to ndim) 
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
@CALLS      : numerical recipes stuff
              calc_centroid
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

public  void  procrustes(
    int         npoints,
    int         ndim, 
    Real        **Apoints,
    Real        **Bpoints,
    Real        translation[],
    Real        centre_of_rotation[],
    Transform   *rotation_transform,
    Real        *scale_ptr )
{
    int   i, j;
    Real  *Atranslation, *Btranslation, *svd_W;
    Real  **Ashift, **Bshift, **Atranspose, **Btranspose, **rotation;
    Real  **svd_V, **svd_VT;
    Real  **Brotated, **product;
    Real  trace1, trace2;
    Real  **svd_U;
                                   
    /* Get the vectors for centroids */

    ALLOC( Atranslation, ndim );
    ALLOC( Btranslation, ndim );
    ALLOC( svd_W, ndim );

    /* Get various matrices */

    ALLOC2D( rotation, ndim, ndim );
    ALLOC2D( Ashift, npoints, ndim );
    ALLOC2D( Bshift, npoints, ndim );
    ALLOC2D( Atranspose, ndim, npoints );
    ALLOC2D( Btranspose, ndim, npoints );
    ALLOC2D( svd_U, ndim, ndim );
    ALLOC2D( svd_V, ndim, ndim );
    ALLOC2D( svd_VT, ndim, ndim );
    ALLOC2D( Brotated, npoints, ndim );
    ALLOC2D( product, npoints, npoints );

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
    matrix_multiply( npoints, ndim, npoints, Brotated, Atranspose, product );
    trace1 = trace_of_matrix( npoints, product );
    matrix_multiply( npoints, ndim, npoints, Bshift, Btranspose, product );
    trace2 = trace_of_matrix( npoints, product );

    if (trace2 != 0.0)
        *scale_ptr = trace1 / trace2;
    else
        *scale_ptr = 0.0;

    make_identity_transform( rotation_transform );

    for_less( i, 0, N_DIMENSIONS )
        for_less( j, 0, N_DIMENSIONS )
            Transform_elem( *rotation_transform, i, j ) = rotation[j][i];

    /* Free vectors */

    FREE( Atranslation );
    FREE( Btranslation );
    FREE( svd_W );

    /* Free matrices */

    FREE2D( rotation );
    FREE2D( Ashift );
    FREE2D( Bshift );
    FREE2D( Atranspose );
    FREE2D( Btranspose );
    FREE2D( svd_U );
    FREE2D( svd_V );
    FREE2D( svd_VT );
    FREE2D( Brotated );
    FREE2D( product );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : calc_centroid
@INPUT      : npoints - number of points
              ndim    - number of dimensions
              points  - points matrix (in numerical recipes form).
                 The dimensions of this matrix should be defined to be 
                 1 to npoints and 1 to ndim (when calling the numerical 
                 recipes routine matrix).
@OUTPUT     : centroid - vector of centroid of points (in num. rec. form)
                 This vector should run from 1 to ndim.
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

private  void  calc_centroid(
    int     npoints,
    int     ndim,
    Real    **points, 
    Real    centroid[] )
{
    int i, j;

    /* Loop over each dimension */

    for_less( i, 0, ndim )
    {
         /* Add up points and divide by number of points */

         centroid[i] = 0.0;
         for_less( j, 0, npoints )
             centroid[i] += points[j][i];

         if( npoints > 0 )
             centroid[i] /= (Real) npoints;
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : translate_points
@INPUT      : npoints - number of points
              ndim    - number of dimensions
              points  - points matrix (in numerical recipes form).
                 The dimensions of this matrix should be defined to be 
                 1 to npoints and 1 to ndim (when calling the numerical 
                 recipes routine matrix).
              translation - translation vector (in num. rec. form, running
                 from 1 to ndim).
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

private  void  translate_points(
    int    npoints,
    int    ndim,
    Real   **points, 
    Real   translation[],
    Real   **newpoints)
{
    int i, j;

    for_less( i, 0, npoints )
    {
        for_less( j, 0, ndim )
            newpoints[i][j] = points[i][j] + translation[j];
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : trace_of_matrix
@INPUT      : size   - size of the_matrix (the_matrix should be square)
              the_matrix - matrix for which trace should be calculated (in 
                 numerical recipes form). Dimensions are 1 to size and 
                 1 to size.
@OUTPUT     : (none)
@RETURNS    : trace of matrix
@DESCRIPTION: Calculates the trace of a matrix.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  Real  trace_of_matrix(
    int    size,
    Real   **the_matrix )
{
    int  i;
    Real sum;

    sum = 0.0;

    for_less( i, 0, size )
        sum += the_matrix[i][i];

    return( sum );
}
