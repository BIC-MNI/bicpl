
#include  <def_objects.h>
#include  <def_arrays.h>
#include  <def_progress.h>
#include  <def_files.h>
#include  <math.h>

public  void  empty_polygons_struct( polygons )
    polygons_struct   *polygons;
{
    polygons->colour_flag = ONE_COLOUR;
    polygons->colours = (Colour *) 0;

    polygons->n_points = 0;
    polygons->points = (Point *) 0;
    polygons->normals = (Vector *) 0;

    polygons->n_items = 0;

    polygons->end_indices = (int *) 0;
    polygons->indices = (int *) 0;

    polygons->neighbours = (int *) 0;
    polygons->visibilities = (Smallest_int *) 0;
    polygons->bintree = (bintree_struct *) 0;
}

public  Status  set_polygon_per_item_colours( polygons )
    polygons_struct   *polygons;
{
    Status   status;
    Colour   colour;
    int      i;

    status = OK;

    if( polygons->colour_flag != PER_ITEM_COLOURS )
    {
        colour = polygons->colours[0];
        polygons->colour_flag = PER_ITEM_COLOURS;
        REALLOC( status, polygons->colours, polygons->n_items );

        for_less( i, 0, polygons->n_items )
            polygons->colours[i] = colour;

    }

    return( status );
}

public  Status  set_polygon_per_vertex_colours( polygons )
    polygons_struct   *polygons;
{
    Status   status;
    Colour   colour;
    int      i;

    status = OK;

    if( polygons->colour_flag != PER_VERTEX_COLOURS )
    {
        colour = polygons->colours[0];
        polygons->colour_flag = PER_VERTEX_COLOURS;
        REALLOC( status, polygons->colours, polygons->n_points );

        for_less( i, 0, polygons->n_points )
            polygons->colours[i] = colour;

    }

    return( status );
}

public  Status  create_polygons_visibilities( polygons )
    polygons_struct   *polygons;
{
    Status  status;
    int     i;

    status = OK;

    if( polygons->visibilities == (Smallest_int *) 0 &&
        polygons->n_items > 0 )
    {
        ALLOC( status, polygons->visibilities, polygons->n_items );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] = TRUE;
        }
    }

    return( status );
}

public  void  set_polygons_visibilities( polygons, state )
    polygons_struct   *polygons;
    Boolean           state;
{
    int   i;

    if( polygons->visibilities != (Smallest_int *) 0 )
    {
        for_less( i, 0, polygons->n_items )
            polygons->visibilities[i] = state;
    }
}

public  void  get_polygon_centroid( polygons, poly, centroid )
    polygons_struct   *polygons;
    int               poly;
    Point             *centroid;
{
    int      size, p;
    Point    point;

    fill_Point( *centroid, 0.0, 0.0, 0.0 );

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( p, 0, size )
    {
        point = polygons->points[
                  polygons->indices[POINT_INDEX(polygons->end_indices,poly,p)]];
        ADD_POINTS( *centroid, *centroid, point );
    }

    if( size > 0 )
        SCALE_POINT( *centroid, *centroid, 1.0 / (Real) size );
}

public  Status  create_polygons_bintree( polygons, max_nodes )
    polygons_struct   *polygons;
    int               max_nodes;
{
    Status   status;
    Status   create_object_bintree();

    ALLOC( status, polygons->bintree, 1 );

    if( status == OK )
    {
        status = create_object_bintree( polygons->n_points, polygons->points,
                                        polygons->n_items,
                                        polygons->end_indices,
                                        polygons->indices,
                                        polygons->bintree, max_nodes );
 
    }

    return( status );
}

public  Status  delete_polygons_bintree( polygons )
    polygons_struct   *polygons;
{
    Status   status;
    Status   delete_bintree();

    status = OK;

    if( polygons->bintree != (bintree_struct *) 0 )
    {
        status = delete_bintree( polygons->bintree );

        if( status == OK )
        {
            FREE( status, polygons->bintree );
            polygons->bintree = (bintree_struct *) 0;
        }
    }

    return( status );
}

