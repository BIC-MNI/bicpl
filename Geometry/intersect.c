
#include  <internal_volume_io.h>
#include  <geom.h>

public  BOOLEAN   line_segment_intersects_plane(
    Point   *p1,
    Point   *p2,
    Point   *plane_origin,
    Vector  *plane_normal,
    Point   *intersection_point )
{
    Real     v_dot_n, t;
    Vector   d, v;

    SUB_POINTS( d, *plane_origin, *p1 );
    SUB_POINTS( v, *p2, *p1 );

    v_dot_n = DOT_VECTORS( v, *plane_normal );

    if( v_dot_n == 0.0 )
        return( FALSE );

    t = DOT_VECTORS( d, *plane_normal ) / v_dot_n;

    if( t < 0.0 || t > 1.0 )
        return( FALSE );

    INTERPOLATE_POINTS( *intersection_point, *p1, *p2, t );

    return( TRUE );
}
