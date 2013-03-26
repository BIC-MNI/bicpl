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
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Volumes/output_free.c,v 1.23 2005-08-17 22:26:19 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_volume_free_format
@INPUT      : prefix
              volume
              axis_ordering
@OUTPUT     : 
@RETURNS    : OR or VIO_ERROR
@DESCRIPTION: Outputs a volume in the simple free format.  Will probably
              become obsolete soon.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_Status  output_volume_free_format(
    VIO_STR         prefix,
    VIO_Volume         volume,
    int            axis_ordering[] )
{
    VIO_Status             status;
    VIO_Real               trans, separations[VIO_MAX_DIMENSIONS];
    int                sizes[VIO_MAX_DIMENSIONS];
    int                a1, a2, a3;
    int                n_bytes_per_voxel, indices[VIO_MAX_DIMENSIONS];
    void               *ptr;
    FILE               *file;
    VIO_STR             header_filename, voxel_filename, abs_voxel_filename;
    VIO_STR             filename_no_dirs;
    int                axis;
    VIO_progress_struct    progress;
    VIO_General_transform  *voxel_to_world;
    VIO_Transform          *transform;

    header_filename = concat_strings( prefix, ".fre" );
    abs_voxel_filename = alloc_string( string_length(prefix) + 10 );
    abs_voxel_filename = concat_strings( prefix, ".img" );

    filename_no_dirs = remove_directories_from_filename( prefix );
    voxel_filename = concat_strings( filename_no_dirs, ".img" );

    status = open_file( header_filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( get_volume_data_type(volume) == UNSIGNED_BYTE )
        n_bytes_per_voxel = 1;
    else
        n_bytes_per_voxel = 2;

    if( status == VIO_OK )
        status = output_int( file, n_bytes_per_voxel );

    if( status == VIO_OK )
        status = output_newline( file );

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    voxel_to_world = get_voxel_to_world_transform( volume );

    if( get_transform_type(voxel_to_world) == LINEAR )
        transform = get_linear_transform_ptr( voxel_to_world );
    else
        transform = (VIO_Transform *) NULL;

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        if( status == VIO_OK )
        {
            if( transform != (VIO_Transform *) NULL )
            {
                trans = Transform_elem(*transform,axis,3);
                if( separations[axis] < 0.0 )
                    trans += separations[axis] * (VIO_Real) (sizes[axis]-1);
            }
            else
                trans = 0.0;

            status = output_float( file, (float) trans );
        }
    }

    if( status == VIO_OK )
        status = output_newline( file );

    for_less( axis, 0, VIO_N_DIMENSIONS )
    {
        if( status == VIO_OK )
            status = output_int( file, sizes[axis_ordering[axis]] );

        if( status == VIO_OK )
            status = output_real( file, separations[axis_ordering[axis]]);

        if( status == VIO_OK )
            status = output_character( file, (char) ' ' );

        if( status == VIO_OK )
            status = output_character( file, (char) ('x'+axis_ordering[axis]) );

        if( status == VIO_OK )
            status = output_newline( file );
    }

    if( status == VIO_OK )
        status = output_string( file, voxel_filename  );

    if( status == VIO_OK )
        status = output_newline( file );

    if( status == VIO_OK )
        status = close_file( file );

    if( status == VIO_OK )
        status = open_file( abs_voxel_filename, WRITE_FILE, BINARY_FORMAT,
                            &file );

    if( status == VIO_OK )
    {
        a1 = axis_ordering[0];
        a2 = axis_ordering[1];
        a3 = axis_ordering[2];

        initialize_progress_report( &progress, FALSE, sizes[a1] * sizes[a2],
                                    "Writing Volume" );

        for_less( indices[a1], 0, sizes[a1] )
        {
            for_less( indices[a2], 0, sizes[a2] )
            {
                for_less( indices[a3], 0, sizes[a3] )
                {
                    GET_VOXEL_PTR_3D( ptr, volume,
                                      indices[VIO_X],indices[VIO_Y],indices[VIO_Z] );
                    status = io_binary_data( file, WRITE_FILE,
                                             ptr, (size_t) n_bytes_per_voxel,1);
                }

                update_progress_report( &progress, indices[a1] * sizes[a2] +
                                        indices[a2] + 1 );
            }
        }

        terminate_progress_report( &progress );
    }

    delete_string( header_filename );
    delete_string( voxel_filename );
    delete_string( abs_voxel_filename );
    delete_string( filename_no_dirs );
                        
    return( status );
}
