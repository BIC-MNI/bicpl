#include  <bicpl.h>

#define  BINTREE_FACTOR  0.5

int  main(
    int   argc,
    char  *argv[] )
{
    STRING               src_polygons_filename;
    File_formats         format;
    int                  n_src_objects, n_rays, ray, dim, obj_index;
    VIO_Real                 ratio, dist;
    object_struct        **src_objects;
    polygons_struct      *polygons;
    Point                min_range, max_range, origin;
    Vector               direction;
    VIO_BOOL              n_intersects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &src_polygons_filename ) ||
        !get_real_argument( 0.0, &ratio ) )
    {
        return( 1 );
    }

    (void) get_int_argument( 0, &n_rays );

    if( input_graphics_file( src_polygons_filename,
                             &format, &n_src_objects, &src_objects ) != VIO_OK ||
        n_src_objects < 1 || get_object_type( src_objects[0] ) != POLYGONS  )
        return( 1 );

    polygons = get_polygons_ptr( src_objects[0] );

    print( "File input: %d items\n", polygons->n_items );

    create_polygons_bintree( polygons,
                             VIO_ROUND( (VIO_Real) polygons->n_items * ratio ) );

    get_range_points( polygons->n_points, polygons->points, &min_range,
                      &max_range );

    set_random_seed( 514485223 );

    n_intersects = 0;
    for_less( ray, 0, n_rays )
    {
        for_less( dim, 0, N_DIMENSIONS )
        {
            Point_coord( origin, dim ) = (Point_coord_type)
                 ((RPoint_coord(min_range,dim) +
                  RPoint_coord(max_range,dim))/ 2.0 +
                  2.0 * get_random_0_to_1() *
                  (RPoint_coord(max_range,dim) - RPoint_coord(min_range,dim)));
            Vector_coord( direction, dim ) = (Point_coord_type)
                                           (2.0 * get_random_0_to_1() - 1.0);
        }

        NORMALIZE_VECTOR( direction, direction );

        if( intersect_ray_with_bintree( &origin, &direction,
                                           polygons->bintree,
                                           src_objects[0], &obj_index,
                                           &dist, NULL ) )
            ++n_intersects;
    }

    print( "%d/%d\n", n_intersects, n_rays );

    return( 0 );
}
