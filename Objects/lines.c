
#include  <def_objects.h>
#include  <def_arrays.h>

public  Status  initialize_lines( lines, col )
    lines_struct    *lines;
    Colour          col;
{
    Status   status;

    ALLOC( status, lines->colours, 1 );

    lines->colour_flag = ONE_COLOUR;
    lines->colours[0] = col;

    lines->line_thickness = 1;
    lines->n_points = 0;
    lines->n_items = 0;

    return( status );
}

public  Status  delete_lines( lines )
    lines_struct    *lines;
{
    Status   status;

    FREE( status, lines->colours );

    if( lines->n_points > 0 )
        FREE( status, lines->points );

    if( status == OK && lines->n_items > 0 )
        FREE( status, lines->end_indices );

    if( status == OK && lines->n_items > 0 )
        FREE( status, lines->indices );

    return( status );
}

public  Status  start_new_line( lines )
    lines_struct   *lines;
{
    Status   status;
    int      n_indices;

    n_indices = NUMBER_INDICES( *lines );

    ADD_ELEMENT_TO_ARRAY( status, lines->n_items, lines->end_indices,
                          n_indices, DEFAULT_CHUNK_SIZE );

    return( status );
}

public  Status  add_point_to_line( lines, point )
    lines_struct   *lines;
    Point          *point;
{
    Status   status;

    if( lines->n_items == 0 )
        status = start_new_line( lines );

    ADD_ELEMENT_TO_ARRAY( status, lines->n_points, lines->points,
                          *point, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        ADD_ELEMENT_TO_ARRAY( status, lines->end_indices[lines->n_items-1],
                              lines->indices,
                              lines->n_points-1, DEFAULT_CHUNK_SIZE );
    }

    return( status );
}
