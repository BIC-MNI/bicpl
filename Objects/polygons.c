
#include  <def_mni.h>

public  Status  initialize_polygons(
    polygons_struct   *polygons,
    Colour            col,
    Surfprop          *spr )
{
    Status   status;

    ALLOC( status, polygons->colours, 1 );

    if( status == OK )
        polygons->colour_flag = ONE_COLOUR;

    polygons->colours[0] = col;
    polygons->surfprop = *spr;

    polygons->n_points = 0;
    polygons->points = (Point *) 0;
    polygons->normals = (Vector *) 0;

    polygons->n_items = 0;
    polygons->end_indices = (int *) 0;
    polygons->indices = (int *) 0;

    polygons->visibilities = (Smallest_int *) 0;

    return( status );
}

public  Status  delete_polygons( polygons_struct *polygons )
{
    Status   status;

    FREE( status, polygons->colours );

    if( polygons->n_points > 0 )
        FREE( status, polygons->points );

    if( status == OK && polygons->n_points > 0 &&
        polygons->normals != (Vector *) 0 )
        FREE( status, polygons->normals );

    if( status == OK && polygons->n_items > 0 )
        FREE( status, polygons->indices );

    if( status == OK && polygons->n_items > 0 )
        FREE( status, polygons->end_indices );

    if( status == OK && polygons->visibilities != (Smallest_int *) 0 )
        FREE( status, polygons->visibilities );

    return( status );
}

public  Status  start_new_polygon( polygons_struct *polygons )
{
    Status   status;
    int      n_indices;

    n_indices = NUMBER_INDICES( *polygons );

    ADD_ELEMENT_TO_ARRAY( status, polygons->n_items, polygons->end_indices,
                          n_indices, DEFAULT_CHUNK_SIZE );

    return( status );
}

public  Status  add_point_to_polygon(
    polygons_struct   *polygons,
    Point             *point,
    Vector            *normal )
{
    int      n_points;
    Status   status;

    if( polygons->n_items == 0 )
        status = start_new_polygon( polygons );

    if( polygons->n_points > 1 )
    {
        if( normal != (Vector *) 0 && polygons->normals == (Vector *) 0 ||
            normal == (Vector *) 0 && polygons->normals != (Vector *) 0 )
        {
            print(
              "Error: be consistent with normals in add_point_to_polygon.\n" );
        }
    }

    if( normal != (Vector *) 0 )
    {
        n_points = polygons->n_points;
        ADD_ELEMENT_TO_ARRAY( status, n_points, polygons->normals,
                              *normal, DEFAULT_CHUNK_SIZE );
    }

    ADD_ELEMENT_TO_ARRAY( status, polygons->n_points, polygons->points,
                          *point, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        ADD_ELEMENT_TO_ARRAY( status,
                              polygons->end_indices[polygons->n_items-1],
                              polygons->indices,
                              polygons->n_points-1, DEFAULT_CHUNK_SIZE );
    }

    return( status );
}
