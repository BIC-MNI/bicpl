/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_xfm.c
@DESCRIPTION: Routine to calculate a General_transform from a pair of tag
              point lists.
@METHOD     : 
@GLOBALS    : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : $Log: compute_xfm.c,v $
@MODIFIED   : Revision 1.10  1995-07-07 18:18:23  david
@MODIFIED   : check_in_all
@MODIFIED   :
 * Revision 1.9  1995/07/07  18:16:45  david
 * *** empty log message ***
 *
 * Revision 1.8  1995/07/05  14:52:06  david
 * *** empty log message ***
 *
 * Revision 1.7  1995/06/23  14:24:37  david
 * check_in_all
 *
 * Revision 1.6  1995/05/19  21:51:21  david
 * *** empty log message ***
 *
 * Revision 1.5  1995/04/04  03:42:05  david
 * check_in_all
 *
 * Revision 1.4  1995/03/07  18:54:49  david
 * check_in_all
 *
 * Revision 1.3  95/02/27  17:20:20  david
 * *** empty log message ***
 * 
 * Revision 1.2  94/11/25  14:23:13  david
 * check_in_all
 * 
 * Revision 1.1  94/11/04  14:45:47  david
 * Initial revision
 * 
 * Revision 1.4  93/09/10  11:52:58  neelin
 * Fixed 9 and 10 parameter fitting.
 * 
 * Revision 1.3  93/09/08  15:45:00  neelin
 * Added 9 and 10 parameter linear fitting.
 * 
 * Revision 1.2  93/09/01  15:35:17  neelin
 * Changed calls to lfit to lfit_vector.
 * 
 * Revision 1.1  93/09/01  15:25:25  neelin
 * Initial revision
 * 
 * Revision 1.1  93/09/01  13:21:30  neelin
 * Initial revision
 * 
---------------------------------------------------------------------------- */

#include <internal_volume_io.h>
#include <geom.h>
#include <numerical.h>
#include <compute_xfm.h>

/* Function declarations */

private void compute_procrustes_transform(int npoints, 
                                          Real **tag_list1, 
                                          Real **tag_list2, 
                                          Trans_type trans_type,
                                          General_transform *transform);
private void compute_arb_param_transform(int npoints, 
                                         Real **tag_list1, 
                                         Real **tag_list2, 
                                         Trans_type trans_type,
                                         General_transform *transform);
private void compute_12param_transform(int npoints, 
                                       Real **tag_list1, 
                                       Real **tag_list2, 
                                       Trans_type trans_type,
                                       General_transform *transform);
private void compute_tps_transform(int npoints, 
                                   Real **tag_list1, 
                                   Real **tag_list2, 
                                   Trans_type trans_type,
                                   General_transform *transform);
