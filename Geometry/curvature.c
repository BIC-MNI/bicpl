#include  <internal_volume_io.h>
#include  <geom.h>

public  void  get_polygon_vertex_curvatures(
    polygons_struct   *polygons,
    Real              smoothing_distance,
    Real              low_threshold,
    Real              curvatures[] )
{
    int              size, point_index, vertex_index, poly;
    Real             curvature, base_length;
    Smallest_int     *point_done;
    Point            centroid;
    Vector           normal;
    progress_struct  progress;

    check_polygons_neighbours_computed( polygons );

    ALLOC( point_done, polygons->n_points );

    for_less( point_index, 0, polygons->n_points )
        point_done[point_index] = FALSE;

    initialize_progress_report( &progress, FALSE, polygons->n_items,
                                "Computing Curvatures" );

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( vertex_index, 0, size )
        {
            point_index = polygons->indices[
                POINT_INDEX(polygons->end_indices,poly,vertex_index)];

            if( !point_done[point_index] )
            {
                point_done[point_index] = TRUE;

                if( smoothing_distance <= 0.0 )
                {
                    compute_polygon_point_centroid( polygons, poly,
                          vertex_index, point_index, &centroid,
                          &normal, &base_length, &curvature );
                }
                else
                {
                    curvature = get_smooth_surface_curvature( polygons,
                                       poly, vertex_index, smoothing_distance );
                }

                if( ABS( curvature ) < low_threshold )
                    curvature = 0.0;

                curvatures[point_index] = curvature;
            }
        }

        update_progress_report( &progress, poly + 1 );
    }

    terminate_progress_report( &progress );
}
