
#include  <mni.h>

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

    lines->bintree = (bintree_struct *) NULL;
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

    if( lines->bintree != (bintree_struct *) NULL )
        delete_lines_bintree( lines );
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

public  void  create_lines_bintree(
    lines_struct   *lines,
    int            max_nodes )
{
    Real             radius;
    int              line, size, n_segments, seg, object_id;
    range_struct     *bound_vols;
    Point            min_range, max_range;
    Point            points[2];

    ALLOC( lines->bintree, 1 );

    n_segments = 0;
    for_less( line, 0, lines->n_items )
        n_segments += GET_OBJECT_SIZE( *lines, line ) - 1;

    ALLOC( bound_vols, n_segments );

    radius = (Real) lines->line_thickness;

    object_id = 0;
    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        for_less( seg, 0, size - 1 )
        {
            points[0] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,line,seg)]];
            points[1] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,line,seg+1)]];

            get_range_points( 2, points, &min_range, &max_range );
            bound_vols[object_id].limits[X][0] = Point_x(min_range) - radius;
            bound_vols[object_id].limits[Y][0] = Point_y(min_range) - radius;
            bound_vols[object_id].limits[Z][0] = Point_z(min_range) - radius;
            bound_vols[object_id].limits[X][1] = Point_x(max_range) + radius;
            bound_vols[object_id].limits[Y][1] = Point_y(max_range) + radius;
            bound_vols[object_id].limits[Z][1] = Point_z(max_range) + radius;
            ++object_id;
        }
    }

    create_object_bintree( n_segments, bound_vols,
                           lines->bintree, max_nodes );

    FREE( bound_vols );
}

public  void  delete_lines_bintree(
    lines_struct   *lines )
{
    if( lines->bintree != (bintree_struct *) 0 )
    {
        delete_bintree( lines->bintree );

        FREE( lines->bintree );
        lines->bintree = (bintree_struct *) 0;
    }
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
        *seg = obj_index - lines->end_indices[(*line)-1] - *line;
}
