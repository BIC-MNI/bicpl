/* ----------------------------- MNI Header -----------------------------------
@NAME       : procrustes.c
@DESCRIPTION: File containing routines for doing procrustes calculations.
@METHOD     : Contains routines :
                 procrustes
                 transformations_to_homogeneous
                 translation_to_homogeneous
                 rotation_to_homogeneous
@CALLS      : 
@CREATED    : January 29, 1992 (Peter Neelin)
@MODIFIED   : February 7, 1992 (Peter Neelin)
                 - added routine transformations_to_homogeneous
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

#include <internal_volume_io.h>
#include <geom.h>
#include <numerical.h>

private  void  translation_to_homogeneous(
    int    ndim,
    Real   translation[],
    Real  **transformation );

private   void  rotation_to_homogeneous(
    int     ndim,
    Real    **rotation,
    Real    **transformation );

private  void  matrix_scalar_multiply(
    int     rows,
    int     cols,
    Real    scalar, 
    Real    **the_matrix,
    Real    **product );

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
    int     npoints,
    int     ndim, 
    Real    **Apoints,
    Real    **Bpoints,
    Real    translation[],
    Real    centre_of_rotation[],
    Real    **rotation,
    Real    *scale_ptr )
{
    int   i;
    Real  *Atranslation, *Btranslation, *svd_W;
    Real  **Ashift, **Bshift, **Atranspose, **Btranspose;
    Real  **svd_V, **svd_VT;
    Real  **Brotated, **product;
    Real  trace1, trace2;
    Real  **svd_U;
                                   
    /* Get the vectors for centroids */

    ALLOC( Atranslation, ndim );
    ALLOC( Btranslation, ndim );
    ALLOC( svd_W, ndim );

    /* Get various matrices */

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

    /* Free vectors */

    FREE( Atranslation );
    FREE( Btranslation );
    FREE( svd_W );

    /* Free matrices */

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
@NAME       : transformations_to_homogeneous
@INPUT      : ndim    - number of dimensions
              translation - Numerical recipes vector (1 to ndim) that 
                 specifies the translation to be applied first.
              centre_of_rotation - Numerical recipes vector (1 to ndim) that
                 specifies the centre of rotation and scaling.
              rotation - Numerical recipes matrix (1 to ndim by 1 to ndim) 
                 for rotation about centre_of_rotation (applied after 
                 translation). Note that this matrix need not only specify
                 rotation/reflexion - any ndim x ndim matrix will work.
              scale - Scalar value giving global scaling to be applied after
                 translation and rotation.
@OUTPUT     : transformation - Numerical recipes matrix (1 to ndim+1 by
                 1 to ndim+1) specifying the transformation for homogeneous 
                 coordinates. To apply this transformation, a point
                 vector should be post-multiplied by this matrix, with the
                 last coordinate of the ndim+1 point vector having value
                 one. The calling routine must allocate space for this
                 matrix.
@RETURNS    : (nothing)
@DESCRIPTION: Computes a transformation matrix in homogeneous coordinates
              given a translation, a rotation matrix (or other 
              non-homogeneous matrix) and a global scaling factor.
              Transformations are applied in that order.
@METHOD     : Apply the following operations (multiply from left to right):
                 1) Translate by translation
                 2) Translate by -centre_of_rotation
                 3) Rotate
                 4) Scale
                 5) Translate by centre_of_rotation
@GLOBALS    : (none)
@CALLS      : numerical recipes stuff
              translation_to_homogeneous
              matrix_multiply
              matrix_scalar_multiply
