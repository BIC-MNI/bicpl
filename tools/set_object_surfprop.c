#include  <bicpl.h>

private  void  usage(
    VIO_STR   executable )
{
    VIO_STR  usage_str = "\n\
Usage: %s  input.obj  output.obj amb_coef diff_coef spec_coef spec_exp opacity\n\
\n\
     Copies the input objects to the output objects, with the surfprop set\n\
     accordingly.\n\n";

    print_error( usage_str, executable );
}


int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               src_filename, dest_filename;
    VIO_File_formats         format;
    VIO_Real                 amb_coef, diff_coef, spec_coef, spec_exp, opacity;
    VIO_Surfprop             spr;
    int                  i;
    int                  n_objects;
    object_struct        **objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) ||
        !get_string_argument( "", &dest_filename ) ||
        !get_real_argument( 0.0, &amb_coef ) ||
        !get_real_argument( 0.0, &diff_coef ) ||
        !get_real_argument( 0.0, &spec_coef ) ||
        !get_real_argument( 0.0, &spec_exp ) ||
        !get_real_argument( 0.0, &opacity ) )
    {
        usage( argv[0] );
        return( 1 );
    }

    fill_Surfprop( spr, amb_coef, diff_coef, spec_coef, spec_exp, opacity );

    if( input_graphics_file( src_filename,
                             &format, &n_objects, &objects ) != OK )
        return( 1 );

    for_less( i, 0, n_objects )
        set_object_surfprop( objects[i], &spr );

    if( output_graphics_file( dest_filename, format, n_objects, objects ) != OK)
        return( 1 );

    delete_object_list( n_objects, objects );

    return( 0 );
}
