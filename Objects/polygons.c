
#include  <mni.h>

private  Real  angle_between_points(
    Point  *prev,
    Point  *this,
    Point  *next );
private  void  reverse_polygon_order(
    polygons_struct   *polygons,
    int               poly );
private  Real  estimate_polygon_curvature(
    Point   *point,
    int     n_neighs,
    Point   neighs[],
    Point   *centroid,
    Vector  *normal,
    Real    *base_length );

public  void  initialize_polygons(
    polygons_struct   *polygons,
    Colour            col,
    Surfprop          *spr )
{
    ALLOC( polygons->colours, 1 );

    polygons->colour_flag = ONE_COLOUR;

    polygons->colours[0] = col;

    if( spr != (Surfprop *) 0 )
        polygons->surfprop = *spr;

    polygons->n_points = 0;
    polygons->points = (Point *) 0;
    polygons->normals = (Vector *) 0;

    polygons->line_thickness = 1;

    polygons->n_items = 0;
    polygons->end_indices = (int *) 0;
    polygons->indices = (int *) 0;

    polygons->visibilities = (Smallest_int *) 0;
    polygons->neighbours = (int *) 0;
    polygons->bintree = (bintree_struct_ptr) NULL;
}

public  void  delete_polygons( polygons_struct *polygons )
{
    free_colours( polygons->colour_flag, polygons->colours, polygons->n_points,
                  polygons->n_items );

    if( polygons->n_points > 0 )
        FREE( polygons->points );

    if( polygons->n_points > 0 && polygons->normals != (Vector *) 0 )
        FREE( polygons->normals );

    if( polygons->n_items > 0 )
        FREE( polygons->indices );

    if( polygons->n_items > 0 )
        FREE( polygons->end_indices );

    if( polygons->visibilities != (Smallest_int *) 0 )
        FREE( polygons->visibilities );

    if( polygons->neighbours != (int *) 0 )
        FREE( polygons->neighbours );

    delete_bintree_if_any( &polygons->bintree );

    polygons->visibilities = (Smallest_int *) 0;
    polygons->neighbours = (int *) 0;
}

public  void  copy_polygons(
    polygons_struct   *src,
    polygons_struct   *dest )
{
    int   i, n_indices, n_colours;

    *dest = *src;

    n_colours = get_n_colours( src->colour_flag, src->n_points, src->n_items );

    ALLOC( dest->colours, n_colours );
    for_less( i, 0, n_colours )
        dest->colours[i] = src->colours[i];

    ALLOC( dest->points, src->n_points );
    if( src->normals != (Vector *) NULL )
        ALLOC( dest->normals, src->n_points )

    for_less( i, 0, src->n_points )
    {
        dest->points[i] = src->points[i];
        if( src->normals != (Vector *) NULL )
            dest->normals[i] = src->normals[i];
        
    }

    ALLOC( dest->end_indices, src->n_items );
    for_less( i, 0, src->n_items )
        dest->end_indices[i] = src->end_indices[i];

    n_indices = NUMBER_INDICES( *src );
    ALLOC( dest->indices, n_indices );

    for_less( i, 0, n_indices )
        dest->indices[i] = src->indices[i];

    dest->visibilities = (Smallest_int *) 0;
    dest->neighbours = (int *) 0;
    dest->bintree = (bintree_struct_ptr) NULL;
}

public  void  set_polygon_per_item_colours(
    polygons_struct   *polygons )
{
    Colour   colour;
    int      i;

    if( polygons->colour_flag != PER_ITEM_COLOURS )
    {
        colour = polygons->colours[0];
        polygons->colour_flag = PER_ITEM_COLOURS;
        REALLOC( polygons->colours, polygons->n_items );

        for_less( i, 0, polygons->n_items )
            polygons->colours[i] = colour;

    }
}

public  void  create_polygons_visibilities(
    polygons_struct   *polygons )
{
    int     i;

    if( polygons->visibilities == (Smallest_int *) 0 &&
        polygons->n_items > 0 )
    {
        ALLOC( polygons->visibilities, polygons->n_items );

        for_less( i, 0, polygons->n_items )
            polygons->visibilities[i] = TRUE;
    }
}

