#include  <def_mni.h>
#include  <def_deform.h>
#include  <def_module.h>

private  void  get_surface_colour(
    Real    curvature,
    Real    max_curvature,
    Colour  *colour );

public  void  create_sphere_from_polygons(
    polygons_struct   *polygons,
    polygons_struct   *sphere,
    Point             *centre,
    Real              x_radius,
    Real              y_radius,
    Real              z_radius,
    Real              max_curvature,
    Boolean           offset_flag,
    Real              offset_factor )
{
    int           n_up, size, point_index, vertex_index, poly;
    Real          curvature, base_length;
    Smallest_int  *point_done;
    Point         centroid;
    Vector        normal;

    if( get_tessellation_of_polygons_sphere( polygons, &n_up ) )
    {
        check_polygons_neighbours_computed( polygons );

        create_polygons_sphere( centre, x_radius, y_radius,
                                z_radius, n_up, 2 * n_up, FALSE, sphere );

        ALLOC( point_done, polygons->n_points );

        sphere->colour_flag = PER_VERTEX_COLOURS;

        REALLOC( sphere->colours, sphere->n_points );

        for_less( point_index, 0, polygons->n_points )
            point_done[point_index] = FALSE;

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

                    compute_polygon_point_centroid( polygons, poly,
                              vertex_index, point_index, &centroid,
                              &normal, &base_length, &curvature );

                    if( offset_flag && ABS( curvature ) < max_curvature )
                        curvature = 0.0;

                    get_surface_colour( curvature, max_curvature,
                                        &sphere->colours[point_index] );
                }
            }
        }
    }
}

private  void  get_surface_colour(
    Real    curvature,
    Real    max_curvature,
    Colour  *colour )
{
    Real   ratio;

    if( curvature < -max_curvature )
        curvature = -max_curvature;
    else if( curvature > max_curvature )
        curvature = max_curvature;

    ratio = (curvature + max_curvature) / max_curvature / 2.0;

    *colour = make_Colour_0_1( ratio, ratio, ratio );
}
