#include  <internal_volume_io.h>
#include  <data_structures.h>
#include  <geom.h>

public  void  delete_the_bintree(
    bintree_struct  **bintree )
{
    if( *bintree != (bintree_struct *) 0 )
    {
        delete_bintree( *bintree );

        FREE( *bintree );
        *bintree = (bintree_struct *) 0;
    }
}

private  void  check_install_bintree_delete_function()
{
    static  BOOLEAN  first = TRUE;

    if( first )
    {
        first = FALSE;
        set_bintree_delete_function( delete_the_bintree );
    }
}

public  void  *allocate_bintree()
{
    bintree_struct   *bintree;

    ALLOC( bintree, 1 );

    return( (void *) bintree );
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

    check_install_bintree_delete_function();

    lines->bintree = allocate_bintree();

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

public  void  create_polygons_bintree(
    polygons_struct   *polygons,
    int               max_nodes )
{
    int              poly, size;
    range_struct     *bound_vols;
    Point            min_range, max_range;
    Point            points[MAX_POINTS_PER_POLYGON];

    check_install_bintree_delete_function();

    polygons->bintree = allocate_bintree();

    ALLOC( bound_vols, polygons->n_items );

    for_less( poly, 0, polygons->n_items )
    {
        size = get_polygon_points( polygons, poly, points );

        get_range_points( size, points, &min_range, &max_range );
        bound_vols[poly].limits[X][0] = Point_x(min_range);
        bound_vols[poly].limits[Y][0] = Point_y(min_range);
        bound_vols[poly].limits[Z][0] = Point_z(min_range);
        bound_vols[poly].limits[X][1] = Point_x(max_range);
        bound_vols[poly].limits[Y][1] = Point_y(max_range);
        bound_vols[poly].limits[Z][1] = Point_z(max_range);
    }

    create_object_bintree( polygons->n_items, bound_vols,
                           polygons->bintree, max_nodes );

    FREE( bound_vols );
}

public  void  create_quadmesh_bintree(
    quadmesh_struct   *quadmesh,
    int               max_nodes )
{
    int              i, j, m, n, obj_index;
    range_struct     *bound_vols;
    Point            min_range, max_range;
    Point            points[4];

    check_install_bintree_delete_function();

    quadmesh->bintree = allocate_bintree();

    get_quadmesh_n_objects( quadmesh, &m, &n );

    ALLOC( bound_vols, m * n );

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            obj_index = IJ( i, j, n );
            get_quadmesh_patch( quadmesh, i, j, points );

            get_range_points( 4, points, &min_range, &max_range );

            bound_vols[obj_index].limits[X][0] = Point_x(min_range);
            bound_vols[obj_index].limits[Y][0] = Point_y(min_range);
            bound_vols[obj_index].limits[Z][0] = Point_z(min_range);
            bound_vols[obj_index].limits[X][1] = Point_x(max_range);
            bound_vols[obj_index].limits[Y][1] = Point_y(max_range);
            bound_vols[obj_index].limits[Z][1] = Point_z(max_range);
        }
    }

    create_object_bintree( m * n, bound_vols,
                           quadmesh->bintree, max_nodes );

    FREE( bound_vols );
}
