
#include  <def_mni.h>

public  void  initialize_lines(
    lines_struct    *lines,
    Colour          col )
{
    ALLOC( lines->colours, 1 );

    lines->colour_flag = ONE_COLOUR;
    lines->colours[0] = col;

    lines->line_thickness = 1;
    lines->n_points = 0;
    lines->n_items = 0;
}

public  void  delete_lines( lines_struct *lines )
{
    free_colours( lines->colour_flag, lines->colours, lines->n_points,
                  lines->n_items );

    if( lines->n_points > 0 )
        FREE( lines->points );

    if( lines->n_items > 0 )
        FREE( lines->end_indices );

    if( lines->n_items > 0 )
        FREE( lines->indices );
}

public  void  start_new_line( lines_struct *lines )
{
    int      n_indices;

    n_indices = NUMBER_INDICES( *lines );

    ADD_ELEMENT_TO_ARRAY( lines->end_indices, lines->n_items,
                          n_indices, DEFAULT_CHUNK_SIZE );
}

public  void  add_point_to_line(
    lines_struct   *lines,
    Point          *point )
{
    if( lines->n_items == 0 )
        start_new_line( lines );

    ADD_ELEMENT_TO_ARRAY( lines->indices, lines->end_indices[lines->n_items-1],
                          lines->n_points, DEFAULT_CHUNK_SIZE );

    ADD_ELEMENT_TO_ARRAY( lines->points, lines->n_points,
                          *point, DEFAULT_CHUNK_SIZE );
}
