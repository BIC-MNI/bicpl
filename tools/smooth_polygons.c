#include  <bicpl.h>

static  void  usage(
    VIO_STR   executable )
{
    VIO_STR  usage_str = "\n\
Usage: %s  input.obj  [output.obj] [distance] [ratio] [threshold] [normal_ratio]\n\
\n\
     Smooths the polygon, placing output in input.obj, or if\n\
     specified, output.obj.\n\n";

    fprintf(stderr, usage_str, executable );
}

int  main(
    int    argc,
    char   *argv[] )
{
    VIO_Status           status;
    VIO_STR              src_filename, dest_filename;
    int                  i, n_iters, n_objects;
    VIO_Real             threshold;
    VIO_File_formats     format;
    object_struct        **object_list;
    polygons_struct      *polygons;
    VIO_Real             Max_smoothing_distance;
    VIO_Real             Smoothing_ratio;
    VIO_Real             Smoothing_threshold;
    VIO_Real             Smoothing_normal_ratio;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    (void)  get_string_argument( src_filename, &dest_filename );
    (void)  get_real_argument( 0.5, &Max_smoothing_distance );
    (void)  get_real_argument( 0.5, &Smoothing_ratio );
    (void)  get_real_argument( 0.01, &Smoothing_threshold );
    (void)  get_real_argument( 0.0, &Smoothing_normal_ratio );

    if( input_graphics_file( src_filename, &format, &n_objects,
                             &object_list ) != VIO_OK )
        return( 1 );

    for_less( i, 0, n_objects )
    {
        if( get_object_type( object_list[i] ) == POLYGONS )
        {
            polygons = get_polygons_ptr(object_list[i]);
            smooth_polygon( polygons, Max_smoothing_distance,
                            Smoothing_ratio, Smoothing_threshold,
                            Smoothing_normal_ratio,
                            FALSE, (VIO_Volume) NULL, 0, 0 );

            compute_polygon_normals( polygons );
        }
    }

    status = output_graphics_file( dest_filename, format,
                                   n_objects, object_list );

    delete_object_list( n_objects, object_list );

    return( status != VIO_OK );
}
