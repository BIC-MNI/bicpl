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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/texture_values.c,v 1.1 1997-01-27 15:27:42 david Exp $";
#endif

#include  <internal_volume_io.h>
#include  <objects.h>

public  Status  output_texture_values(
    STRING   filename,
    int      n_values,
    Real     values[] )
{
    int      v;
    Status   status;
    FILE     *file;

    status = open_file( filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( status != OK )
        return( status );

    for_less( v, 0, n_values )
    {
        if( output_real( file, values[v] ) != OK ||
            output_newline( file ) != OK )
        {
            print_error( "Error outputting %d'th value out of %d to file %s\n",
                         v+1, n_values, filename );
            return( ERROR );
        }
    }

    (void) close_file( file );

    return( OK );
}

public  Status  input_texture_values(
    STRING   filename,
    int      n_values,
    Real     values[] )
{
    Status   status;
    FILE     *file;
    int      v;
    Real     after_end;

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status != OK )
        return( status );

    for_less( v, 0, n_values )
    {
        if( input_real( file, &values[v] ) != OK )
        {
            print_error( "Error inputting %d'th value out of %d from file %s\n",
                         v+1, n_values, filename );
            return( ERROR );
        }
    }

    if( input_real( file, &after_end ) == OK )
    {
        print_error( "Warning: extra values at end of file: %s\n", filename );
    }

    (void) close_file( file );

    return( OK );
}
