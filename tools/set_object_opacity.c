#include  <bicpl.h>

static  void  usage(
    VIO_STR   executable )
{
    VIO_STR  usage_str = "\n\
Usage: %s  input.obj  output.obj opacity\n\
\n\
     Copies the input objects to the output objects, with the opacity set\n\
     accordingly, a value between 0 (transparent) and 1.\n\n";

    print_error( usage_str, executable );
}

int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               src_filename, dest_filename;
    VIO_File_formats         format;
    VIO_Surfprop             *spr;
    VIO_Real                 opacity;
    Colour_flags         colour_flag;
    VIO_Colour               col, *colours;
    int                  i, n_colours, n_points, c;
    int                  n_objects;
    VIO_Point                *points;
    object_struct        **objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) ||
        !get_string_argument( "", &dest_filename ) ||
        !get_real_argument( 0.0, &opacity ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    if( input_graphics_file( src_filename,
                             &format, &n_objects, &objects ) != VIO_OK )
        return( 1 );

    for_less( i, 0, n_objects )
    {
        spr = get_object_surfprop( objects[i] );
        if( spr != NULL )
            Surfprop_t(*spr) = (VIO_Spr_type) opacity;

        colour_flag = *get_object_colours( objects[i], &colours );

        n_points = get_object_points( objects[i], &points );

        n_colours = get_n_colours( colour_flag, n_points, 1 );

        for_less( c, 0, n_colours )
        {
            col = colours[c];
            colours[c] = make_rgba_Colour_0_1( get_Colour_r_0_1(col),
                                               get_Colour_g_0_1(col),
                                               get_Colour_b_0_1(col),
                                               opacity );
        }
    }

    if( output_graphics_file( dest_filename, format, n_objects, objects ) != VIO_OK)
        return( 1 );

    delete_object_list( n_objects, objects );

    return( 0 );
}
