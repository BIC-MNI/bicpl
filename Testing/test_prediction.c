#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int     argc,
    char    *argv[] )
{
    int      i, j, n_points, dim;
    Real     x, y, value;
    Real     xs[10], ys[10], *weights[2][2], constants[2];

    ALLOC( weights[0][0], 10 );
    ALLOC( weights[0][1], 10 );
    ALLOC( weights[1][0], 10 );
    ALLOC( weights[1][1], 10 );

    xs[0] = 1.0;
    ys[0] = 2.0;
    xs[1] = 3.0;
    ys[1] = 5.0;
    xs[2] = 5.0;
    ys[2] = 8.0;

    xs[0] = 1.0;
    ys[0] = 0.0;
    xs[1] = -1.0;
    ys[1] = 0.0;

    xs[0] = 7.6536700278753571;
    ys[0] = 0.0;
    xs[1] = 1.1208519367670462;
    ys[1] = 2.7059789613948975;
    xs[2] = -5.4119662404558344;
    ys[2] = 5.4119577148906028;

    x = 0.0;
    y = 0.0;

    n_points = 2;
    n_points = 3;

    if( !get_prediction_weights_2d( x, y, n_points, xs, ys,
                                    weights[0], &constants[0],
                                    weights[1], &constants[1] ) )
        print_error( "Dang.\n" );

    for_less( dim, 0, 2 )
    {
        value = constants[dim];
        for_less( j, 0, n_points )
        {
            value += weights[dim][0][j] * xs[j] +
                     weights[dim][1][j] * ys[j];
        }

        print( "%g %g: %g\n", x, y, value );
    }

/*
    if( !get_interpolation_weights_2d( xs[3], ys[3], n_points, xs, ys,
                                       weights ) )
            print_error( "Dang.\n" );

    values[3] = weights[0] * values[0] +
                weights[1] * values[1] +
                weights[2] * values[2];

    n_points = 4;

    for_less( i, 0, n_points )
    {
        if( !get_interpolation_weights_2d( xs[i], ys[i], n_points, xs, ys,
                                           weights ) )
            print_error( "Dang.\n" );

        value = 0.0;
        for_less( j, 0, n_points )
            value += weights[j] * values[j];

        print( "%g %g: ", values[i], value );

        for_less( j, 0, n_points )
            print( " %g", weights[j] );
        print( "\n" );
    }
*/

    return( 0 );
}
