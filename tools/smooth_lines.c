#include  <bicpl.h>

private  void  usage(
    VIO_STR   executable )
{
    VIO_STR  usage_str = "\n\
Usage: %s  input_lines.obj  output_lines.obj [smooth_distance]\n\
\n\
     Copies the input lines to the output lines, with the any points within\n\
     smooth_distance (default 1) apart being deleted.\n\n";

    print_error( usage_str, executable );
}

int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               src_filename, dest_filename;
    VIO_File_formats         format;
    VIO_Real                 smooth_distance;
    int                  i;
    int                  n_objects;
    object_struct        **objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) ||
        !get_string_argument( "", &dest_filename ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    (void) get_real_argument( 1.0, &smooth_distance );

    if( input_graphics_file( src_filename,
                             &format, &n_objects, &objects ) != OK )
        return( 1 );

    for_less( i, 0, n_objects )
    {
        if( get_object_type( objects[i] ) == LINES )
            smooth_lines( get_lines_ptr(objects[i]), smooth_distance );
    }

    if( output_graphics_file( dest_filename, format, n_objects, objects ) != OK)
        return( 1 );

    delete_object_list( n_objects, objects );

    return( 0 );
}
