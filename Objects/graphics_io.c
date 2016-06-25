/** 
 * \file graphics_io.c
 * \brief Read/write graphics objects.
 *
 \copyright
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
*/

#include "bicpl_internal.h"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : input_graphics_file
@INPUT      : filename
@OUTPUT     : format
              n_objects
              object_list
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Inputs a file of graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status   input_graphics_file(
    VIO_STR         filename,
    VIO_File_formats   *format,
    int            *n_objects,
    object_struct  ***object_list )
{
    VIO_Status         status;
    FILE           *file;
    VIO_BOOL        eof;
    object_struct  *object;
    VIO_STR         current_directory;

    status = open_file_with_default_suffix( filename, "obj", READ_FILE,
                                            BINARY_FORMAT, &file );

    *n_objects = 0;

    if( status == VIO_OK )
    {
        current_directory = extract_directory( filename );

        do
        {
            status = input_object( current_directory, file, format,
                                   &object, &eof );

            if( status == VIO_OK && !eof )
                add_object_to_list( n_objects, object_list, object );

        } while( status == VIO_OK && !eof );

        delete_string( current_directory );
    }

    if( status == VIO_OK )
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
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Writes a file of graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status   output_graphics_file(
    VIO_STR         filename,
    VIO_File_formats   format,
    int            n_objects,
    object_struct  *object_list[] )
{
    VIO_Status         status;
    int            i;
    FILE           *file;

    status = open_file_with_default_suffix( filename, "obj", WRITE_FILE,
                                            BINARY_FORMAT, &file );

    if( status == VIO_OK )
    {
        for_less( i, 0, n_objects )
        {
            if( status == VIO_OK )
                status = output_object( file, format, object_list[i] );
        }
    }

    if( status == VIO_OK )
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
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Inputs a landmark file, a tag point file, or a graphics file,
              creating graphics objects.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status   input_objects_any_format(
    VIO_Volume     volume,
    VIO_STR        filename,
    VIO_Colour     marker_colour,
    VIO_Real       marker_size,
    Marker_types   marker_type,
    int            *n_objects,
    object_struct  **object_list[] )
{
    VIO_Status            status = VIO_OK;
    VIO_File_formats      format;

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
    else if (!input_freesurfer_graphics_file(filename, n_objects, object_list) &&
             !input_brainsuite_graphics_file(filename, n_objects, object_list) &&
             !input_wavefront_graphics_file(filename, n_objects, object_list))
    {
        status = input_graphics_file( filename, &format,
                                      n_objects, object_list );
    }

    return( status );
}
