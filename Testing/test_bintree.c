#include  <bicpl.h>

#define  BINTREE_FACTOR  0.5

int  main(
    int   argc,
    char  *argv[] )
{
    STRING               src_polygons_filename;
    File_formats         format;
    int                  n_src_objects;
    Real                 ratio;
    object_struct        **src_objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &src_polygons_filename ) ||
        !get_real_argument( 0.0, &ratio ) )
    {
        return( 1 );
    }

    if( input_graphics_file( src_polygons_filename,
                             &format, &n_src_objects, &src_objects ) != OK )
        return( 1 );

    print( "File input: %d items\n", get_polygons_ptr(
                                            src_objects[0])->n_items );

    create_polygons_bintree( get_polygons_ptr(src_objects[0]),
                             ROUND( (Real) get_polygons_ptr(
                                            src_objects[0])->n_items *
                                    ratio ) );


    return( 0 );
}