public  Status  delete_polygons( polygons )
    polygons_struct    *polygons;
{
    Status   status;
    Status   delete_polygons_bintree();

    FREE( status, polygons->colours );

    if( polygons->n_points > 0 )
        FREE( status, polygons->points );

    if( status == OK && polygons->n_points > 0 )
        FREE( status, polygons->normals );

    if( status == OK && polygons->n_items > 0 )
        FREE( status, polygons->indices );

    if( status == OK && polygons->n_items > 0 )
        FREE( status, polygons->end_indices );

    if( status == OK && polygons->visibilities != (Smallest_int *) 0 )
        FREE( status, polygons->visibilities );

    if( status == OK && polygons->neighbours != (int *) 0 )
        FREE( status, polygons->neighbours );

    if( status == OK )
        status = delete_polygons_bintree( polygons );

    return( status );
}

public  int  find_vertex_index( polygons, poly, point_index )
    polygons_struct  *polygons;
    int              poly;
    int              point_index;
{
    int   e, index, size, p;

    index = -1;

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( e, 0, size )
    {
        p = polygons->indices[POINT_INDEX(polygons->end_indices,poly,e)];

        if( p == point_index )
        {
            index = e;
            break;
        }
    }

    return( index );
}

public  Boolean  find_next_edge_around_point( polygons, poly, index_1, index_2,
                                       next_poly, next_index_1, next_index_2 )
    polygons_struct   *polygons;
    int               poly;
    int               index_1;
    int               index_2;
    int               *next_poly;
    int               *next_index_1;
    int               *next_index_2;
{
    int                    size, edge, point_index, neighbour_point_index;
    int                    next_neigh_index;

    point_index = polygons->indices[
                       POINT_INDEX( polygons->end_indices, poly, index_1 )];

    neighbour_point_index = polygons->indices[
                       POINT_INDEX( polygons->end_indices, poly, index_2 )];

    size = GET_OBJECT_SIZE( *polygons, poly );

    if( index_2 == (index_1+1) % size )
        edge = index_1;
    else
        edge = index_2;
 
    *next_poly = polygons->neighbours[
                      POINT_INDEX(polygons->end_indices,poly,edge)];

    if( *next_poly >= 0 )
    {
        size = GET_OBJECT_SIZE(*polygons,*next_poly);
        *next_index_1 = find_vertex_index( polygons, *next_poly, point_index );

        *next_index_2 = (*next_index_1 + 1) % size;
        next_neigh_index = polygons->indices[
                           POINT_INDEX( polygons->end_indices, *next_poly,
                                        *next_index_2 )];
        if( next_neigh_index == neighbour_point_index )
        {
            *next_index_2 = (*next_index_1 + size- 1) % size;
        }
    }

    return( *next_poly >= 0 );
}

public  int  get_neighbours_of_point( polygons, poly, vertex_index, neighbours,
                                      max_neighbours, interior_point )
    polygons_struct   *polygons;
    int               poly;
    int               vertex_index;
    int               neighbours[];
    int               max_neighbours;
    Boolean           *interior_point;
{
    int      n_neighbours, current_poly, current_index_within_poly;
    int      size, neighbour_index_within_poly;
    Boolean  found;

    size = GET_OBJECT_SIZE( *polygons, poly );

    current_poly = poly;
    current_index_within_poly = vertex_index;
    neighbour_index_within_poly = (vertex_index-1+size)%size;

    neighbours[0] = polygons->indices[POINT_INDEX(polygons->end_indices,poly,
                          neighbour_index_within_poly)];
    n_neighbours = 1;

    do
    {
        found = find_next_edge_around_point( polygons,
                        current_poly, current_index_within_poly,
                        neighbour_index_within_poly,
                        &current_poly, &current_index_within_poly,
                        &neighbour_index_within_poly );

        if( found && current_poly != poly )
        {
            if( n_neighbours < max_neighbours )
            {
                neighbours[n_neighbours] = polygons->indices[
                       POINT_INDEX(polygons->end_indices,current_poly,
                                   neighbour_index_within_poly)];
                ++n_neighbours;
            }
        }
    }
    while( found && current_poly != poly );

    *interior_point = found;

    return( n_neighbours );
}

