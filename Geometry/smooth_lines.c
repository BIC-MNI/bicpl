
#include  <def_mni.h>

public  void  smooth_lines(
    lines_struct  *lines,
    Real          smooth_length )
{
    int       *new_ids, n_points, n_items, n_indices, *indices, *end_indices;
    int       l, p, point_index, size;
    Boolean   keep_point;
    Point     prev;
    Real      dist_to_prev;
    Point     *points;
    Colour    colour;

    n_points = 0;
    n_items = 0;
    n_indices = 0;

    ALLOC( new_ids, lines->n_points );

    for_less( l, 0, lines->n_points )
        new_ids[l] = -1;

    for_less( l, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, l );

        for_less( p, 0, size )
        {
            point_index = lines->indices[
                             POINT_INDEX(lines->end_indices,l,p)];

            if( p == 0 || p == size-1 )
            {
                keep_point = TRUE;
            }
            else
            {
                dist_to_prev = distance_between_points(&prev,
                                         &lines->points[point_index]);

                keep_point = (dist_to_prev >= smooth_length);
            }

            if( keep_point )
            {
                prev = lines->points[point_index];

                if( new_ids[point_index] < 0 )
                {
                    new_ids[point_index] = n_points;

                    ADD_ELEMENT_TO_ARRAY( points, n_points,
                              lines->points[point_index], DEFAULT_CHUNK_SIZE );
                }

                ADD_ELEMENT_TO_ARRAY( indices, n_indices,
                                     new_ids[point_index], DEFAULT_CHUNK_SIZE );
            }
        }

        ADD_ELEMENT_TO_ARRAY( end_indices, n_items,
                              n_indices, DEFAULT_CHUNK_SIZE );
    }

    colour = lines->colours[0];
    delete_lines( lines );

    lines->colour_flag = ONE_COLOUR;
    ALLOC( lines->colours, 1 );
    lines->colours[0] = colour;
    lines->n_points = n_points;
    lines->points = points;
    lines->n_items = n_items;
    lines->indices = indices;
    lines->end_indices = end_indices;

    FREE( new_ids );
}

public  void  create_line_spline(
    lines_struct  *lines,
    int           n_curve_segments,
    lines_struct  *new_lines )
{
    int       l, p, point_index1, point_index2, pt_index, line_size, segment;
    Boolean   wrap_around;
    Point     points[4], point;
    Real      u;

    ALLOC( new_lines->colours, 1 );

    new_lines->colour_flag = ONE_COLOUR;
    new_lines->colours[0] = lines->colours[0];
    new_lines->line_thickness = lines->line_thickness;

    new_lines->n_points = 0;
    new_lines->n_items = 0;

    for_less( l, 0, lines->n_items )
    {
        line_size = GET_OBJECT_SIZE( *lines, l );

        point_index1 = lines->indices[POINT_INDEX(lines->end_indices,l,0)];
        point_index2 = lines->indices[
                         POINT_INDEX(lines->end_indices,l,line_size-1)];

        wrap_around = (point_index1 == point_index2);

        begin_adding_points_to_line( new_lines );

        add_point_to_line( new_lines, &lines->points[point_index1] );

        for_less( p, 0, line_size-1 )
        {
            if( p == 0 )
            {
                if( wrap_around && line_size - 2 >= 0 )
                    pt_index = line_size-2;
                else
                    pt_index = 0;
            }
            else
                pt_index = p - 1;

            points[0] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,l,pt_index)]];

            points[1] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,l,p)]];

            points[2] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,l,p+1)]];

            if( p == line_size-2 )
            {
                if( wrap_around && 1 < line_size )
                    pt_index = 1;
                else
                    pt_index = line_size-1;
            }
            else
                pt_index = p + 2;

            points[3] = lines->points[lines->indices[
                          POINT_INDEX(lines->end_indices,l,pt_index)]];

            for_inclusive( segment, 1, n_curve_segments )
            {
                u = (Real) segment / (Real) n_curve_segments;

                Point_x(point) = CUBIC_UNIVAR( Point_x(points[0]),
                                               Point_x(points[1]),
                                               Point_x(points[2]),
                                               Point_x(points[3]), u );

                Point_y(point) = CUBIC_UNIVAR( Point_y(points[0]),
                                               Point_y(points[1]),
                                               Point_y(points[2]),
                                               Point_y(points[3]), u );

                Point_z(point) = CUBIC_UNIVAR( Point_z(points[0]),
                                               Point_z(points[1]),
                                               Point_z(points[2]),
                                               Point_z(points[3]), u );

                add_point_to_line( new_lines, &point );
            }
        }
    }
}
