#include  <internal_volume_io.h>
#include  <geom.h>

private  Real  get_baricentric(
    Point       *point,
    Point       *p1,
    Point       *p2,
    Point       *p3 )
{
    Real     weight;
    Vector   hor, up, normal, vert, point_offset;

    SUB_POINTS( point_offset, *point, *p1 );
    SUB_POINTS( hor, *p2, *p1 );
    SUB_POINTS( up, *p3, *p1 );

    CROSS_VECTORS( normal, hor, up );
    CROSS_VECTORS( vert, normal, hor );

    weight = DOT_VECTORS( point_offset, vert ) / DOT_VECTORS( up, vert );

    return( weight );
}

private  void  get_triangle_interpolation_weights(
    Point       *point,
    Point       points[],
    Real        weights[] )
{
    weights[0] = get_baricentric( point, &points[1], &points[2], &points[0] );
    weights[1] = get_baricentric( point, &points[2], &points[0], &points[1] );
    weights[2] = get_baricentric( point, &points[0], &points[1], &points[2] );
}

private  Real   distance_to_segment(
    Point   *point,
    Point   *p1,
    Point   *p2,
    Real    *alpha )
{
    Vector   delta, offset, scaled_delta;
    Real     d_dot_d;

    SUB_POINTS( delta, *p2, *p1 );
    SUB_POINTS( offset, *point, *p1 );

    d_dot_d = DOT_VECTORS( delta, delta );

    if( d_dot_d == 0.0 )
    {
        *alpha = 0.0;
        return( distance_between_points( point, p1 ) );
    }

    *alpha = DOT_VECTORS( offset, delta ) / d_dot_d;

    if( *alpha <= 0.0 )
    {
        *alpha = 0.0;
        return( distance_between_points( point, p1 ) );
    }
    else if( *alpha >= 1.0 )
    {
        *alpha = 1.0;
        return( distance_between_points( point, p2 ) );
    }

    SCALE_VECTOR( scaled_delta, delta, *alpha );
    SUB_POINTS( offset, offset, scaled_delta );

    return( MAGNITUDE( offset ) );
}

private  BOOLEAN  intersect_lines(
    Point    *p1,
    Point    *p2,
    Point    *q1,
    Point    *q2,
    Point    *intersect )
{
    BOOLEAN  intersects;
    Real     t, bottom;
    Vector   dp, dq;

    SUB_POINTS( dp, *p2, *p1 );
    SUB_POINTS( dq, *q2, *q1 );

    bottom = Vector_x(dp) * Vector_y(dq) - Vector_y(dp) * Vector_x(dq);

    if( bottom == 0.0 )
    {
        intersects = FALSE;
    }
    else
    {
        t = (Vector_x(dq) * (Point_y(*p1) - Point_y(*q1)) +
             Vector_y(dq) * (Point_x(*q1) - Point_x(*p1))) / bottom;
        GET_POINT_ON_RAY( *intersect, *p1, dp, t );
        intersects = TRUE;
    }

    return( intersects );
}

private  Real  get_two_d_coordinate(
    Point    *p,
    Point    *p1,
    Point    *p2,
    Point    *q1,
    Point    *q2 )
{
    Real     factor;
    Point    intersect, point;
    Vector   p_p1, p1_q1, rotated;

    SUB_POINTS( p1_q1, *q1, *p1 );

    if( intersect_lines( p1, p2, q1, q2, &intersect ) )
    {
        if( !intersect_lines( &intersect, p, p1, q1, &point ) )
        {
/*
            handle_internal_error( "get_two_d_coordinate" );
*/
            return( 0.0 );
        }
        SUB_POINTS( p_p1, point, *p1 );
        factor = DOT_VECTORS( p_p1, p1_q1 ) / DOT_VECTORS( p1_q1, p1_q1 );
    }
    else
    {
        fill_Vector( rotated, Point_y(*p2) - Point_y(*p1),
                              Point_x(*p1) - Point_x(*p2), 0.0 );
        SUB_POINTS( p_p1, *p, *p1 );

        factor = DOT_VECTORS( p_p1, rotated ) / DOT_VECTORS( p1_q1, rotated );
    }

    return( factor );
}

private  void  get_quadrilateral_interpolation_weights(
    Point       *point,
    Point       points[],
    Real        weights[] )
{
    Real     u, v;

    u = get_two_d_coordinate( point,
                              &points[0], &points[3], &points[1], &points[2] );
    v = get_two_d_coordinate( point,
                              &points[0], &points[1], &points[3], &points[2] );

    weights[0] = (1.0 - u) * (1.0 - v);
    weights[1] =        u  * (1.0 - v);
    weights[2] =        u  *        v;
    weights[3] = (1.0 - u) *        v;
}

