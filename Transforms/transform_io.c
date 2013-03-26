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

#include "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Transforms/transform_io.c,v 1.9 2005-08-17 22:26:48 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_transform_file
@INPUT      : filename
              comments
              transform
@OUTPUT     : 
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Writes the specified transform to the file, in MNI format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  write_transform_file(
    VIO_STR       filename,
    VIO_STR       comments,
    VIO_Transform    *transform )
{
    VIO_Status              status;
    FILE                *file;
    VIO_General_transform   gen_transform;

    create_linear_transform( &gen_transform, transform );

    status = open_file_with_default_suffix( filename, "xfm",
                                 WRITE_FILE, ASCII_FORMAT, &file );

    if( status == VIO_OK )
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
@RETURNS    : VIO_OK or VIO_ERROR
@DESCRIPTION: Reads a transform from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  read_transform_file(
    VIO_STR       filename,
    VIO_Transform    *transform )
{
    VIO_Status             status;
    FILE               *file;
    VIO_Transform          *lin_trans;
    VIO_General_transform  gen_transform;

    status = open_file_with_default_suffix( filename, "xfm",
                         READ_FILE, ASCII_FORMAT, &file );

    if( status == VIO_OK )
    {
        if( input_transform( file, filename, &gen_transform ) != VIO_OK )
            status = VIO_ERROR;

        (void) close_file( file );
    }

    if( status == VIO_OK && get_transform_type(&gen_transform) != LINEAR )
    {
        print( "File %s does not contain a linear transform.\n", filename );
        status = VIO_ERROR;
    }

    if( status == VIO_OK )
    {
        lin_trans = get_linear_transform_ptr( &gen_transform );
        *transform = *lin_trans;
    }

    delete_general_transform( &gen_transform );

    return( status );
}