#define  MAX_TEMP_STORAGE  1000

public  void  compute_polygon_normal( polygons, poly, normal )
    polygons_struct  *polygons;
    int              poly;
    Vector           *normal;
{
    int                e, size, point_index;
    Point              polygon[MAX_TEMP_STORAGE];
    void               find_polygon_normal();

    size = GET_OBJECT_SIZE( *polygons, poly );
    if( size > MAX_TEMP_STORAGE )
        size = MAX_TEMP_STORAGE;

    for_less( e, 0, size )
    {
        point_index =
              polygons->indices[POINT_INDEX(polygons->end_indices,poly,e)];

        polygon[e] = polygons->points[point_index];
    }

    find_polygon_normal( size, polygon, normal );
}

public  Status  compute_polygon_normals( polygons )
    polygons_struct  *polygons;
{
    int                e, poly, size, point_index, prev_index, next_index;
    Real               scale;
    Real               angle_between_points();
    Vector             normal, normal_scaled;
    void               find_polygon_normal();
    progress_struct    progress;

    for_less( point_index, 0, polygons->n_points )
    {
        fill_Vector( polygons->normals[point_index], 0.0, 0.0, 0.0 );
    }

    initialize_progress_report( &progress, FALSE, polygons->n_items,
                                "Computing Normals" );

    for_less( poly, 0, polygons->n_items )
    {
        compute_polygon_normal( polygons, poly, &normal );

        NORMALIZE_VECTOR( normal, normal );

        size = GET_OBJECT_SIZE( *polygons, poly );

        point_index =
             polygons->indices[POINT_INDEX(polygons->end_indices,poly,size-1)];
        next_index =
             polygons->indices[POINT_INDEX(polygons->end_indices,poly,0)];

        for_less( e, 0, size )
        {
            prev_index = point_index;
            point_index = next_index;
            next_index =
                  polygons->indices[POINT_INDEX(polygons->end_indices,poly,
                                                (e+1)%size)];

            scale = angle_between_points( &polygons->points[prev_index],
                                          &polygons->points[point_index],
                                          &polygons->points[next_index] );

            SCALE_VECTOR( normal_scaled, normal, scale );

            ADD_VECTORS( polygons->normals[point_index],
                         polygons->normals[point_index],
                         normal_scaled );
        }

        update_progress_report( &progress, poly + 1 );
    }

    terminate_progress_report( &progress );

    for_less( point_index, 0, polygons->n_points )
    {
        NORMALIZE_VECTOR( polygons->normals[point_index],
                          polygons->normals[point_index] );
    }

    return( OK );
}

private  Real  angle_between_points( prev, this, next )
    Point  *prev;
    Point  *this;
    Point  *next;
{
    Real    angle, x, y;
    Real    compute_clockwise_rotation();
    Vector  v1, v2, x_axis, y_axis;

    SUB_POINTS( v1, *prev, *this );
    SUB_POINTS( v2, *next, *this );

    NORMALIZE_VECTOR( v1, v1 );
    NORMALIZE_VECTOR( v2, v2 );

    x = DOT_VECTORS( v1, v2 );

    SCALE_VECTOR( x_axis, v1, x );

    SUB_VECTORS( y_axis, v2, x_axis );

    y = MAGNITUDE( y_axis );

    angle = compute_clockwise_rotation( x, y );

    if( angle < 0.0 )  angle = -angle;

    return( angle );
}

public  Boolean  get_plane_polygon_intersection( normal, d, polygons, poly,
                                                 intersection_points )
    Vector           *normal;
    Real             d;
    polygons_struct  *polygons;
    int              poly;
    Point            intersection_points[];
{
    int       i1, i2, edge, size, n_intersections;

    n_intersections = 0;

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( edge, 0, size )
    {
        i1 = polygons->indices[POINT_INDEX(polygons->end_indices,poly,edge)];
        i2 = polygons->indices[
                  POINT_INDEX(polygons->end_indices,poly,(edge+1)%size)];

        if( get_plane_segment_intersection( normal, d, &polygons->points[i1],
                                        &polygons->points[i2],
                                        &intersection_points[n_intersections] ))
        {
            ++n_intersections;

            if( n_intersections == 2 )
                break;
        }
    }

    return( n_intersections == 2 );
}