@CREATED    : February 7, 1992 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  transformations_to_homogeneous(
    int     ndim, 
    Real    translation[],
    Real    centre_of_rotation[],
    Real    **rotation,
    Real    scaling,
    Real  **transformation )
{
    int   i;
    int   size;
    Real  *centre_translate;
    Real  **trans1, **trans2;
    Real  **trans_temp, **rotation_and_scale;

    size = ndim + 1;

    /* Allocate matrices and vectors */

    ALLOC( centre_translate, ndim );
    ALLOC2D( trans1, size, size );
    ALLOC2D( trans2, size, size );
    ALLOC2D( trans_temp, size, size );
    ALLOC2D( rotation_and_scale, ndim, ndim );

    /* Construct translation matrix */

    translation_to_homogeneous( ndim, translation, trans1 );

    /* Construct translation matrix for centre of rotation and apply it */

    for_less( i, 0, ndim )
        centre_translate[i] = -centre_of_rotation[i];

    translation_to_homogeneous( ndim, centre_translate, trans_temp );
    matrix_multiply( size, size, size, trans1, trans_temp, trans2 );

    /* Scale rotation matrix, then convert it to homogeneous coordinates and
       apply it */

    matrix_scalar_multiply( ndim, ndim, scaling, rotation, rotation_and_scale );
    rotation_to_homogeneous( ndim, rotation_and_scale, trans_temp );
    matrix_multiply( size, size, size, trans2, trans_temp, trans1 );

    /* Return to centre of rotation */

    translation_to_homogeneous( ndim, centre_of_rotation, trans_temp );
    matrix_multiply( size, size, size, trans1, trans_temp, transformation );

    /* Free matrices */

    FREE( centre_translate );
    FREE2D( trans1 );
    FREE2D( trans2 );
    FREE2D( trans_temp );
    FREE2D( rotation_and_scale );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : translation_to_homogeneous
@INPUT      : ndim    - number of dimensions
              translation - Numerical recipes vector (1 to ndim) that 
                 specifies the translation.
@OUTPUT     : transformation - Numerical recipes matrix (1 to ndim+1 by
                 1 to ndim+1) specifying the transformation for homogeneous 
                 coordinates. To apply this transformation, a point
                 vector should be post-multiplied by this matrix, with the
                 last coordinate of the ndim+1 point vector having value
                 one. The calling routine must allocate space for this
                 matrix.
@RETURNS    : (nothing)
@DESCRIPTION: Computes a transformation matrix in homogeneous coordinates
              given a translation.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : February 7, 1992 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  translation_to_homogeneous(
    int    ndim,
    Real   translation[],
    Real  **transformation )
{
    int  i, j, size;

    size = ndim+1;

    /* Construct translation matrix */

    for_less( i, 0, ndim )
    {
        for_less( j, 0, size )
        {
            if( i == j )
                transformation[i][j] = 1.0;
            else
                transformation[i][j] = 0.0;
        }
    }

    for_less( j, 0, ndim )
        transformation[size-1][j] = translation[j];

    transformation[size-1][size-1] = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : rotation_to_homogeneous
@INPUT      : ndim    - number of dimensions
              rotation - Numerical recipes matrix (1 to ndim by 1 to ndim) 
                 for rotation about origin. Note that this matrix need not 
                 only specify rotation/reflexion - any ndim x ndim matrix 
                 will work.
@OUTPUT     : transformation - Numerical recipes matrix (1 to ndim+1 by
                 1 to ndim+1) specifying the transformation for homogeneous 
                 coordinates. To apply this transformation, a point
                 vector should be post-multiplied by this matrix, with the
                 last coordinate of the ndim+1 point vector having value
                 one. The calling routine must allocate space for this
                 matrix.
@RETURNS    : (nothing)
@DESCRIPTION: Computes a transformation matrix in homogeneous coordinates
              given a rotation matrix.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : 
@CREATED    : February 7, 1992 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private   void  rotation_to_homogeneous(
    int     ndim,
    Real    **rotation,
    Real    **transformation )
{
    int   i, j, size;

    size = ndim + 1;

    /* Construct  matrix */

    for_less( i, 0, size )
    {
        for_less( j, 0, size )
        {
            if( i==size-1 || j==size-1 )
                transformation[i][j] = 0.0;
            else
                transformation[i][j] = rotation[i][j];
        }
    }

    transformation[size-1][size-1] = 1.0;
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


/* ----------------------------- MNI Header -----------------------------------
@NAME       : matrix_scalar_multiply
@INPUT      : rows    - number of rows of the_matrix.
              cols    - number of columns of the_matrix
              scalar  - scalar by which the_matrix should be multiplied.
              the_matrix  - matrix to be multiplied (in numerical recipes 
                 form). Dimensions are 1 to rows and 1 to cols.
@OUTPUT     : product - result of multiply ( in numerical recipes form).
                 Dimensions are 1 to rows and 1 to cols. This matrix
                 can be the input matrix.
@RETURNS    : (nothing)
@DESCRIPTION: Multiplies a matrix by a scalar.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
---------------------------------------------------------------------------- */

private  void  matrix_scalar_multiply(
    int     rows,
    int     cols,
    Real    scalar, 
    Real    **the_matrix,
    Real    **product )
{
    int   i, j;

    for_less( i, 0, rows )
        for_less( j, 0, cols )
            product[i][j] = scalar * the_matrix[i][j];
}
