#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int     argc,
    char    *argv[] )
{
    int      i, n_points;
    Point    vertex, neighbours[10];
    Real     x_flat[10], y_flat[10];

    n_points = 4;

    fill_Point( vertex, 1.0, -1.0, 1.0 );
    fill_Point( neighbours[0], 5.0, 0.0, 1.0 );
    fill_Point( neighbours[1], 5.0, 1.0, 1.0 );
    fill_Point( neighbours[2], -1.0, 5.0, 1.0 );
    fill_Point( neighbours[3], 0.0, -1.0, 7.0 );

    flatten_around_vertex( &vertex, n_points, neighbours, x_flat, y_flat );

    for_less( i, 0, n_points )
    {
        print( "%d:  %g %g\n", i, x_flat[i], y_flat[i] );
    }

    return( 0 );
}