private  void  get_arbitrary_polygon_interpolation_weights(
    Point       *point,
    int         n_points,
    Point       points[],
    Real        weights[] )
{
    int      i, j;
    Real     sum_weights, alpha, dist;

    for_less( i, 0, n_points )
        weights[i] = 0.0;

    sum_weights = 0.0;

    for_less( i, 0, n_points )
    {
        dist = distance_to_segment( point, &points[i], &points[(i+1)%n_points],
                                    &alpha );

        if( dist == 0.0 )
        {
            for_less( j, 0, n_points )
                weights[j] = 0.0;
            weights[i] = 1.0 - alpha;
            weights[(i+1)%n_points] = alpha;
            sum_weights = 1.0;
            break;
        }

        weights[i] += (1.0 - alpha) / dist;
        weights[(i+1)%n_points] += alpha / dist;

        sum_weights += 1.0 / dist;
    }

    for_less( i, 0, n_points )
        weights[i] /= sum_weights;
}

public  void  get_polygon_interpolation_weights(
    Point       *point,
    int         n_points,
    Point       points[],
    Real        weights[] )
{
    if( n_points == 3 )
        get_triangle_interpolation_weights( point, points, weights );
    else if( n_points == 4 )
        get_quadrilateral_interpolation_weights( point, points, weights );
    else
        get_arbitrary_polygon_interpolation_weights( point, n_points, points,
                                                     weights );
}

public  void  map_point_between_polygons(
    polygons_struct  *p1,
    int              poly_index,
    Point            *p1_point,
    polygons_struct  *p2,
    Point            *p2_point )
{
    int     i, size;
    Point   poly1_points[MAX_POINTS_PER_POLYGON];
    Point   poly2_points[MAX_POINTS_PER_POLYGON];
    Point   scaled_point;
    Real    weights[MAX_POINTS_PER_POLYGON];

    size = get_polygon_points( p1, poly_index, poly1_points );

    get_polygon_interpolation_weights( p1_point, size, poly1_points, weights );

    if( get_polygon_points( p2, poly_index, poly2_points ) != size )
        handle_internal_error( "map_point_between_polygons" );

    fill_Point( *p2_point, 0.0, 0.0, 0.0 );

    for_less( i, 0, size )
    {
        SCALE_POINT( scaled_point, poly2_points[i], weights[i] );
        ADD_POINTS( *p2_point, *p2_point, scaled_point );
    }
}

public  void  map_point_to_unit_sphere(
    polygons_struct  *p,
    Point            *point,
    polygons_struct  *unit_sphere,
    Point            *unit_sphere_point )
{
    Real     mag;
    Point    poly_point;
    Vector   offset;
    int      poly;

    poly = find_closest_polygon_point( point, p, &poly_point );

    map_point_between_polygons( p, poly, &poly_point, unit_sphere,
                                unit_sphere_point );

    CONVERT_POINT_TO_VECTOR( offset, *unit_sphere_point );

    mag = MAGNITUDE( offset );

    if( mag != 1.0 )
    {
        SCALE_POINT( *unit_sphere_point, *unit_sphere_point, 1.0 / mag );
    }
}

private  void  polygon_transform_point(
    object_struct   *src_object,
    object_struct   *dest_object,
    Point           *src_point,
    Point           *dest_point )
{
    int     obj_index;
    Point   point;

    (void) find_closest_point_on_object( src_point, src_object, &obj_index,
                                         &point );

    map_point_between_polygons( get_polygons_ptr(src_object), obj_index,
                                &point, get_polygons_ptr(dest_object),
                                dest_point );
}

public  void  polygon_transform_points(
    polygons_struct   *src_polygons,
    polygons_struct   *dest_polygons,
    int               n_points,
    Point             src_points[],
    Point             dest_points[] )
{
    int             i;
    object_struct   *src_object, *dest_object;

    if( !polygons_are_same_topology( src_polygons, dest_polygons ) )
    {
        print_error(
             "polygon_transform_points: polygons are not same topology.\n" );
        return;
    }

    src_object = create_object( POLYGONS );
    *get_polygons_ptr(src_object) = *src_polygons;
    dest_object = create_object( POLYGONS );
    *get_polygons_ptr(dest_object) = *dest_polygons;

    for_less( i, 0, n_points )
    {
        polygon_transform_point( src_object, dest_object,
                                 &src_points[i], &dest_points[i] );
    }
}
