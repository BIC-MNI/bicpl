#include  <module.h>

private  Real  sq_distance_between_points(
    Point  *p1,
    Point  *p2 )
{
    Vector  diff;

    SUB_POINTS( diff, *p1, *p2 );

    return( DOT_VECTORS( diff, diff ) );
}

private  Real  point_segment_sq_distance(
    Point   *p,
    Point   *q1,
    Point   *q2,
    Point   *closest_point )
{
    Real    mag, t;
    Vector  to_point, offset;

    SUB_POINTS( to_point, *p, *q1 );
    SUB_POINTS( offset, *q2, *q1 );

    mag = DOT_VECTORS( offset, offset );
    if( mag == 0.0 )
    {
        *closest_point = *q1;
        return( DOT_VECTORS( to_point, to_point ) );
    }

    t = DOT_VECTORS( to_point, offset ) / mag;

    if( t <= 0.0 )
    {
        *closest_point = *q1;
        return( sq_distance_between_points( p, q1 ) );
    }
    else if( t >= 1.0 )
    {
        *closest_point = *q2;
        return( sq_distance_between_points( p, q2 ) );
    }

    SCALE_VECTOR( offset, offset, t );
    ADD_POINT_VECTOR( *closest_point, *q1, offset );

    return( sq_distance_between_points( p, closest_point ) );
}

private  Real  find_point_polygon_distance(
    Point     *point,
    int       n_points,
    Point     poly_points[],
    Vector    *normal,
    Point     *closest_point )
{
    int      i, closest;
    Real     n_dot_n, t, closest_dist, dist, d1, d2;
    Vector   offset, o_a;
    Point    seg1_point, seg2_point;

    n_dot_n = DOT_VECTORS( *normal, *normal );
    if( n_dot_n == 0.0 )
    {
        fill_Point( *closest_point, 0.0, 0.0, 0.0 );
        return( 1.0e30 );
    }

    SUB_POINTS( o_a, poly_points[0], *point );

    t = DOT_VECTORS( o_a, *normal ) / n_dot_n;

    SCALE_VECTOR( offset, *normal, t ); 
    ADD_POINT_VECTOR( *closest_point, *point, offset );

    if( point_within_polygon( closest_point, n_points, poly_points, normal ) )
        return( MAGNITUDE( offset ) );

    closest = 0;
    closest_dist = 0.0;

    for_less( i, 0, n_points )
    {
        dist = sq_distance_between_points( point, &poly_points[i] );
        if( i == 0 || dist < closest_dist )
        {
            closest = i;
            closest_dist = dist;
        }
    }

    d1 = point_segment_sq_distance( point,
                                    &poly_points[(closest-1+n_points)%n_points],
                                    &poly_points[closest], &seg1_point );
    d2 = point_segment_sq_distance( point,
                                    &poly_points[closest],
                                    &poly_points[(closest+1)%n_points],
                                    &seg2_point );

    if( d1 < d2 )
    {
        *closest_point = seg1_point;
        closest_dist = d1;
    }
     else
    {
        *closest_point = seg2_point;
        closest_dist = d2;
    }

    return( sqrt( closest_dist ) );
}

public  int  find_closest_polygon_point(
    Point              *point,
    polygons_struct    *polygons,
    Point              *closest_point )
{
    int      poly, size, closest_poly;
    Real     dist, closest_dist;
    Point    poly_points[MAX_POINTS_PER_POLYGON], closest;
    Vector  normal;

    closest_dist = 0.0;

    for_less( poly, 0, polygons->n_items )
    {
        size = get_polygon_points( polygons, poly, poly_points );
        find_polygon_normal( size, poly_points, &normal );
        dist = find_point_polygon_distance( point, size, poly_points, &normal,
                                            &closest );
        if( poly == 0 || dist < closest_dist )
        {
            closest_poly = poly;
            closest_dist = dist;
            *closest_point = closest;
        }
    }

    return( closest_poly );
}
