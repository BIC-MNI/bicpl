
#include  <def_mni.h>

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

    polygons->n_items = 0;
    polygons->end_indices = (int *) 0;
    polygons->indices = (int *) 0;

    polygons->visibilities = (Smallest_int *) 0;
    polygons->neighbours = (int *) 0;
    polygons->bintree = (bintree_struct *) 0;
}

public  void  delete_polygons( polygons_struct *polygons )
{
    FREE( polygons->colours );

    if( polygons->n_points > 0 )
        FREE( polygons->points );

    if( polygons->n_points > 0 &&
        polygons->normals != (Vector *) 0 )
        FREE( polygons->normals );

    if( polygons->n_items > 0 )
        FREE( polygons->indices );

    if( polygons->n_items > 0 )
        FREE( polygons->end_indices );

    if( polygons->visibilities != (Smallest_int *) 0 )
        FREE( polygons->visibilities );

    if( polygons->neighbours != (int *) 0 )
        FREE( polygons->neighbours );

    if( polygons->bintree != (bintree_struct *) NULL )
        delete_polygons_bintree( polygons );

    polygons->visibilities = (Smallest_int *) 0;
    polygons->neighbours = (int *) 0;
    polygons->bintree = (bintree_struct *) 0;
}

public  Status  create_polygons_bintree(
    polygons_struct   *polygons,
    int               max_nodes )
{
    ALLOC( polygons->bintree, 1 );

    create_object_bintree( polygons->n_points, polygons->points,
                           polygons->n_items, polygons->end_indices,
                           polygons->indices, polygons->bintree, max_nodes );
}

public  void  delete_polygons_bintree(
    polygons_struct   *polygons )
{
    if( polygons->bintree != (bintree_struct *) 0 )
    {
        delete_bintree( polygons->bintree );

        FREE( polygons->bintree );
        polygons->bintree = (bintree_struct *) 0;
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
