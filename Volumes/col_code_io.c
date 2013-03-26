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

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/col_code_io.c,v 1.6 2005-08-17 22:26:19 bert Exp $";
#endif

#define    DEFAULT_USER_DEFINED_COLOUR_CODE_SUFFIX      "ccd"

BICAPI VIO_STR    get_default_user_def_colour_code_suffix( void )
{
    return( DEFAULT_USER_DEFINED_COLOUR_CODE_SUFFIX );
}

BICAPI VIO_Status  input_user_defined_colour_coding(
    colour_coding_struct   *colour_coding,
    VIO_STR                 filename )
{
    VIO_Status   status;
    FILE     *file;
    VIO_Real     pos, *positions;
    VIO_Colour   col, *colours;
    VIO_STR   line;
    int      n_colours;

    if( open_file_with_default_suffix( filename,
                                      get_default_user_def_colour_code_suffix(),
                                      READ_FILE, ASCII_FORMAT, &file ) != OK )
        return( ERROR );

    n_colours = 0;
    colours = NULL;
    positions = NULL;

    status = OK;
    while( input_real( file, &pos ) == OK )
    {
        if( input_line( file, &line ) != OK )
        {
            print_error( "Error loading user defined colour coding.\n" );
            status = ERROR;
            break;
        }

        col = convert_string_to_colour( line );

        delete_string( line );

        ADD_ELEMENT_TO_ARRAY( colours, n_colours, col, DEFAULT_CHUNK_SIZE );
	/* n_colours is the number of elements currently in the array.
	   A side-effect of the macro ADD_ELEMENT_TO_ARRAY is that n_colours is
	   incremented, so we have to decrement it before using it as the index
	   for the second array. */
        --n_colours; 
        ADD_ELEMENT_TO_ARRAY( positions, n_colours, pos, DEFAULT_CHUNK_SIZE );
    }

    (void) close_file( file );

    if( status == OK &&
        !define_colour_coding_user_defined( colour_coding,
                                n_colours, colours, positions, RGB_SPACE ) )
    {
        status = ERROR;
    }

    if( n_colours > 0 )
    {
        FREE( colours );
        FREE( positions );
    }

    return( status );
}
