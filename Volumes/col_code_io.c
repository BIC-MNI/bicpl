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
#include  <vols.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/col_code_io.c,v 1.1 1997-04-16 15:38:52 david Exp $";
#endif

#define    DEFAULT_USER_DEFINED_COLOUR_CODE_SUFFIX      "ccd"

public  STRING    get_default_user_def_colour_code_suffix( void )
{
    return( DEFAULT_USER_DEFINED_COLOUR_CODE_SUFFIX );
}

public  Status  input_user_defined_colour_coding(
    colour_coding_struct   *colour_coding,
    STRING                 filename )
{
    Status   status;
    FILE     *file;
    Real     pos, *positions;
    Colour   col, *colours;
    STRING   line;
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
