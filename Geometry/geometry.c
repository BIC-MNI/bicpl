
#include  <def_geometry.h>

public  Boolean  intersect_lines_3d( origin1, delta1, origin2, delta2,
                                     intersection_pt )
    Point   *origin1;
    Vector  *delta1;
    Point   *origin2;
    Vector  *delta2;
    Point   *intersection_pt;
{
    Boolean   intersects;
    Real      t, bottom;
    Real      d11, d12, d22;
    Real      o11, o12, o21, o22;

    d11 = DOT_VECTORS( *delta1, *delta1 );
    d12 = DOT_VECTORS( *delta1, *delta2 );
    d22 = DOT_VECTORS( *delta2, *delta2 );

    bottom = d11 * d22 - d12 * d12;

    intersects = (bottom != 0.0);

    if( intersects )
    {
        o11 = DOT_POINT_VECTOR( *origin1, *delta1 );
        o12 = DOT_POINT_VECTOR( *origin1, *delta2 );
        o21 = DOT_POINT_VECTOR( *origin2, *delta1 );
        o22 = DOT_POINT_VECTOR( *origin2, *delta2 );

        t = -(d22 * (o11 - o21) + d12 * (o22 - o12)) / bottom;

        GET_POINT_ON_RAY( *intersection_pt, *origin1, *delta1, t );
    }

    return( intersects );
}

public  void  find_polygon_normal( n_points, points, normal )
    int      n_points;
    Point    points[];
    Vector   *normal;
{
    int    i, next_i;

    Vector_x(*normal) = 0.0;
    Vector_y(*normal) = 0.0;
    Vector_z(*normal) = 0.0;

    for_less( i, 0, n_points )
    {
        next_i = (i + 1) % n_points;

        Vector_x(*normal) -= (Point_y(points[i]) + Point_y(points[next_i])) *
                             (Point_z(points[i]) - Point_z(points[next_i]) );

        Vector_y(*normal) -= (Point_z(points[i]) + Point_z(points[next_i])) *
                             (Point_x(points[i]) - Point_x(points[next_i]) );

        Vector_z(*normal) -= (Point_x(points[i]) + Point_x(points[next_i])) *
                             (Point_y(points[i]) - Point_y(points[next_i]) );
    }
}

public  Real  get_polygon_2d_area( n_points, points )
    int      n_points;
    Point    points[];
{
    int    i, next_i;
    Real   area;

    area = 0.0;

    for_less( i, 0, n_points )
    {
        next_i = (i + 1) % n_points;
        area += Point_x(points[i]) * Point_y(points[next_i]) -
                Point_x(points[next_i]) * Point_y(points[i]);
    }

    return( area / 2.0 );
}

public  void  get_noncolinear_vector( v, non_colinear )
    Vector   *v;
    Vector   *non_colinear;
{
    *non_colinear = *v;

    if( Vector_x(*v) == 0.0 )
        Vector_x(*non_colinear) = 1.0;
    else if( Vector_y(*v) == 0.0 )
        Vector_y(*non_colinear) = Vector_x(*v);
    else if( Vector_z(*v) == 0.0 )
        Vector_z(*non_colinear) = Vector_x(*v);
    else
        Vector_z(*non_colinear) = 0.0;
}

public  void   get_plane_through_points( n_points, points, normal, 
                                         plane_constant )
    int      n_points;
    Point    points[];
    Vector   *normal;
    Real     *plane_constant;
{
    Point   centroid;
    void    get_points_centroid();
    Real    distance_from_plane();

    find_polygon_normal( n_points, points, normal );

    NORMALIZE_VECTOR( *normal, *normal );

    get_points_centroid( n_points, points, &centroid );

    *plane_constant = - distance_from_plane( &centroid, normal, 0.0 );
}

public  Real  distance_from_plane( point, plane_normal, plane_constant )
    Point    *point;
    Vector   *plane_normal;
    Real     plane_constant;
{
    return( DOT_POINT_VECTOR( *point, *plane_normal ) + plane_constant );
}

public  Boolean  find_closest_line_sphere_intersection( line_origin,
                     line_direction, sphere_centre, x_size, y_size, z_size,
                     intersection )
    Point    *line_origin;
    Vector   *line_direction;
    Point    *sphere_centre;
    Real     x_size;
    Real     y_size;
    Real     z_size;
    Point    *intersection;
{
    Boolean   intersects;
    int       n_solutions;
    Real      a, b, c, ox, oy, oz, dx, dy, dz, t1, t2, t;
    Vector    offset;

    ox = (Point_x(*line_origin) - Point_x(*sphere_centre)) / x_size;
    oy = (Point_y(*line_origin) - Point_y(*sphere_centre)) / y_size;
    oz = (Point_z(*line_origin) - Point_z(*sphere_centre)) / z_size;

    dx = Vector_x(*line_direction) / x_size;
    dy = Vector_y(*line_direction) / y_size;
    dz = Vector_z(*line_direction) / z_size;

    a = dx * dx + dy * dy + dz * dz;
    b = 2.0 * ( ox * dx + oy * dy + oz * dz);
    c = ox * ox + oy * oy + oz * oz - 1.0;

    n_solutions = solve_quadratic( a, b, c, &t1, &t2 );

    if( n_solutions == 0 )
        intersects = FALSE;
    else if( n_solutions == 1 )
    {
        intersects = TRUE;
        t = t1;
    }
    else
    {
        intersects = TRUE;
        t = t1;
        if( ABS(t2) < ABS(t1) )
            t = t2;
    }

    if( intersects )
    {
        SCALE_VECTOR( offset, *line_direction, t );
        ADD_POINT_VECTOR( *intersection, *line_origin, offset );
    }

    return( intersects );
}
