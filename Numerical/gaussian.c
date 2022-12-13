/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
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

#include "bicpl_internal.h"


/* EVIL HACK: scaled_maximal_pivoting_gaussian_elimination is an internal function in libminc*/
/* see minc2_private.h*/

int scaled_maximal_pivoting_gaussian_elimination(int   n,
                                                  int   row[],
                                                  double **a,
                                                  int   n_values,
                                                  double **solution );

int scaled_maximal_pivoting_gaussian_elimination_real(int n,
                                                      double **coefs,
                                                      int n_values,
                                                      double **values );


/* ----------------------------- MNI Header -----------------------------------
@NAME       : scaled_maximal_pivoting_gaussian_elimination_float
@INPUT      : n
              coefs
              n_values
              values
@OUTPUT     : 
@RETURNS    : TRUE if successful
@DESCRIPTION: Finds the solution to coefs[n][n] * solution[n][n_values] =
                             values[n_values][n] transposed, placing the
              solution in values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_BOOL  scaled_maximal_pivoting_gaussian_elimination_float(
    int    n,
    float  **coefs,
    int    n_values,
    float  **values )
{
    int       i, j, v, *row;
    VIO_Real      **a, **solution;
    VIO_BOOL   success;

    ALLOC( row, n );
    VIO_ALLOC2D( a, n, n );
    VIO_ALLOC2D( solution, n, n_values );

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            a[i][j] = (VIO_Real) coefs[i][j];
        for_less( v, 0, n_values )
            solution[i][v] = (VIO_Real) values[v][i];
    }

    success = scaled_maximal_pivoting_gaussian_elimination( n, row, a, n_values,
                                                            solution );

    if( success )
    {
        for_less( i, 0, n )
        {
            for_less( v, 0, n_values )
            {
                values[v][i] = (float) solution[row[i]][v];
            }
        }
    }

    VIO_FREE2D( a );
    VIO_FREE2D( solution );
    FREE( row );

    return( success );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : solve_linear_system_float
@INPUT      : n
              coefs[n][n]
              values
@OUTPUT     : solution
@RETURNS    : TRUE if successful
@DESCRIPTION: Solves the linear system of equations (coefs * solution = values).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] )
{
    int       i;

    for_less( i, 0, n )
        solution[i] = values[i];

    return( scaled_maximal_pivoting_gaussian_elimination_float( n, coefs, 1,
                                                                &solution ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : invert_square_matrix_float
@INPUT      : n
              matrix
@OUTPUT     : inverse
@RETURNS    : TRUE if successful.
@DESCRIPTION: Computes the inverse of a square matrix of floats.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse )
{
    float     tmp;
    VIO_BOOL   success;
    int       i, j;

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            inverse[i][j] = 0.0f;
        inverse[i][i] = 1.0f;
    }

    success = scaled_maximal_pivoting_gaussian_elimination_float( n, matrix,
                                                                  n, inverse );

    if( success )
    {
        for_less( i, 0, n-1 )
        {
            for_less( j, i+1, n )
            {
                tmp = inverse[i][j];
                inverse[i][j] = inverse[j][i];
                inverse[j][i] = tmp;
            }
        }
    }

    return( success );
}
