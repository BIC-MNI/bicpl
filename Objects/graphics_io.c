
#include  <mni.h>

public  Status   input_graphics_file(
    char           filename[],
    File_formats   *format,
    int            *n_objects,
    object_struct  ***object_list )
{
    Status         status;
    FILE           *file;
    BOOLEAN        eof;
    object_struct  *object;
    STRING         current_directory;

    status = open_file_with_default_suffix( filename, "obj", READ_FILE,
                                            BINARY_FORMAT, &file );

    *n_objects = 0;

    if( status == OK )
    {
        extract_directory( filename, current_directory );

        do
        {
            status = input_object( current_directory, file, format,
                                   &object, &eof );

            if( status == OK && !eof )
                add_object_to_list( n_objects, object_list, object );

        } while( status == OK && !eof );
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

public  Status   output_graphics_file(
    char           filename[],
    File_formats   format,
    int            n_objects,
    object_struct  *object_list[] )
{
    Status         status;
    int            i;
    FILE           *file;

    status = open_file_with_default_suffix( filename, "obj", WRITE_FILE,
                                            BINARY_FORMAT, &file );

    if( status == OK )
    {
        for_less( i, 0, n_objects )
        {
            if( status == OK )
                status = output_object( file, format, object_list[i] );
        }
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}
