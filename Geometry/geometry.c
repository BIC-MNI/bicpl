
#include  <mni.h>

public  BOOLEAN  intersect_lines_3d(
    Point   *origin1,
    Vector  *delta1,
    Point   *origin2,
    Vector  *delta2,
    Point   *intersection_pt )
{
    BOOLEAN   intersects;
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

public  void  find_polygon_normal(
    int      n_points,
    Point    points[],
    Vector   *normal )
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

    NORMALIZE_VECTOR( *normal, *normal );
}

public  Real  get_polygon_2d_area(
    int      n_points,
    Point    points[] )
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

public  void   get_plane_through_points(
    int      n_points,
    Point    points[],
    Vector   *normal,
    Real     *plane_constant )
{
    Point   centroid;

    find_polygon_normal( n_points, points, normal );

    NORMALIZE_VECTOR( *normal, *normal );

    get_points_centroid( n_points, points, &centroid );

    *plane_constant = - distance_from_plane( &centroid, normal, 0.0 );
}

public  Real  distance_from_plane(
    Point    *point,
    Vector   *plane_normal,
    Real     plane_constant )
{
    return( DOT_POINT_VECTOR( *point, *plane_normal ) + plane_constant );
}

public  Real  distance_from_line(
    Point    *point,
    Point    *end_point1,
    Point    *end_point2 )
{
    Vector   v12, v1p, perp;
    Real     projection, dist, len;

    SUB_POINTS( v12, *end_point2, *end_point1 );
    SUB_POINTS( v1p, *point, *end_point1 );

    len = DOT_VECTORS( v12, v12 );
    if( len == 0.0 )
        len = 1.0;

    projection = DOT_VECTORS( v12, v1p ) / len;

    SCALE_VECTOR( v12, v12, projection );
    SUB_POINT_VECTOR( perp, v1p, v12 );

    dist = MAGNITUDE( perp );

    return( dist );
}

public  BOOLEAN  find_closest_line_sphere_intersection(
    Point    *line_origin,
    Vector   *line_direction,
    Point    *sphere_centre,
    Real     x_size,
    Real     y_size,
    Real     z_size,
    Point    *intersection )
{
    BOOLEAN   intersects;
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

public  BOOLEAN  clip_line_to_box(
    Point    *origin,
    Vector   *direction,
    Real     x_min,
    Real     x_max,
    Real     y_min,
    Real     y_max,
    Real     z_min,
    Real     z_max,
    Real     *t_min,
    Real     *t_max )
{
    BOOLEAN  first;
    int      c, first_ind;
    Real     dir, t1, t2, limits[2][N_DIMENSIONS];

    limits[0][X] = x_min;
    limits[1][X] = x_max;
    limits[0][Y] = y_min;
    limits[1][Y] = y_max;
    limits[0][Z] = z_min;
    limits[1][Z] = z_max;

    first = TRUE;

    for_less( c, 0, 3 )
    {
        dir = Vector_coord( *direction, c );

        if( dir != 0.0 )
        {
            if( dir < 0.0 )
                first_ind = 1;
            else
                first_ind = 0;

            t1 = (limits[first_ind][c] - Point_coord(*origin,c)) / dir;
            t2 = (limits[1-first_ind][c] - Point_coord(*origin,c)) / dir;

            if( first )
            {
                *t_min = t1;
                *t_max = t2;
                first = FALSE;
            }
            else
            {
                if( t1 > *t_min )
                    *t_min = t1;
                if( t2 < *t_max )
                    *t_max = t2;
            }
        }
    }

    return( *t_min <= *t_max );
}