private  void  build_homogeneous_from_parameters(
    Real  **calc_transformation,
    Real  centre[],
    Real  translation[],
    Real  scales[],
    Real  shears[],
    Real  angles[] );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_transform_from_tags
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  compute_transform_from_tags(
    int                 npoints, 
    Real                **tag_list1, 
    Real                **tag_list2, 
    Trans_type          trans_type,
    General_transform   *transform)
{
    /* Check number of points for linear transformation */

    if( ((trans_type == TRANS_LSQ6) ||
         (trans_type == TRANS_LSQ7) ||
         (trans_type == TRANS_LSQ9) ||
         (trans_type == TRANS_LSQ10) ||
         (trans_type == TRANS_LSQ12)) &&
        (npoints < MIN_POINTS_LINEAR) )
    {
        create_linear_transform(transform, NULL);
        return;
    }

    /* Check number of points for thin-plate spline transformation */

    if( (trans_type == TRANS_TPS) && (npoints < MIN_POINTS_TPS) )
    {
        create_linear_transform(transform, NULL);
        return;
    }

    switch (trans_type)
    {
    case TRANS_LSQ6:
    case TRANS_LSQ7:
        compute_procrustes_transform( npoints, tag_list1, tag_list2,
                                      trans_type, transform );
        break;

    case TRANS_LSQ9:
    case TRANS_LSQ10:
        compute_arb_param_transform( npoints, tag_list1, tag_list2,
                                     trans_type, transform );
        break;

    case TRANS_LSQ12:
        compute_12param_transform( npoints, tag_list1, tag_list2,
                                   trans_type, transform );
        break;

    case TRANS_TPS:
        compute_tps_transform( npoints, tag_list1, tag_list2,
                               trans_type, transform );
        break;

    default:
        print_error(
            "Invalid transform type in compute_transform_from tags\n" );

        exit(EXIT_FAILURE);
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_procrustes_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses 6 or 7 parameter procrustes transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  compute_procrustes_transform(
    int                 npoints, 
    Real                **tag_list1, 
    Real                **tag_list2, 
    Trans_type          trans_type,
    General_transform   *transform)
{
    Real        translation[N_DIMENSIONS];
    Real        centre_of_rotation[N_DIMENSIONS];
    Real        **rotation, scale;
    Real        **calc_transformation;
    int         i, j;
    Transform   linear_transform;

    /* Create needed matrices and vectors */

    ALLOC2D( rotation, N_DIMENSIONS, N_DIMENSIONS );
    ALLOC2D( calc_transformation, N_DIMENSIONS + 1, N_DIMENSIONS + 1 );

    /* Do procrustes fit */

    procrustes( npoints, N_DIMENSIONS, tag_list1, tag_list2, translation, 
                centre_of_rotation, rotation, &scale );

    /* Set scale appropriately */

    if( trans_type == TRANS_LSQ6 )
        scale = 1.0;

    /* Calculate matrix in homogeneous coordinates */

    transformations_to_homogeneous( N_DIMENSIONS,
                                    translation, centre_of_rotation,
                                    rotation, scale, calc_transformation );

    /* Save the transform */

    for_less( i, 0, N_DIMENSIONS + 1 )
    {
        for_less( j, 0, N_DIMENSIONS + 1 )
            Transform_elem(linear_transform, i, j) = calc_transformation[j][i];
    }

    create_linear_transform( transform, &linear_transform );

    /* Free the matrices and vectors */

    FREE2D( calc_transformation );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_arb_param_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Computes a linear transform using arbitrary parameters.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Louis Collins and Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  compute_arb_param_transform(
    int                 npoints, 
    Real                **tag_list1, 
    Real                **tag_list2, 
    Trans_type          trans_type,
    General_transform   *transform )
{
    Real       translation[N_DIMENSIONS];
    Real       centre_of_rotation[N_DIMENSIONS], **rotation, scale;
    Real       scales[N_DIMENSIONS];
    Real       shears[N_DIMENSIONS], angles[N_DIMENSIONS];
    Real       **calc_transformation;
    int        i, j;
    Transform  linear_transform;
  
    if( trans_type != TRANS_LSQ9 && trans_type != TRANS_LSQ10 )
    {
        print_error( "Error in compute_arb_param_transform().\n" );
        print_error( "Trans_type (=%d) requested is not LSQ9 or LSQ10.\n", 
                     trans_type );
        exit(EXIT_FAILURE);
    }

    /* Create needed matrices and vectors */

    ALLOC2D( rotation, N_DIMENSIONS, N_DIMENSIONS );
    ALLOC2D( calc_transformation, N_DIMENSIONS + 1, N_DIMENSIONS + 1 );
  
    /* Do procrustes fit */

    procrustes( npoints, N_DIMENSIONS, tag_list1, tag_list2, translation, 
                centre_of_rotation, rotation, &scale );
  
    if( !rotmat_to_ang( rotation, angles ) )
    {
        print_error( "Error in compute_arb_param_transform().\n");
        print_error( "Cannot extract angles from rotation matrix.\n");
        exit(EXIT_FAILURE);
    }
  
    for_less( i, 0, N_DIMENSIONS )
    {
        shears[i] = 0.0;
        scales[i] = scale;
    }

    if( !optimize_simplex( tag_list1, tag_list2,
                           npoints, trans_type,
                           centre_of_rotation, 
                           translation,
                           scales,
                           shears,
                           angles) )
    {
        print_error( "Error in compute_arb_param_transform(),\n");
        print_error( "in call to optimize_simplex().\n");
        exit(EXIT_FAILURE);
    }

    /* Calculate matrix in homogeneous coordinates */

    build_homogeneous_from_parameters( calc_transformation,
                                       centre_of_rotation, 
                                       translation,
                                       scales,
                                       shears,
                                       angles );

    /* Save the transform */

    for_less( i, 0, N_DIMENSIONS + 1 )
    {
        for_less( j, 0, N_DIMENSIONS + 1 )
            Transform_elem(linear_transform, i, j) = calc_transformation[j][i];
    }

    create_linear_transform( transform, &linear_transform );
  
    /* Free the matrices and vectors */

    FREE2D(calc_transformation );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_rots
@INPUT      : rot_x, rot_y, rot_z - three rotation angles, in radians.
@OUTPUT     : xmat, a numerical recipes matrix for homogeous transformations
@RETURNS    : nothing
@DESCRIPTION: to be applied by premultiplication, ie rot*vec = newvec
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Tue Jun  8 08:44:59 EST 1993 LC
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void   make_rots(
    Real      **xmat,
    Real      rot_x,
    Real      rot_y,
    Real      rot_z )
{
    Real     **TRX, **TRY, **TRZ, **T1, **T2;
   
    ALLOC2D( TRX, 4, 4 );
    ALLOC2D( TRY, 4, 4 );
    ALLOC2D( TRZ, 4, 4 );
    ALLOC2D( T1, 4, 4 );
    ALLOC2D( T2, 4, 4 );

    nr_rotx( TRX, rot_x );             /* create the rotate X matrix */
    nr_roty( TRY, rot_y );             /* create the rotate Y matrix */
    nr_rotz( TRZ, rot_z );             /* create the rotate Z matrix */

    matrix_multiply( 4, 4, 4, TRY, TRX, T1 ); /* apply rx and ry */
    matrix_multiply( 4, 4, 4, TRZ, T1,  T2 ); /* apply rz */

    transpose( 4, 4, T2, xmat );
   
    FREE2D( TRX );
    FREE2D( TRY );
    FREE2D( TRZ );
    FREE2D( T1 );
    FREE2D( T2 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : build_transformation_matrix
@INPUT      : center, translations, scales, rotations
@OUTPUT     : lt  - a linear transformation matrix
@RETURNS    : nothing
@DESCRIPTION: mat = (c)(s*r)(-c)(t),
               the matrix is to be  PREmultiplied with a vector (mat*vec)
               when used in the application
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Thu Jun  3 09:37:56 EST 1993 lc
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  build_transformation_matrix(
    Real  lt[3][4], 
    Real  center[],
    Real  translations[],
    Real  scales[],
    Real  shears[],
    Real  rotations[] )
{
    Real    **T, **SH, **S, **R, **C, **T1, **T2, **T3, **T4;
    int     i, j;

    ALLOC2D( T, 4, 4 );
    ALLOC2D( SH, 4, 4 );
    ALLOC2D( S, 4, 4 );
    ALLOC2D( R, 4, 4 );
    ALLOC2D( C, 4, 4 );
    ALLOC2D( T1, 4, 4 );
    ALLOC2D( T2, 4, 4 );
    ALLOC2D( T3, 4, 4 );
    ALLOC2D( T4, 4, 4 );

    /* mat = (C)(SH)(S)(R)(-C)(T) */

    nr_ident( T, 4, 4 );                     /* make (T)(-C) */

    for_less( i, 0, 3 )
        T[i][3] = translations[i] - center[i];                

    /* make rotation matix */

    make_rots( R, rotations[0], rotations[1], rotations[2] );

    /* make shear rotation matrix */

    make_rots( SH, shears[0], shears[1], shears[2] );

    /* make scaling matrix */

    nr_ident( S, 4, 4 );

    for_less( i, 0, 3 )
        S[i][i] = scales[i];

    nr_ident( C, 4, 4 );          /* make center          */

    for_less( i, 0, 3 )
        C[i][3] = center[i];                

    matrix_multiply( 4, 4, 4, C, SH,  T1 );  
    matrix_multiply( 4, 4, 4, T1, S,  T2 );  
    matrix_multiply( 4, 4, 4, T2, R,  T3 );  
    matrix_multiply( 4, 4, 4, T3, T, T4 );  

    for_less( i, 0, 3 )
    {
        for_less( j, 0, 4 )
            lt[i][j] = T4[i][j];
    }

    FREE2D( T );
    FREE2D( SH );
    FREE2D( S );
    FREE2D( R );
    FREE2D( C );
    FREE2D( T1 );
    FREE2D( T2 );
    FREE2D( T3 );
    FREE2D( T4 );
}

private  void  build_homogeneous_from_parameters(
    Real  **calc_transformation,
    Real  centre[],
    Real  translation[],
    Real  scales[],
    Real  shears[],
    Real  angles[] )
{
    Real  mat[3][4];
    int   i, j;
  
    build_transformation_matrix( mat, centre, translation, scales, shears,
                                 angles );

    for_less( i, 0, N_DIMENSIONS )
        for_less( j, 0, N_DIMENSIONS + 1 )
            calc_transformation[j][i] = mat[i][j];

    for_less( i, 0, N_DIMENSIONS )
        calc_transformation[i][N_DIMENSIONS] = 0.0;

    calc_transformation[N_DIMENSIONS][N_DIMENSIONS] = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_12param_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses 12 parameter linear transformation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : 
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  compute_12param_transform(
    int                 npoints, 
    Real                **tag_list1, 
    Real                **tag_list2, 
    Trans_type          trans_type,
    General_transform   *transform)
{
    Real       *x, solution[N_DIMENSIONS + 1];
    int        d, dim;
    int        point;
    Transform  linear_transform;

    /* Check transformation type */

    if( trans_type != TRANS_LSQ12 )
    {
        print_error( "Internal error in compute_12param_transform!\n");
        exit(EXIT_FAILURE);
    }

    make_identity_transform( &linear_transform );

    ALLOC( x, npoints );

    for_less( dim, 0, N_DIMENSIONS )
    {
        /* Copy the data points */

        for_less( point, 0, npoints )
            x[point] = tag_list1[point][dim];

        /*--- find the solution */

        least_squares( npoints, N_DIMENSIONS, tag_list2, x, solution );

        Transform_elem( linear_transform, dim, N_DIMENSIONS ) = solution[0];
        for_less( d, 0, N_DIMENSIONS )
            Transform_elem( linear_transform, dim, d ) = solution[1+d];
    }

    /* Create general transform */

    create_linear_transform( transform, &linear_transform );

    /* Free matrices and vectors */

    FREE( x );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_tps_transform
@INPUT      : npoints - number of pairs of tag points
              tag_list1 - first list of tag points
              tag_list2 - second list of tag points
              trans_type - type of transformation to calculate
@OUTPUT     : transform - computed transform
@RETURNS    : (nothing)
@DESCRIPTION: Routine to calculate a general transform from a pair of lists
              of tag points. The transform is from tag_list2 to tag_list1.
              Uses thin-plate spline transform.
@METHOD     : We calculate the tranformation from tag_list1 to tag_list2
              and then invert it so that resampling volumes is faster
              (doing a forward transformation, rather than a iterative
              inverse).
@GLOBALS    : 
@CALLS      : 
@CREATED    : August 30, 1993 (Peter Neelin)
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */
 
private  void  compute_tps_transform(
    int                 npoints, 
    Real                **tag_list1, 
    Real                **tag_list2, 
    Trans_type          trans_type,
    General_transform   *transform)
{
    Real               **displacements;
    General_transform  inv_transform;

    /* Check trans_type */

    if (trans_type != TRANS_TPS)
    {
        print_error( "Wrong trans type in compute_tps_transform\n");
        exit(EXIT_FAILURE);
    }

    /* Allocate matrices */

    ALLOC2D( displacements, npoints+1+N_DIMENSIONS, N_DIMENSIONS );

    get_nonlinear_warp( tag_list1, tag_list2, displacements, npoints,
                        N_DIMENSIONS, N_DIMENSIONS );

    /* ---- Create general transform */

    create_thin_plate_transform_real( &inv_transform, N_DIMENSIONS, npoints, 
                                      tag_list1, displacements );

    /* ---- Invert general transform */

    create_inverse_general_transform( &inv_transform, transform );

    /* ---- Free inverse transform */

    delete_general_transform(&inv_transform);

    /* ---- Free displacements */

    FREE2D( displacements );
}
