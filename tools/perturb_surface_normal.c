#include  <bicpl.h>

static   VIO_Real  evaluate_gaussian(
    VIO_Real   d,
    VIO_Real   fwhm );

int  main(
    int    argc,
    char   *argv[] )
{
    VIO_STR           input_filename, output_filename;
    int              i, n_objects;
    VIO_File_formats     format;
    object_struct    **object_list;
    polygons_struct  *polygons;
    VIO_Point            centre;
    VIO_Real             fwhm, distance, x, y, z;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &input_filename ) ||
        !get_real_argument( 0.0, &x ) ||
        !get_real_argument( 0.0, &y ) ||
        !get_real_argument( 0.0, &z ) ||
        !get_real_argument( 0.0, &fwhm ) ||
        !get_real_argument( 0.0, &distance ) ||
        !get_string_argument( NULL, &output_filename ) )
    {
        fprintf(stderr,
          "Usage: %s  input.obj  x y z fwhm dist out.obj\n", argv[0] );
        return( 1 );
    }

    if( input_graphics_file( input_filename, &format, &n_objects,
                             &object_list ) != VIO_OK || n_objects != 1 ||
        get_object_type(object_list[0]) != POLYGONS )
    {
        fprintf(stderr, "Error reading %s.\n", input_filename );
        return( 1 );
    }

    polygons = get_polygons_ptr( object_list[0] );
    fill_Point( centre, x, y, z );

    for_less( i, 0, polygons->n_points )
    {
        VIO_Real dist = distance_between_points( &polygons->points[i], &centre );
        VIO_Real mag = distance * evaluate_gaussian( dist, fwhm );
        Point_x(polygons->points[i]) += Vector_x(polygons->normals[i]) * mag;
        Point_y(polygons->points[i]) += Vector_y(polygons->normals[i]) * mag;
        Point_z(polygons->points[i]) += Vector_z(polygons->normals[i]) * mag;
    }

    //compute_polygon_normals( polygons );

    output_graphics_file( output_filename, format, 1, object_list );

    return( 0 );
}

static   VIO_Real  evaluate_gaussian(
    VIO_Real   d,
    VIO_Real   fwhm )
{
    VIO_Real  e;

    e = -0.5 * d * d * 8.0 * log( 2.0 ) / fwhm / fwhm;

    return( exp( e ) );
}
