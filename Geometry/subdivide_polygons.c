#include  <def_mni.h>
#include  <def_module.h>

private  void  subdivide_polygon(
    polygons_struct   *polygons,
    int               poly,
    int               *new_n_points,
    Point             *new_points[],
    int               *new_n_polygons,
    int               *new_end_indices[],
    int               *new_n_indices,
    int               *new_indices[],
    hash_table_struct *midpoint_table );

public  void  subdivide_polygons(
    polygons_struct  *polygons )
{
    int                i, new_n_points, new_n_indices, new_n_polygons;
    int                *new_indices, *new_end_indices;
    int                *index_ptr;
    Point              *new_points;
    Point              dummy;
    Colour             save_colour;
    hash_table_struct  midpoint_table;
    hash_table_pointer hash_pointer;
    progress_struct    progress;

    if( is_this_sphere_topology( polygons ) )
    {
        (void) printf( "Subdividing assuming sphere topology.\n" );
        create_polygons_sphere( &dummy, 0.0, 0.0, 0.0, 0, 0, TRUE, polygons );
        return;
    }

    save_colour = polygons->colours[0];

    new_n_points = polygons->n_points;
    new_n_polygons = 0;
    new_n_indices = 0;

    SET_ARRAY_SIZE( new_points, 0, new_n_points, DEFAULT_CHUNK_SIZE );

    for_less( i, 0, new_n_points )
        new_points[i] = polygons->points[i];

    initialize_hash_table( &midpoint_table, 2, polygons->n_points, 0.75, 0.4 );

    initialize_progress_report( &progress, TRUE, polygons->n_items,
                                "Subdividing Polygon" );

    for_less( i, 0, polygons->n_items )
    {
        subdivide_polygon( polygons, i,
                           &new_n_points, &new_points,
                           &new_n_polygons, &new_end_indices,
                           &new_n_indices, &new_indices,
                           &midpoint_table );
        update_progress_report( &progress, i+1 );
    }

    terminate_progress_report( &progress );

    initialize_hash_pointer( &hash_pointer );

    while( get_next_hash_entry(&midpoint_table,&hash_pointer,
                               (void **) &index_ptr ) )
    {
        FREE( index_ptr );
    }

    delete_hash_table( &midpoint_table );

    delete_polygons( polygons );

    ALLOC( polygons->colours, 1 );
    polygons->colour_flag = ONE_COLOUR;
    polygons->colours[0] = save_colour;

    ALLOC( polygons->normals, new_n_points );

    polygons->n_points = new_n_points;
    polygons->points = new_points;
    polygons->n_items = new_n_polygons;
    polygons->end_indices = new_end_indices;
    polygons->indices = new_indices;
}

private  void  subdivide_polygon(
    polygons_struct   *polygons,
    int               poly,
    int               *new_n_points,
    Point             *new_points[],
    int               *new_n_polygons,
    int               *new_end_indices[],
    int               *new_n_indices,
    int               *new_indices[],
    hash_table_struct *midpoint_table )
{
    int     edge, size, edge_indices[2], point_indices[4], *midpoint_ptr;
    int     midpoint_indices[4], centre_point_index, p1, p2;
    void    *data_ptr;
    Point   midpoint;

    size = GET_OBJECT_SIZE( *polygons, poly );

    if( size < 3 || size > 4 )
    {
        print( "Polygon size %d in subdivide polygons\n", size );
        return;
    }

    for_less( edge, 0, size )
    {
        point_indices[edge] = polygons->indices[
                      POINT_INDEX(polygons->end_indices,poly,edge)];
    }

    for_less( edge, 0, size )
    {
        p1 = point_indices[edge];
        p2 = point_indices[(edge+1)%size];
        edge_indices[0] = MIN( p1, p2 );
        edge_indices[1] = MAX( p1, p2 );

        if( lookup_in_hash_table( midpoint_table, edge_indices, &data_ptr ) )
        {
            midpoint_indices[edge] = * ((int *) data_ptr);
            if( midpoint_indices[edge] < 0 ||
                midpoint_indices[edge] < 0 >= *new_n_indices )
            {
                HANDLE_INTERNAL_ERROR( "midpoint_indices" );
            }
        }
        else
        {
            midpoint_indices[edge] = *new_n_points;
            INTERPOLATE_POINTS( midpoint, polygons->points[edge_indices[0]],
                                polygons->points[edge_indices[1]], 0.5 );
            ADD_ELEMENT_TO_ARRAY( *new_points, *new_n_points,
                                  midpoint, DEFAULT_CHUNK_SIZE );
            ALLOC( midpoint_ptr, 1 );
            midpoint_ptr[0] = midpoint_indices[edge];
            insert_in_hash_table( midpoint_table, edge_indices,
                                  (void *) midpoint_ptr );
        }
    }

    if( size == 3 )
    {

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );
    }
    else
    {
        centre_point_index = *new_n_points;

        ADD_POINTS( midpoint, polygons->points[point_indices[0]],
                              polygons->points[point_indices[1]] );
        ADD_POINTS( midpoint, midpoint, polygons->points[point_indices[2]] );
        ADD_POINTS( midpoint, midpoint, polygons->points[point_indices[3]] );
        SCALE_POINT( midpoint, midpoint, 0.25 );

        ADD_ELEMENT_TO_ARRAY( *new_points, *new_n_points,
                              midpoint, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              point_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_indices, *new_n_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( *new_end_indices, *new_n_polygons,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );
    }
}