public  void  set_polygons_visibilities(
    polygons_struct   *polygons,
    BOOLEAN           state )
{
    int   i;

    if( polygons->visibilities != (Smallest_int *) 0 )
    {
        for_less( i, 0, polygons->n_items )
            polygons->visibilities[i] = state;
    }
}

public  void  start_new_polygon( polygons_struct *polygons )
{
    int      n_indices;

    n_indices = NUMBER_INDICES( *polygons );

    ADD_ELEMENT_TO_ARRAY( polygons->end_indices, polygons->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}

public  void  add_point_to_polygon(
    polygons_struct   *polygons,
    Point             *point,
    Vector            *normal )
{
    int      n_points;

    if( polygons->n_items == 0 )
        start_new_polygon( polygons );

    if( polygons->n_points > 1 )
    {
        if( normal != (Vector *) 0 && polygons->normals == (Vector *) 0 ||
            normal == (Vector *) 0 && polygons->normals != (Vector *) 0 )
        {
            print(
              "Error: be consistent with normals in add_point_to_polygon.\n" );
        }
    }

    ADD_ELEMENT_TO_ARRAY( polygons->indices,
                          polygons->end_indices[polygons->n_items-1],
                          polygons->n_points, DEFAULT_CHUNK_SIZE );

    if( normal != (Vector *) 0 )
    {
        n_points = polygons->n_points;
        ADD_ELEMENT_TO_ARRAY( polygons->normals, n_points,
                              *normal, DEFAULT_CHUNK_SIZE );
    }

    ADD_ELEMENT_TO_ARRAY( polygons->points, polygons->n_points,
                          *point, DEFAULT_CHUNK_SIZE );
}

public  int  get_polygon_points(
    polygons_struct   *polygons,
    int               poly,
    Point             points[] )
{
    int      size, p;

    size = GET_OBJECT_SIZE( *polygons, poly );

    for_less( p, 0, size )
    {
        points[p] = polygons->points[
                 polygons->indices[POINT_INDEX(polygons->end_indices,poly,p)]];
    }

    return( size );
}

public  void  get_polygon_centroid(
    polygons_struct   *polygons,
    int               poly,
    Point             *centroid )
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

public  int  find_vertex_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index )
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

public  int  find_edge_index(
    polygons_struct  *polygons,
    int              poly,
    int              point_index1,
    int              point_index2 )
{
    int   e, index, size, p1, p2;

    index = -1;

    size = GET_OBJECT_SIZE( *polygons, poly );

    p2 = polygons->indices[POINT_INDEX(polygons->end_indices,poly,0)];
    for_less( e, 0, size )
    {
        p1 = p2;
        p2 = polygons->indices[POINT_INDEX(polygons->end_indices,poly,
                               (e+1)%size)];

        if( p1 == point_index1 && p2 == point_index2 ||
            p1 == point_index2 && p2 == point_index1 )
        {
            index = e;
            break;
        }
    }

    return( index );
}

public  BOOLEAN  find_polygon_with_edge(
    polygons_struct  *polygons,
    int              point_index1,
    int              point_index2,
    int              *poly_containing_edge,
    int              *edge_index )
{
    int   poly;

    for_less( poly, 0, polygons->n_items )
    {
        *edge_index = find_edge_index( polygons, poly,
                                       point_index1, point_index2 );

        if( *edge_index >= 0 )
        {
            *poly_containing_edge = poly;
            break;
        }
    }

    return( poly < polygons->n_items );
}

public  BOOLEAN  lookup_polygon_vertex(
    polygons_struct   *polygons,
    Point             *point,
    int               *point_index )
{
    int      i;

    for_less( i, 0, polygons->n_points )
    {
        if( EQUAL_POINTS( polygons->points[i], *point ) )
        {
            *point_index = i;
            break;
        }
    }

    return( i < polygons->n_points );
}

