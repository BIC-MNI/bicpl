
#include  <internal_volume_io.h>
#include  <objects.h>
#include  <geom.h>

public  void  initialize_lines(
    lines_struct    *lines,
    Colour          col )
{
    ALLOC( lines->colours, 1 );

    lines->colour_flag = ONE_COLOUR;
    lines->colours[0] = col;

    lines->line_thickness = 1.0;
    lines->n_points = 0;
    lines->n_items = 0;

    lines->bintree = (bintree_struct_ptr) NULL;
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

    delete_bintree_if_any( &lines->bintree );
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

public  void  get_line_segment_index(
    lines_struct  *lines,
    int           obj_index,
    int           *line,
    int           *seg )
{
    *line = 0;
    while( obj_index > lines->end_indices[*line] - *line - 2 )
        ++(*line);

    if( *line == 0 )
        *seg = obj_index;
    else
        *seg = obj_index - (lines->end_indices[(*line)-1] - *line);
}

static  void  (*bintree_delete_function) ( bintree_struct_ptr* ) = NULL;

public  void  set_bintree_delete_function(
    void  (*func)( bintree_struct_ptr * ) )
{
    bintree_delete_function = func;
}

public  void  delete_bintree_if_any(
    bintree_struct_ptr   *bintree )
{
    if( bintree_delete_function != NULL )
        (*bintree_delete_function) ( bintree );
}

public  Real  get_lines_length(
    lines_struct  *lines )
{
    Real  len;
    int   line, i, p0, p1, size;

    len = 0.0;

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        for_less( i, 0, size )
        {
            p0 = lines->indices[POINT_INDEX(lines->end_indices,line,i)];
            p1 = lines->indices[POINT_INDEX(lines->end_indices,line,i+1)];
            len += distance_between_points( &lines->points[p0],
                                            &lines->points[p1] );
        }
    }

    return( len );
}
