
#include  <internal_volume_io.h>
#include  <geom.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Transforms/transform_io.c,v 1.1 1995-07-08 03:37:47 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_transform_file
@INPUT      : filename
              comments
              transform
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Writes the specified transform to the file, in MNI format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  write_transform_file(
    char       filename[],
    char       comments[],
    Transform  *transform )
{
    Status              status;
    FILE                *file;
    General_transform   gen_transform;

    create_linear_transform( &gen_transform, transform );

    status = open_file_with_default_suffix( filename, "xfm",
                                 WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = output_transform( file, filename, NULL, comments,
                                   &gen_transform );

        (void) close_file( file );
    }

    delete_general_transform( &gen_transform );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_transform_file
@INPUT      : filename
@OUTPUT     : transform
@RETURNS    : OK or ERROR
@DESCRIPTION: Reads a transform from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  read_transform_file(
    char       filename[],
    Transform  *transform )
{
    Status             status;
    FILE               *file;
    General_transform  gen_transform;

    status = open_file_with_default_suffix( filename, "xfm",
                         READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        if( input_transform( file, filename, &gen_transform ) != OK )
            status = ERROR;

        (void) close_file( file );
    }

    if( status == OK && get_transform_type(&gen_transform) != LINEAR )
    {
        print( "File %s does not contain a linear transform.\n", filename );
        status = ERROR;
    }

    if( status == OK )
        *transform = *get_linear_transform_ptr(&gen_transform);

    delete_general_transform( &gen_transform );

    return( status );
}