public  BOOLEAN  find_polygon_with_vertex(
    polygons_struct   *polygons,
    int               point_index,
    int               *poly_index,
    int               *vertex_index )
{
    int      poly, size, i;
    BOOLEAN  found;

    found = FALSE;

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        for_less( i, 0, size )
        {
            if( polygons->indices[POINT_INDEX(polygons->end_indices,poly,i)] ==
                point_index )
            {
                *poly_index = poly;
                *vertex_index = i;
                found = TRUE;
                break;
            }
        }
    }

    return( found );
}

public  BOOLEAN  find_next_edge_around_point(
    polygons_struct   *polygons,
    int               poly,
    int               index_1,
    int               index_2,
    int               *next_poly,
    int               *next_index_1,
    int               *next_index_2 )
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

public  int  get_neighbours_of_point(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               neighbours[],
    int               max_neighbours,
    BOOLEAN           *interior_point )
{
    int      n_neighbours, current_poly, current_index_within_poly;
    int      size, neighbour_index_within_poly;
    BOOLEAN  found;

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

public  int  get_polygons_around_vertex(
    polygons_struct   *polygons,
    int               poly,
    int               vertex_index,
    int               poly_indices[],
    int               n_polys_alloced )
{
    int      current_poly, current_index_within_poly;
    int      size, neighbour_index_within_poly;
    int      n_polys, dir;
    BOOLEAN  found;

    size = GET_OBJECT_SIZE( *polygons, poly );

    poly_indices[0] = poly;
    n_polys = 1;

    for( dir = -1;  dir <= 1;  dir +=2 )
    {
        current_poly = poly;
        current_index_within_poly = vertex_index;
        neighbour_index_within_poly = (vertex_index + size + dir) % size;

        do
        {
            found = find_next_edge_around_point( polygons,
                            current_poly, current_index_within_poly,
                            neighbour_index_within_poly,
                            &current_poly, &current_index_within_poly,
                            &neighbour_index_within_poly );

            if( found && current_poly != poly )
            {
                if( n_polys < n_polys_alloced )
                {
                    poly_indices[n_polys] = current_poly;
                    ++n_polys;
                }
            }
        }
        while( found && current_poly != poly );

        if( found ) break;
    }

    return( n_polys );
}

#define  MAX_TEMP_STORAGE  1000

public  void  compute_polygon_normal(
    polygons_struct  *polygons,
    int              poly,
    Vector           *normal )
{
    int                e, size, point_index;
    Point              polygon[MAX_TEMP_STORAGE];

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

public  void  compute_polygon_normals(
    polygons_struct  *polygons )
{
    int                e, poly, size, point_index, prev_index, next_index;
    Real               scale;
    Vector             normal, normal_scaled;
    progress_struct    progress;

    for_less( point_index, 0, polygons->n_points )
        fill_Vector( polygons->normals[point_index], 0.0, 0.0, 0.0 );

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
}

private  Real  angle_between_points(
    Point  *prev,
    Point  *this,
    Point  *next )
{
    Real    angle, x, y;
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

public  void  average_polygon_normals(
    polygons_struct  *polygons,
    int              n_iters,
    Real             neighbour_weight )
{
    Real               avg_dot_prod;
    int                e, poly, size, point_index, neigh_index, i;
    Vector             *neigh_normal_sum, *new_normals, new_normal;
    progress_struct    progress;

    if( polygons->n_points <= 0 || polygons->n_items <= 0 )
        return;

    compute_polygon_normals( polygons );

    ALLOC( new_normals, polygons->n_points );
    for_less( point_index, 0, polygons->n_points )
         new_normals[point_index] = polygons->normals[point_index];

    ALLOC( neigh_normal_sum, polygons->n_points );

    for_less( i, 0, n_iters )
    {
        for_less( point_index, 0, polygons->n_points )
            fill_Vector( neigh_normal_sum[point_index], 0.0, 0.0, 0.0 );

        initialize_progress_report( &progress, FALSE, polygons->n_items,
                                    "Averaging Normals" );

        for_less( poly, 0, polygons->n_items )
        {
            size = GET_OBJECT_SIZE( *polygons, poly );

            for_less( e, 0, size )
            {
                point_index = polygons->indices[
                           POINT_INDEX(polygons->end_indices,poly,e)];
                neigh_index = polygons->indices[
                           POINT_INDEX(polygons->end_indices,poly,(e+1)%size)];

                ADD_VECTORS( neigh_normal_sum[point_index],
                             neigh_normal_sum[point_index],
                             new_normals[neigh_index] );
                ADD_VECTORS( neigh_normal_sum[neigh_index],
                             neigh_normal_sum[neigh_index],
                             new_normals[point_index] );
            }

            update_progress_report( &progress, poly + 1 );
        }

        terminate_progress_report( &progress );

        avg_dot_prod = 0.0;

        for_less( point_index, 0, polygons->n_points )
        {
            NORMALIZE_VECTOR( neigh_normal_sum[point_index],
                              neigh_normal_sum[point_index] );

            INTERPOLATE_VECTORS( new_normal,
                                 polygons->normals[point_index],
                                 neigh_normal_sum[point_index],
                                 neighbour_weight );

            NORMALIZE_VECTOR( new_normal, new_normal );

            avg_dot_prod += DOT_VECTORS( new_normal, new_normals[point_index] );
            new_normals[point_index] = new_normal;
        }

        avg_dot_prod /= (Real) polygons->n_points;

        print( "Average dot product: %g\n", avg_dot_prod );
    }

    for_less( point_index, 0, polygons->n_points )
         polygons->normals[point_index] = new_normals[point_index];

    FREE( neigh_normal_sum );
    FREE( new_normals );
}

public  BOOLEAN  get_plane_polygon_intersection(
    Vector           *normal,
    Real             d,
    polygons_struct  *polygons,
    int              poly,
    Point            intersection_points[] )
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

public  BOOLEAN  get_plane_segment_intersection(
    Vector           *normal,
    Real             d,
    Point            *p1,
    Point            *p2,
    Point            *intersection_point )
{
    Real     dist1, dist2, t;
    BOOLEAN  intersects;
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

public  void   reverse_polygons_vertices(
    polygons_struct  *polygons )
{
    int         poly;

    if( polygons->neighbours != (int *) 0 )
        FREE( polygons->neighbours );

    for_less( poly, 0, polygons->n_items )
        reverse_polygon_order( polygons, poly );
}

public  void   make_polygons_front_facing(
    polygons_struct  *polygons )
{
    int         poly;

    if( polygons->neighbours != (int *) 0 )
        FREE( polygons->neighbours );

    for_less( poly, 0, polygons->n_items )
    {
        if( polygon_is_back_facing( polygons, poly ) )
            reverse_polygon_order( polygons, poly );
    }
}

public  BOOLEAN  polygon_is_back_facing(
    polygons_struct   *polygons,
    int               poly )
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

private  void  reverse_polygon_order(
    polygons_struct   *polygons,
    int               poly )
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

public  void  compute_points_centroid_and_normal(
    polygons_struct  *polygons,
    int              point_index,
    int              n_neighbours,
    int              neighbours[],
    Point            *centroid,
    Vector           *normal,
    Real             *base_length,
    Real             *curvature )
{
    int              i;
    Point            neigh_points[MAX_NEIGHBOURS];

    if( n_neighbours > 2 )
    {
        for_less( i, 0, n_neighbours )
            neigh_points[i] = polygons->points[neighbours[i]];

        get_points_centroid( n_neighbours, neigh_points, centroid );

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

public  void  compute_polygon_point_centroid(
    polygons_struct  *polygons,
    int              poly,
    int              vertex_index,
    int              point_index,
    Point            *centroid,
    Vector           *normal,
    Real             *base_length,
    Real             *curvature )
{
    int              n_neighbours;
    int              neighbours[MAX_NEIGHBOURS];
    BOOLEAN          interior_point;

    n_neighbours = get_neighbours_of_point( polygons, poly, vertex_index,
                                            neighbours, MAX_NEIGHBOURS,
                                            &interior_point );

    compute_points_centroid_and_normal( polygons, point_index,
                                        n_neighbours, neighbours,
                                        centroid, normal, base_length,
                                        curvature );
}

private  Real  estimate_polygon_curvature(
    Point   *point,
    int     n_neighs,
    Point   neighs[],
    Point   *centroid,
    Vector  *normal,
    Real    *base_length )
{
    int    i;
    Real   curvature, len;
    Vector to_point;

    len = 0.0;
    for_less( i, 0, n_neighs )
        len += distance_between_points( &neighs[i], centroid );

    if( n_neighs > 0 )
        len = 2.0 * len / (Real) n_neighs;

    if( len == 0.0 )
        len = 1.0;

    *base_length = len;

    SUB_POINTS( to_point, *point, *centroid );

    curvature = MAGNITUDE( to_point ) / len;

    if( DOT_VECTORS( to_point, *normal ) < 0.0 )
        curvature = -curvature;

    return( curvature );
}

private  void  get_opposite_point(
    polygons_struct  *polygons,
    int              poly,
    int              edge,
    Point            *point )
{
    int  v, size;

    size = GET_OBJECT_SIZE( *polygons, poly );

    if( size == 3 )
        v = (edge + 2) % size;
    else
        v = (edge + size/2) % size;

    *point = polygons->points[polygons->indices[
                         POINT_INDEX( polygons->end_indices, poly, v)]];
}

public  Real  get_polygon_edge_angle(
    polygons_struct  *polygons,
    int              poly,
    int              edge )
{
    int     size, i, point_index1, point_index2, neighbour_poly;
    Real    angle, edge_len_squared, scale, x, y;
    Point   p1, p2, poly1_point, poly2_point;
    Vector  v1, v2, normal, diff, edge_vec;

    neighbour_poly = polygons->neighbours[POINT_INDEX( polygons->end_indices,
                                                       poly, edge )];

    if( neighbour_poly < 0 )
        return( PI );

    size = GET_OBJECT_SIZE( *polygons, poly );

    point_index1 = polygons->indices[ POINT_INDEX( polygons->end_indices,
                                                   poly, edge )];
    point_index2 = polygons->indices[ POINT_INDEX( polygons->end_indices,
                                                   poly, (edge+1)%size )];
    p1 = polygons->points[point_index1];
    p2 = polygons->points[point_index2];

    get_opposite_point( polygons, poly, edge, &poly1_point );

    i = find_edge_index( polygons, neighbour_poly, point_index1, point_index2 );
    get_opposite_point( polygons, neighbour_poly, i, &poly2_point );

    SUB_POINTS( edge_vec, p2, p1 );
    SUB_POINTS( v1, poly1_point, p1 );
    SUB_POINTS( v2, poly2_point, p1 );

    edge_len_squared = DOT_VECTORS( edge_vec, edge_vec );
    if( edge_len_squared == 0.0 )
        edge_len_squared = 1.0;

    scale = DOT_VECTORS( v1, edge_vec ) / edge_len_squared;
    SCALE_VECTOR( diff, edge_vec, scale );
    SUB_VECTORS( v1, v1, diff );
    NORMALIZE_VECTOR( v1, v1 );

    CROSS_VECTORS( normal, edge_vec, v1 );
    NORMALIZE_VECTOR( normal, normal );

    scale = DOT_VECTORS( v2, edge_vec ) / edge_len_squared;
    SCALE_VECTOR( diff, edge_vec, scale );
    SUB_VECTORS( v2, v2, diff );

    x = DOT_VECTORS( v2, v1 );
    y = -DOT_VECTORS( v2, normal );

    angle = compute_clockwise_rotation( x, y );

    if( angle < 0.0 )
        angle += 2.0 * PI;

    return( angle );
}

public  BOOLEAN  polygons_are_same_topology(
    polygons_struct  *p1,
    polygons_struct  *p2 )
{
    return( objects_are_same_topology( p1->n_points, p1->n_items,
                                       p1->end_indices, p1->indices,
                                       p2->n_points, p2->n_items,
                                       p2->end_indices, p2->indices ) );
}
