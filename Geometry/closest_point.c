
#include  <module.h>

#define  MAX_POINTS    300

private  Real   get_point_polygon_distance(
    Point            *point,
    polygons_struct  *polygons,
    int              poly_index,
    Point            *object_point )
{
    Point    points[MAX_POINTS];
    int      size;

    if( polygons->visibilities == (Smallest_int *) 0 ||
        polygons->visibilities[poly_index] )
    {
        size = get_polygon_points( polygons, poly_index, points );

        return( find_point_polygon_distance( point, size, points,
                                             object_point ) );
    }
    else
        return( 1.0e30 );
}

private  Real   get_point_quadmesh_distance(
    Point            *point,
    quadmesh_struct  *quadmesh,
    int              obj_index,
    Point            *object_point )
{
    Point    points[4];
    int      i, j, m, n;

    get_quadmesh_n_objects( quadmesh, &m, &n );

    i = obj_index / n;
    j = obj_index % n;

    get_quadmesh_patch( quadmesh, i, j, points );

    return( find_point_polygon_distance( point, 4, points, object_point ) );
}

public  void  get_closest_point_on_line_segment(
    Point  *point,
    Point  *p1,
    Point  *p2,
    Point  *closest_point )
{
    Vector   p2_minus_p1, p_minus_p1;
    Real     mag, t;

    SUB_VECTORS( p2_minus_p1, *p2, *p1 );
    SUB_VECTORS( p_minus_p1, *point, *p1 );

    mag = DOT_VECTORS( p2_minus_p1, p2_minus_p1 );

    if( mag == 0.0 )
        *closest_point = *p1;
    else
    {
        t = DOT_VECTORS( p_minus_p1, p2_minus_p1 ) / mag;

        if( t <= 0.0 )
            *closest_point = *p1;
        else if( t >= 1.0 )
            *closest_point = *p2;
        else
            INTERPOLATE_POINTS( *closest_point, *p1, *p2, t );
    }
}

private  Real   get_point_line_segment_distance(
    Point            *point,
    lines_struct     *lines,
    int              obj_index,
    Point            *object_point )
{
    int      line, seg, p1, p2;

    get_line_segment_index( lines, obj_index, &line, &seg );

    p1 = lines->indices[POINT_INDEX(lines->end_indices,line,seg)];
    p2 = lines->indices[POINT_INDEX(lines->end_indices,line,seg+1)];

    get_closest_point_on_line_segment( point, &lines->points[p1],
                                       &lines->points[p2], object_point );

    return( distance_between_points( point, object_point ) );
}

private  Real  get_point_marker_distance(
    Point            *point,
    marker_struct    *marker,
    Point            *object_point )
{
    *object_point = marker->position;

    return( distance_between_points( point, object_point ) );
}

public  Real  get_point_object_distance(
    Point                 *point,
    object_struct         *object,
    int                   obj_index,
    Point                 *object_point )
{
    Real                  dist;

    if( get_object_type( object ) == POLYGONS )
    {
        dist = get_point_polygon_distance( point, get_polygons_ptr(object),
                                           obj_index, object_point );
    }
    else if( get_object_type( object ) == QUADMESH )
    {
        dist = get_point_quadmesh_distance( point, get_quadmesh_ptr(object),
                                            obj_index, object_point );
    }
    else if( get_object_type( object ) == LINES )
    {
        dist = get_point_line_segment_distance( point, get_lines_ptr(object),
                                                obj_index, object_point );
    }
    else if( get_object_type( object ) == MARKER )
    {
        dist = get_point_marker_distance( point, get_marker_ptr(object),
                                          object_point );
    }
    else
        dist = 1.0e30;

    return( dist );
}

public  Real  find_closest_point_on_object(
    Point           *point,
    object_struct   *object,
    int             *obj_index,
    Point           *point_on_object )
{
    Real             closest_dist, dist;
    Point            obj_point;
    lines_struct     *lines;
    polygons_struct  *polygons;
    quadmesh_struct  *quadmesh;
    int              i, m, n, n_objects;

    if( obj_index != (int *) NULL )
        *obj_index = -1;

    switch( get_object_type( object ) )
    {
    case LINES:
        lines = get_lines_ptr( object );
        if( lines->n_items == 0 )
        {
            n_objects = 0;
            break;
        }

        if( lines->bintree != (bintree_struct *) NULL )
        {
            return( find_closest_point_in_bintree( point, lines->bintree,
                             object, obj_index, point_on_object ) );
        }
   
        n_objects = lines->end_indices[lines->n_items-1] - lines->n_items;
        break;

    case POLYGONS:
        polygons = get_polygons_ptr( object );
        if( polygons->bintree != (bintree_struct *) NULL )
        {
            return( find_closest_point_in_bintree( point, polygons->bintree,
                             object, obj_index, point_on_object ) );
        }
        n_objects = polygons->n_items;
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        if( quadmesh->bintree != (bintree_struct *) NULL )
        {
            return( find_closest_point_in_bintree( point, quadmesh->bintree,
                             object, obj_index, point_on_object ) );
        }
        get_quadmesh_n_objects( quadmesh, &m, &n );
        n_objects = m * n;
        break;

    case MARKER:
        n_objects = 1;
        break;

    default:
        n_objects = 0;
        break;
    }

    closest_dist = 0.0;
    for_less( i, 0, n_objects )
    {
        dist = get_point_object_distance( point, object, i, &obj_point);

        if( i == 0 || dist < closest_dist )
        {
            closest_dist = dist;
            *obj_index = i;
            *point_on_object = obj_point;
        }
    }

    return( closest_dist );
}
