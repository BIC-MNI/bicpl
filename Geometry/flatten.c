
/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Sep. 10, 1996    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */


#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/flatten.c,v 1.1 1996-09-12 15:07:24 david Exp $";
#endif

#include  <geom.h>
#include  <internal_volume_io.h>

public  void  flatten_around_vertex(
    Point  *vertex,
    int    n_neighbours,
    Point  neighbours[],
    Real   x_flat[],
    Real   y_flat[] )
{
    Real  sum_angles, factor, angle, dist;
    int   i;

    sum_angles = 0.0;

    for_less( i, 0, n_neighbours )
    {
        angle = get_angle_between_points( &neighbours[i], vertex,
                                          &neighbours[(i+1)%n_neighbours] );
        sum_angles += angle;
    }

    if( sum_angles == 0.0 )
    {
        print_error( "flatten_around_vertex: sum of angles is 0.\n" );
        return;
    }

    factor = 2.0 * PI / sum_angles;

    x_flat[0] = distance_between_points( vertex, &neighbours[0] );
    y_flat[0] = 0.0;

    for_less( i, 0, n_neighbours-1 )
    {
        angle = get_angle_between_points( &neighbours[i], vertex,
                                          &neighbours[(i+1)%n_neighbours] );
        sum_angles += angle;

        angle = sum_angles * factor;

        dist = distance_between_points( vertex, &neighbours[i+1] );

        x_flat[i+1] = dist * cos( angle );
        y_flat[i+1] = dist * sin( angle );
    }
}
