
#include  <internal_volume_io.h>
#include  <geom.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Numerical/least_squares.c,v 1.2 1995-03-07 18:54:58 david Exp $";
#endif

public  void  least_squares(
    int     n_points,
    int     n_dims,
    Real    **points,
    Real    values[],
    Real    parameters[] )
{
    int      pt, i, j;
    Real     **coefs, *constants, *p;

    ALLOC2D( coefs, n_dims+1, n_dims+1 );
    ALLOC( constants, n_dims+1 );
    ALLOC( p, n_dims+1 );

    for_less( i, 0, n_dims+1 )
    {
        for_less( j, 0, n_dims+1 )
            coefs[i][j] = 0.0;

        constants[i] = 0.0;
    }

    for_less( pt, 0, n_points )
    {
        p[0] = 1.0;
        for_less( i, 0, n_dims )
            p[i+1] = points[pt][i];

        for_less( i, 0, n_dims+1 )
        {
            for_less( j, 0, n_dims+1 )
                coefs[i][j] += p[i] * p[j];

            constants[i] += p[i] * values[pt];
        }
    }

    for_less( i, 0, n_dims+1 )
    {
        for_less( j, i+1, n_dims+1 )
            coefs[j][i] = coefs[i][j];
    }

    if( !solve_linear_system( n_dims+1, coefs, constants, parameters ) )
    {
        print( "Could not solve least squares, non-invertible matrix.\n" );
        for_less( i, 0, n_dims+1 )
            parameters[i] = 0.0;
    }
}
