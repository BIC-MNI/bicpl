
#include  <internal_volume_io.h>
#include  <geom.h>
#include  <numerical.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Numerical/least_squares.c,v 1.4 1995-06-23 14:24:52 david Exp $";
#endif

public  void  least_squares(
    int     n_points,
    int     n_dims,
    Real    **points,
    Real    values[],
    Real    parameters[] )
{
    int                   pt, i;
    linear_least_squares  lsq;
    Real                  *p;

    initialize_linear_least_squares( &lsq, n_dims+1 );

    ALLOC( p, n_dims+1 );

    for_less( pt, 0, n_points )
    {
        p[0] = 1.0;
        for_less( i, 0, n_dims )
            p[i+1] = points[pt][i];

        add_to_linear_least_squares( &lsq, p, values[pt] );
    }

    FREE( p );

    (void) get_linear_least_squares_solution( &lsq, parameters );

    delete_linear_least_squares( &lsq );
}

public  void  initialize_linear_least_squares(
    linear_least_squares   *lsq,
    int                    n_parameters )
{
    int   i, j;

    lsq->n_parameters = n_parameters;

    if( n_parameters > 0 )
    {
        ALLOC2D( lsq->second_derivs, n_parameters, n_parameters );
        ALLOC( lsq->constants, n_parameters );

        for_less( i, 0, n_parameters )
        {
            for_less( j, 0, n_parameters )
                lsq->second_derivs[i][j] = 0.0;
            lsq->constants[i] = 0.0;
        }
    }
}

public  void  add_to_linear_least_squares(
    linear_least_squares   *lsq,
    Real                   parameter_coefs[],
    Real                   constant )
{
    int   i, j;

    for_less( i, 0, lsq->n_parameters )
    {
        for_less( j, i, lsq->n_parameters )
            lsq->second_derivs[i][j] += parameter_coefs[i] * parameter_coefs[j];

        lsq->constants[i] += parameter_coefs[i] * constant;
    }
}

public  BOOLEAN  get_linear_least_squares_solution(
    linear_least_squares   *lsq,
    Real                   solution[] )
{
    BOOLEAN  solved;
    int      i, j;

    for_less( i, 0, lsq->n_parameters )
    {
        for_less( j, i+1, lsq->n_parameters )
            lsq->second_derivs[j][i] = lsq->second_derivs[i][j];
    }

    solved = solve_linear_system( lsq->n_parameters, lsq->second_derivs,
                                  lsq->constants, solution );

    if( !solved )
    {
        print_error(
             "Could not solve least squares, non-invertible matrix.\n" );

        for_less( i, 0, lsq->n_parameters )
            solution[i] = 0.0;
    }

    return( solved );
}

public  void  delete_linear_least_squares(
    linear_least_squares   *lsq )
{
    FREE2D( lsq->second_derivs );
    FREE( lsq->constants );
}
