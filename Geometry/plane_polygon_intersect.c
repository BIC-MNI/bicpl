#include  <internal_volume_io.h>
#include  <geom.h>

public  void   intersect_planes_with_polygons(
    polygons_struct   *polygons,
    Point             *plane_origin,
    Vector            *plane_normal,
    lines_struct      *lines )
{
    int     n_points, p;
    int     poly, edge, size;
    int     point_index1, point_index2;
    Point   points[2], int_point;

    initialize_lines( lines, WHITE );

    for_less( poly, 0, polygons->n_items )
    {
        n_points = 0;

        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( edge, 0, size )
        {
            point_index1 = polygons->indices[
                     POINT_INDEX(polygons->end_indices,poly,edge)];
            point_index2 = polygons->indices[
                     POINT_INDEX(polygons->end_indices,poly,(edge+1)%size)];

            if( line_segment_intersects_plane( &polygons->points[point_index1],
                                               &polygons->points[point_index2],
                                               plane_origin, plane_normal,
                                               &int_point ) )
            {
                for_less( p, 0, n_points )
                {
                    if( EQUAL_POINTS( int_point, points[p] ) )
                        break;
                }

                if( p >= n_points )
                {
                    if( n_points == 2 )
                    {
                        n_points = 0;
                        break;
                    }

                    points[n_points] = int_point;
                    ++n_points;
                }
            }
        }

        if( n_points == 2 && !EQUAL_POINTS(points[0],points[1]) )
        {
            start_new_line( lines );
            add_point_to_line( lines, &points[0] );
            add_point_to_line( lines, &points[1] );
        }
    }
}
