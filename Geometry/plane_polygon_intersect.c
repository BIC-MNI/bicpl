#include  <internal_volume_io.h>
#include  <geom.h>

public  void   intersect_planes_with_polygons(
    polygons_struct   *polygons,
    Point             *plane_origin,
    Vector            *plane_normal,
    lines_struct      *lines )
{
    int     n_points;
    int     poly, edge, size;
    int     point_index1, point_index2;
    Vector  v1, v2;
    Real    t1, t2, ratio;
    Point   points[2];

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

            SUB_POINTS( v1, polygons->points[point_index1], *plane_origin );
            SUB_POINTS( v2, polygons->points[point_index2], *plane_origin );

            t1 = DOT_VECTORS( v1, *plane_normal );
            t2 = DOT_VECTORS( v2, *plane_normal );

            /*--- check if it intersects the line segment [p1 .. p2)  */

            if( t1 >= 0.0 && t2 < 0.0 || t1 <= 0.0 && t2 > 0.0 )
            {
                ratio = t1 / (t1 - t2);

                INTERPOLATE_POINTS( points[n_points],
                                    polygons->points[point_index1],
                                    polygons->points[point_index2], ratio );
                ++n_points;
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
