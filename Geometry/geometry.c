
#include  <internal_volume_io.h>
#include  <geom.h>

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
    int     i, next_i;
    Vector  v1, v2;

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

    if( null_Vector( normal ) )
    {
        for_less( i, 0, n_points )
        {
            SUB_POINTS( v1, points[(i+1)%n_points], points[i] );
            SUB_POINTS( v2, points[(i-1)%n_points], points[i] );
            CROSS_VECTORS( *normal, v1, v2 );
            if( !null_Vector( normal ) )
                break;
        }
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
    *t_min = 0.0;
    *t_max = 0.0;

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
        else
        {
            if( Point_coord(*origin,c) < limits[0][c] ||
                Point_coord(*origin,c) > limits[1][c] )
            {
                *t_min = 0.0;
                *t_max = -1.0;
                break;
            }
        }
    }

    return( *t_min <= *t_max );
}
