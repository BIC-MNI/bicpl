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

#include <internal_volume_io.h>
#include <trans.h>
#include <numerical.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Transforms/optimize.c,v 1.11 1995-12-06 21:11:07 david Exp $";
#endif

#define   FUNCTION_TOLERANCE    1e-6
#define   INITIAL_SIMPLEX_SIZE  3.0
#define   MAX_ITERS             300

typedef struct
{
    int   ndim;
    int   npoints;
    Real  *center;
    Real  **pts1;
    Real  **pts2;
} function_data;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lsq_objective
@INPUT      :  lt - a transformation matrix
               pts1
               pts2
               npoints
@OUTPUT     : 
@RETURNS    : sum of square error 
@DESCRIPTION: Transforms the pts2 and takes the sum squared distance from
              pts1.
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 

---------------------------------------------------------------------------- */

private  Real  lsq_objective(
    Transform  *lt,
    Real       **pts1,
    Real       **pts2, 
    int        npoints )
{
    int   i, j;
    Real  sum, error2, delta;
    Real  newpt[N_DIMENSIONS];
  
    sum = 0.0;

    for_less( i, 0, npoints )         /* for all of the points */
    {
        transform_point( lt, pts2[i][X], pts2[i][Y], pts2[i][Z],
                         &newpt[X], &newpt[Y], &newpt[Z] );

        error2 = 0.0;        /* compare it to pts2, summing the squared error */
    
        for_less( j, 0, N_DIMENSIONS )
        {
            delta = newpt[j] - pts1[i][j];
            error2 += delta * delta;
        }

        sum += error2;
    }

#ifdef I_DONT_THINK_THIS_IS_NEEDED

      I if'def'd this out because we are no longer using the Numerical
      recipes code, so I don't think the new amoeba code will blow up,
      as hinted by the next comment.
                          -- David MacDonald

    /* Add in a constant error so that amoeba doesn't blow up when there is no
       error on the points. This is equivalent to having an rms error of 
       1/FUDGE_FACTOR of the length of the long diagonal of the tag point 
       bounding box. A FUDGE_FACTOR of 250 was chosen to given a typical 
       rms error of around 1mm */

#define FUDGE_FACTOR 250.0

    {
        Real  max[N_DIMENSIONS], min[N_DIMENSIONS], minimum_error, range;

        for_less( j, 0, N_DIMENSIONS )
        {
            max[j] = -DBL_MAX;
            min[j] = DBL_MAX;
        }

        for_less( i, 0, npoints )         /* for all of the points */
        {
            for_less( j, 0, N_DIMENSIONS )
            {
                if( pts1[i][j] > max[j] ) max[j] = pts1[i][j];
                if( pts1[i][j] < min[j] ) min[j] = pts1[i][j];
            }
        }

        minimum_error = 0.0;
        for_less( j, 0, N_DIMENSIONS )
        {
           range = (max[j] - min[j]) / FUDGE_FACTOR;
           minimum_error += range * range;
        }

        if( minimum_error == 0.0 ) minimum_error = 1.0;

        sum += npoints * minimum_error;
    }
#endif

    return( sum);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fit_function
@INPUT      : func_data - info for evaluation the fit function
              params - a variable length array of real
@OUTPUT     :               
@RETURNS    : a Real value of the user requested objective function,
              measuring the similarity between two data sets.
@DESCRIPTION: This function
              is passed to the general purpose amoeba routine to be called
              as the function to minimize.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Real  fit_function(
    void   *func_data,
    float  params[] ) 
{
    Transform      transform;
    int            i;
    Real           r;
    Real           trans[3];
    Real           cent[3];
    Real           rots[3];
    Real           scale[3];
    Real           shear[3];
    function_data  *data;

    data = (function_data *) func_data;

    for_less( i, 0, 3 ) {
        trans[i] = (Real) params[i+0];        /* set translations */
        rots[i]  = (Real) params[i+3];        /* set rotations */
        scale[i] = (Real) params[i+6];        /* set scales */
    }

    for_less( i, 0, 3 )                /* set shears */
        shear[i] = 0.0;

    if( data->ndim == 10 ) 
        shear[0] = (Real) params[9]; 
    
    for_less( i, 0, 3 )
        cent[i] = data->center[i];        /* global CENTER used here */

    build_transformation_matrix( &transform, cent, trans, scale, shear, rots );

                                /* call the needed objective function */

    r = lsq_objective( &transform, data->pts1, data->pts2, data->npoints );

    return(r);
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : optimize_simplex
                get the parameters necessary to map point set 1 to point set 2
                using the simplex optimization algorithm.
@INPUT      : pts1,pts2 - two sets of 3D coordinates
              npoints - number of points in the two lists
              trans_type = TRANS_LSQ9 or TRANS_LSQ10
              center - array of center of rotation/scaling
@OUTPUT     : translation
              scales
              shears
              rotations
@RETURNS    : TRUE if ok, FALSE if error.
@DESCRIPTION: 
@METHOD     : uses the ameoba simplex algorithm from numerical recipes. 
@GLOBALS    : 
@CALLS      : 
@CREATED    : September 2, 1993 Louis
@MODIFIED   : July 4, 1995      D. MacDonald   - removed recipes-style code
---------------------------------------------------------------------------- */

public  BOOLEAN  optimize_simplex(
    Real           **pts1,
    Real           **pts2, 
    int            npoints, 
    Trans_type     trans_type,
    Real           center[],
    Real           translations[],
    Real           scales[],
    Real           shears[],
    Real           rotations[] )
{
    Real           initial_guess[10], solution[10], initial_step[10];
    int            i, ndim, iters;
    function_data  func_data;
    amoeba_struct  amoeba;

    switch (trans_type)                /* check to see if trans_type is OK */
    {
    case TRANS_LSQ9: 
        ndim = 9;
        break;
    case TRANS_LSQ10: 
        ndim = 10;
        break;
    default:
        print_error( "Unknown type of transformation requested (%d)\n",
                     trans_type);
        print_error( "Error in line %d, file %s\n",__LINE__, __FILE__);
        return( FALSE );
    }

    /*--- initialize the function data for the function to be minimized */

    func_data.pts1 = pts1;
    func_data.pts2 = pts2;
    func_data.npoints = npoints;
    func_data.center = center;
    func_data.ndim = ndim;

    /*--- initialize the 9 or 10 parameters of the function, the starting
          point for the minimization */

    initial_guess[0] = translations[0];
    initial_guess[1] = translations[1];
    initial_guess[2] = translations[2];

    initial_guess[3] = rotations[0];
    initial_guess[4] = rotations[1];
    initial_guess[5] = rotations[2];

    initial_guess[6] = scales[0];
    initial_guess[7] = scales[1];
    initial_guess[8] = scales[2];

    if( ndim == 10 )        /* one rotation about the x-axis (LR-axis) */
        initial_guess[9] = shears[0];

    /*--- initialize the step sizes for the amoeba minimization */

    initial_step[0] = INITIAL_SIMPLEX_SIZE;
    initial_step[1] = INITIAL_SIMPLEX_SIZE;
    initial_step[2] = INITIAL_SIMPLEX_SIZE;

    initial_step[3] = INITIAL_SIMPLEX_SIZE * DEG_TO_RAD;
    initial_step[4] = INITIAL_SIMPLEX_SIZE * DEG_TO_RAD;
    initial_step[5] = INITIAL_SIMPLEX_SIZE * DEG_TO_RAD;

    initial_step[6] = INITIAL_SIMPLEX_SIZE / 30;
    initial_step[7] = INITIAL_SIMPLEX_SIZE / 30;
    initial_step[8] = INITIAL_SIMPLEX_SIZE / 30;
    
    if( ndim == 10 ) 
        initial_step[9] = INITIAL_SIMPLEX_SIZE * DEG_TO_RAD;

    /*--- perform the amoeba minimization */

    initialize_amoeba( &amoeba, ndim, initial_guess, initial_step,
                       fit_function, (void *) &func_data, FUNCTION_TOLERANCE );

    for_less( iters, 0, MAX_ITERS )
    {
        if( !perform_amoeba( &amoeba ) )
            break;
    }

    (void) get_amoeba_parameters( &amoeba, solution );

    terminate_amoeba( &amoeba );
    
    /*--- copy optimized results */

    for_less( i, 0, 3 )
    {
        translations[i] = solution[i+0];
        rotations[i]    = solution[i+3];
        scales[i]       = solution[i+6];
    }

    if( ndim == 10 )
        shears[0] = solution[9];

    return( TRUE );
}
