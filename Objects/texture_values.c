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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Objects/texture_values.c,v 1.7 2005-08-17 22:28:27 bert Exp $";
#endif

#include "bicpl_internal.h"

static  Status  output_texture_values_ascii(
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

static  Status  input_texture_values_ascii(
    STRING   filename,
    int      *n_values,
    Real     *values[] )
{
    Status   status;
    FILE     *file;
    Real     value;

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status != OK )
        return( status );

    *n_values = 0;
    *values = NULL;

    while( input_real( file, &value ) == OK )
    {
        ADD_ELEMENT_TO_ARRAY( *values, *n_values, value, DEFAULT_CHUNK_SIZE );
    }

    (void) close_file( file );

    return( OK );
}

static  Status  output_texture_values_binary(
    STRING   filename,
    int      n_values,
    Real     values[] )
{
    int      v, sizes[2];
    Status   status;
    Volume   volume;
    STRING   dim_names[] = { MIxspace, MIyspace };

    volume = create_volume( 2, dim_names, NC_FLOAT, FALSE, 0.0, 0.0 );
    sizes[0] = 1;
    sizes[1] = n_values;
    set_volume_sizes( volume, sizes );
    alloc_volume_data( volume );

    for_less( v, 0, n_values )
        set_volume_real_value( volume, 0, v, 0, 0, 0, values[v] );

    status = output_volume( filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            volume, "Texture values.\n", NULL );

    delete_volume( volume );

    return( status );
}

static  Status  input_texture_values_binary(
    STRING   filename,
    int      *n_values,
    Real     *values[] )
{
    int      v, sizes[2];
    Status   status;
    Volume   volume;
    STRING   dim_names[] = { MIxspace, MIyspace };

    status = input_volume( filename, 2, dim_names,
                           NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                           TRUE, &volume, NULL );

    if( status != OK )
        return( status );

    get_volume_sizes( volume, sizes );

    *n_values = sizes[1];

    ALLOC( *values, *n_values );

    for_less( v, 0, *n_values )
        (*values)[v] = get_volume_real_value( volume, 0, v, 0, 0, 0 );

    delete_volume( volume );

    return( OK );
}

/*!
 * \brief Write a set of real values to file.
 *
 * The file will be created, if necessary.
 * BINARY_FORMAT files will not be portable.
 */
BICAPI  Status  output_texture_values(
    STRING         filename,
    File_formats   format,
    int            n_values,
    Real           values[] )
{
    Status   status;

    if( format == ASCII_FORMAT )
        status = output_texture_values_ascii( filename, n_values, values );
    else
        status = output_texture_values_binary( filename, n_values, values );

    return( status );
}

/*!
 * \brief Read a set of real values from file.
 *
 * Reading a BINARY_FORMAT file that was created
 * on another system will produce undefined behaviour.
 */
BICAPI  Status  input_texture_values(
    STRING         filename,
    int            *n_values,
    Real           *values[] )
{
    Status         status;

    if( filename_extension_matches( filename, MNC_ENDING ) )
        status = input_texture_values_binary( filename, n_values, values );
    else
        status = input_texture_values_ascii( filename, n_values, values );

    return( status );
}
