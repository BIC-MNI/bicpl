#include  <def_objects.h>
#include  <def_colours.h>

public  Status  subdivide_lines( lines )
    lines_struct  *lines;
{
    Status             status;
    int                i, new_n_points, new_n_indices, new_n_lines;
    int                *new_indices, *new_end_indices;
    Point              *new_points;
    void               subdivide_line();

    new_n_points = lines->n_points;
    new_n_lines = 0;
    new_n_indices = 0;

    SET_ARRAY_SIZE( status, new_points, 0, new_n_points, DEFAULT_CHUNK_SIZE );

    if( status == OK )
    {
        for_less( i, 0, new_n_points )
            new_points[i] = lines->points[i];
    }

    if( status == OK )
    {
        for_less( i, 0, lines->n_items )
        {
            subdivide_line( lines, i, &new_n_points, &new_points,
                            &new_n_lines, &new_end_indices,
                            &new_n_indices, &new_indices );
        }
    }

    if( status == OK )
        FREE( status, lines->points );

    if( status == OK )
        FREE( status, lines->end_indices );

    if( status == OK )
        FREE( status, lines->indices );

    if( status == OK )
    {
        lines->n_points = new_n_points;
        lines->points = new_points;
        lines->n_items = new_n_lines;
        lines->end_indices = new_end_indices;
        lines->indices = new_indices;

        if( lines->colour_flag != ONE_COLOUR )
        {
            REALLOC( status, lines->colours, 1);
            lines->colours[0] = WHITE;
            lines->colour_flag = ONE_COLOUR;
        }
    }

    return( status );
}

private  void  subdivide_line( lines, l, new_n_points, new_points,
                               new_n_lines, new_end_indices,
                               new_n_indices, new_indices )
    lines_struct      *lines;
    int               l;
    int               *new_n_points;
    Point             *new_points[];
    int               *new_n_lines;
    int               *new_end_indices[];
    int               *new_n_indices;
    int               *new_indices[];
{
    Status  status;
    int     edge, size, p1, p2, midpoint_index;
    Point   midpoint;

    size = GET_OBJECT_SIZE( *lines, l );

    for_less( edge, 0, size-1 )
    {
        p1 = lines->indices[POINT_INDEX(lines->end_indices,l,edge)];
        p2 = lines->indices[POINT_INDEX(lines->end_indices,l,edge+1)];
        midpoint_index = *new_n_points;
        INTERPOLATE_POINTS( midpoint, lines->points[p1],
                            lines->points[p2], 0.5 );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_points, *new_points,
                              midpoint, DEFAULT_CHUNK_SIZE );

        if( edge == 0 )
        {
            ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                                  p1, DEFAULT_CHUNK_SIZE );
        }

        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              midpoint_index, DEFAULT_CHUNK_SIZE );
        ADD_ELEMENT_TO_ARRAY( status, *new_n_indices, *new_indices,
                              p2, DEFAULT_CHUNK_SIZE );
    }

    ADD_ELEMENT_TO_ARRAY( status, *new_n_lines, *new_end_indices,
                          *new_n_indices, DEFAULT_CHUNK_SIZE );
}