public  Boolean  get_plane_segment_intersection( normal, d, p1, p2,
                                                 intersection_point )
    Vector           *normal;
    Real             d;
    Point            *p1;
    Point            *p2;
    Point            *intersection_point;
{
    Real     dist1, dist2, t;
    Boolean  intersects;
    Point    t1, t2;

    dist1 = DIST_FROM_PLANE( *normal, d, *p1 );
    dist2 = DIST_FROM_PLANE( *normal, d, *p2 );

    if( (dist1 <= 0.0 && dist2 >= 0.0 || dist1 >= 0.0 && dist2 <= 0.0) &&
        dist1 != dist2 )
    {
        t = dist1 / (dist1 - dist2);

        SCALE_POINT( t1, *p1, 1.0 - t );
        SCALE_POINT( t2, *p2, t );
        ADD_POINTS( *intersection_point, t1, t2 );

        intersects = TRUE;
    }
    else
        intersects = FALSE;

    return( intersects );
}

public  Status   remove_invisible_polygons( polygons )
    polygons_struct  *polygons;
{
    Status           status;
    int              tmp_n_points, i, n_points, n_items, *indices, *end_indices;
    int              p, poly, n_indices, size, point_index;
    int              *new_ids;
    Point            *points;
    Vector           *normals;
    Colour           col;

    n_points = 0;
    n_items = 0;
    n_indices = 0;

    ALLOC( status, new_ids, polygons->n_points );

    if( status == OK )
    {
        for_less( i, 0, polygons->n_points )
            new_ids[i] = -1;

        for_less( poly, 0, polygons->n_items )
        {
            if( polygons->visibilities[poly] )
            {
                size = GET_OBJECT_SIZE( *polygons, poly );

                for_less( p, 0, size )
                {
                    point_index = polygons->indices[
                            POINT_INDEX(polygons->end_indices,poly,p)];

                    if( new_ids[point_index] < 0 )
                    {
                        new_ids[point_index] = n_points;

                        tmp_n_points = n_points;

                        ADD_ELEMENT_TO_ARRAY( status, tmp_n_points,
                             points, polygons->points[point_index],
                             DEFAULT_CHUNK_SIZE );

                        if( status == OK )
                        {
                            ADD_ELEMENT_TO_ARRAY( status, n_points,
                                 normals, polygons->normals[point_index],
                                 DEFAULT_CHUNK_SIZE );
                        }
                    }

                    ADD_ELEMENT_TO_ARRAY( status, n_indices, indices,
                             new_ids[point_index], DEFAULT_CHUNK_SIZE );
                }

                ADD_ELEMENT_TO_ARRAY( status, n_items, end_indices,
                                      n_indices, DEFAULT_CHUNK_SIZE );
            }
        }
    }

    if( status == OK )
        FREE( status, new_ids );

    col = polygons->colours[0];

    if( status == OK )
        status = delete_polygons( polygons );

    if( status == OK )
    {
        ALLOC( status, polygons->colours, 1 );

        polygons->colour_flag = ONE_COLOUR;
        polygons->colours[0] = col;
        polygons->n_points = n_points;
        polygons->points = points;
        polygons->normals = normals;
        polygons->n_items = n_items;
        polygons->indices = indices;
        polygons->end_indices = end_indices;

        status = create_polygons_visibilities( polygons );
    }

    return( status );
}

public  void   reverse_polygons_vertices( polygons )
    polygons_struct  *polygons;
{
    Status      status;
    int         poly;
    void        reverse_polygon_order();

    if( polygons->neighbours != (int *) 0 )
        FREE( status, polygons->neighbours );

    if( status == OK )
    {
        for_less( poly, 0, polygons->n_items )
        {
            reverse_polygon_order( polygons, poly );
        }
    }
}

