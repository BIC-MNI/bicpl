/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>
#include  <objects.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/graphics_io.c,v 1.5 1995-07-31 13:45:08 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_graphics_file
@INPUT      : filename
@OUTPUT     : format
              n_objects
              object_list
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs a file of graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_graphics_file
@INPUT      : filename
              format
              n_objects
              object_list
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Writes a file of graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_objects_any_format
@INPUT      : volume
              filename
              marker_colour  \
              marker_size      these three used to set tag information
              marker_type    /
@OUTPUT     : n_objects
              object_list
@RETURNS    : OK or ERROR
@DESCRIPTION: Inputs a landmark file, a tag point file, or a graphics file,
              creating graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status   input_objects_any_format(
    Volume         volume,
    char           filename[],
    Colour         marker_colour,
    Real           marker_size,
    Marker_types   marker_type,
    int            *n_objects,
    object_struct  **object_list[] )
{
    Status            status;
    File_formats      format;

    if( filename_extension_matches(filename,get_default_landmark_file_suffix()))
    {
        status = input_landmark_file( volume, filename,
                                      marker_colour, marker_size, marker_type,
                                      n_objects, object_list );
    }
    else if( filename_extension_matches(filename,get_default_tag_file_suffix()))
    {
        status = input_tag_objects_file( filename,
                                 marker_colour, marker_size, marker_type,
                                 n_objects, object_list );
    }
    else
    {
        status = input_graphics_file( filename, &format,
                                      n_objects, object_list );
    }

    return( status );
}
