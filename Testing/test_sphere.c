#include  <internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int     argc,
    char    *argv[] )
{
    int      i, n_points;
    Real     dx, dy, dz, dist1, dist2;
    Real     x_sphere[10], y_sphere[10], z_sphere[10];
    Point    points[10], vertex;

    fill_Point( points[0], 1.0, 0.3, 3.0 );
    fill_Point( points[1], 0.2, 1.0, 2.1 );
    fill_Point( points[2], -1.0, -0.4, 2.7 );
    fill_Point( points[3], 0.0, -1.0, 3.1 );

    fill_Point( vertex, 0.1, -0.2, 4.0 );

    n_points = 4;

    flatten_around_vertex_to_sphere( 10.0, &vertex, n_points, points,
                                     x_sphere, y_sphere, z_sphere );

    for_less( i, 0, n_points )
    {
        dist1 = distance_between_points( &points[i], &vertex );
        dx = x_sphere[i];
        dy = y_sphere[i];
        dz = z_sphere[i];
        dist2 = sqrt( dx * dx + dy * dy + dz * dz );
    
        print( "%g %g\n", dist1, dist2 );
    }

    return( 0 );
}
