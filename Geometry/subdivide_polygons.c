#include  <def_objects.h>
#include  <def_progress.h>
#include  <def_hash.h>

public  Status  subdivide_polygons( polygons )
    polygons_struct  *polygons;
{
    Status             status;
    Status             delete_polygons_bintree();
    int                i, new_n_points, new_n_indices, new_n_polygons;
    int                *new_indices, *new_end_indices;
    int                *index_ptr;
    Point              *new_points;
    Point              dummy;
    hash_table_struct  midpoint_table;
    hash_table_pointer hash_pointer;
    progress_struct    progress;
    Status             create_polygons_sphere();
    void               subdivide_polygon();

    if( is_this_sphere_topology( polygons ) )
    {
        (void) printf( "Subdividing assuming sphere topology.\n" );
        status = create_polygons_sphere( &dummy, 0.0, 0.0, 0.0,
                                         0, 0, TRUE, polygons );

        return( status );
    }

    new_n_points = polygons->n_points;
    new_n_polygons = 0;
    new_n_indices = 0;

    SET_ARRAY_SIZE( status, new_points, 0, new_n_points, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        for_less( i, 0, new_n_points )
            new_points[i] = polygons->points[i];

        status = initialize_hash_table( &midpoint_table, 2, polygons->n_points,
                                        0.5, 0.25 );
    }

    if( status == OK )
    {
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
    }

    if( status == OK )
    {
        initialize_hash_pointer( &hash_pointer );

        while( get_next_hash_entry(&midpoint_table,&hash_pointer,
                      (void **) &index_ptr ) )
        {
            FREE( status, index_ptr );
        }
    }

    if( status == OK )
        status = delete_hash_table( &midpoint_table );

    if( status == OK )
        FREE( status, polygons->points );

    if( status == OK )
        FREE( status, polygons->end_indices );

    if( status == OK )
        FREE( status, polygons->indices );

    if( status == OK )
        REALLOC( status, polygons->normals, new_n_points );

    if( status == OK )
    {
        polygons->n_points = new_n_points;
        polygons->points = new_points;
        polygons->n_items = new_n_polygons;
        polygons->end_indices = new_end_indices;
        polygons->indices = new_indices;

        status = delete_polygons_bintree( polygons );
    }

    if( status == OK && polygons->visibilities != (Smallest_int *) 0 )
        FREE( status, polygons->visibilities );

    if( status == OK && polygons->neighbours != (int *) 0 )
        FREE( status, polygons->neighbours );

    return( status );
}

private  void  subdivide_polygon( polygons, poly,
                               new_n_points, new_points,
                               new_n_polygons, new_end_indices,
                               new_n_indices, new_indices, midpoint_table )
    polygons_struct   *polygons;
    int               poly;
    int               *new_n_points;
    Point             *new_points[];
    int               *new_n_polygons;
    int               *new_end_indices[];
    int               *new_n_indices;
    int               *new_indices[];
    hash_table_struct *midpoint_table;
{
    Status  status;
    int     edge, size, edge_indices[2], point_indices[4], *midpoint_ptr;
    int     midpoint_indices[4], centre_point_index, p1, p2;
    void    *data_ptr;
    Point   midpoint;

    size = GET_OBJECT_SIZE( *polygons, poly );

    if( size < 3 || size > 4 )
    {
        (void) printf( "Polygon size %d in subdivide polygons\n", size );
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
                (void) printf( "Shit\n" );
                abort();
            }
        }
        else
        {
            midpoint_indices[edge] = *new_n_points;
            INTERPOLATE_POINTS( midpoint, polygons->points[edge_indices[0]],
                                polygons->points[edge_indices[1]], 0.5 );
            ADD_ELEMENT_TO_ARRAY( status, *new_n_points, *new_points,
                                  midpoint, DEFAULT_CHUNK_SIZE );
            ALLOC( status, midpoint_ptr, 1 );
            midpoint_ptr[0] = midpoint_indices[edge];
            status = insert_in_hash_table( midpoint_table, edge_indices,
                                           (void *) midpoint_ptr );
        }
    }

    if( size == 3 )
    {

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
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

        ADD_ELEMENT_TO_ARRAY( status, *new_n_points, *new_points,
                              midpoint, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[0], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[3], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              centre_point_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[1], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              point_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_indices[2], DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_polygons, *new_end_indices,
                              *new_n_indices, DEFAULT_CHUNK_SIZE );
    }
}