public  void   make_polygons_front_facing( polygons )
    polygons_struct  *polygons;
{
    Status      status;
    int         poly;
    void        reverse_polygon_order();

    if( polygons->neighbours != (int *) 0 )
        FREE( status, polygons->neighbours );

    if( status == OK )
    {
        for_less( poly, 0, polygons->n_items )
        {
            if( polygon_is_back_facing( polygons, poly ) )
                reverse_polygon_order( polygons, poly );
        }
    }
}

public  Boolean  polygon_is_back_facing( polygons, poly )
    polygons_struct   *polygons;
    int               poly;
{
    int      i, size, point_index;
    Vector   avg_vertex_normal, polygon_normal;

    compute_polygon_normal( polygons, poly, &polygon_normal );

    size = GET_OBJECT_SIZE( *polygons, poly );

    fill_Vector( avg_vertex_normal, 0.0, 0.0, 0.0 );

    for_less( i, 0, size )
    {
        point_index = polygons->indices[
                          POINT_INDEX(polygons->end_indices,poly,i)];
        ADD_VECTORS( avg_vertex_normal, avg_vertex_normal,
                     polygons->normals[point_index] );
    }

    return( DOT_VECTORS( avg_vertex_normal, polygon_normal ) > 0.0 );
}

private  void  reverse_polygon_order( polygons, poly )
    polygons_struct   *polygons;
    int               poly;
{
    int   i, size, start, tmp_swap;

    size = GET_OBJECT_SIZE( *polygons, poly );

    start = POINT_INDEX( polygons->end_indices, poly, 0 );

    for_less( i, 0, size / 2 )
    {
        tmp_swap = polygons->indices[start+(size-1 - i)];
        polygons->indices[start+(size-1 - i)] = polygons->indices[start+i];
        polygons->indices[start+i] = tmp_swap;
    }
}

#define  MAX_NEIGHBOURS   1000

public  void  compute_polygon_point_centroid( polygons, poly, vertex_index,
                                              point_index, centroid, normal,
                                              base_length, curvature )
    polygons_struct  *polygons;
    int              poly;
    int              vertex_index;
    int              point_index;
    Point            *centroid;
    Vector           *normal;
    Real             *base_length;
    Real             *curvature;
{
    int              i, n_neighbours;
    int              neighbours[MAX_NEIGHBOURS];
    Point            neigh_points[MAX_NEIGHBOURS];
    void             get_points_centroid();
    void             find_polygon_normal();
    Real             estimate_polygon_curvature();
    Boolean          interior_point;

    n_neighbours = get_neighbours_of_point( polygons, poly, vertex_index,
                                            neighbours, MAX_NEIGHBOURS,
                                            &interior_point );

    if( interior_point && n_neighbours > 2 )
    {
        for_less( i, 0, n_neighbours )
            neigh_points[i] = polygons->points[neighbours[i]];

        get_points_centroid( n_neighbours, neigh_points,
                             centroid );

        find_polygon_normal( n_neighbours, neigh_points, normal );

        *curvature = estimate_polygon_curvature( &polygons->points[point_index],
                                       n_neighbours, neigh_points,
                                       centroid, normal, base_length );
    }
    else
    {
        *centroid = polygons->points[point_index];
        fill_Vector( *normal, 0.0, 0.0, 0.0 );
        *base_length = 1.0;
        *curvature = 0.0;
    }
}

private  Real  estimate_polygon_curvature( point, n_neighs, neighs, centroid,
                                           normal, base_length )
    Point   *point;
    int     n_neighs;
    Point   neighs[];
    Point   *centroid;
    Vector  *normal;
    Real    *base_length;
{
    Vector to_point;
    Real   distance_between_points();
    Real   curvature;

    *base_length = distance_between_points( &neighs[0], centroid ) +
                   distance_between_points( &neighs[n_neighs/2], centroid );

    if( *base_length == 0.0 )
        *base_length = 1.0;

    SUB_POINTS( to_point, *point, *centroid );

    curvature = MAGNITUDE( to_point ) / (*base_length);

    if( DOT_VECTORS( to_point, *normal ) < 0.0 )
        curvature = -curvature;

    return( curvature );
}
