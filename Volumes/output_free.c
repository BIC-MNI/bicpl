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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/output_free.c,v 1.19 1995-10-19 15:48:24 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : output_volume_free_format
@INPUT      : prefix
              volume
              axis_ordering
@OUTPUT     : 
@RETURNS    : OR or ERROR
@DESCRIPTION: Outputs a volume in the simple free format.  Will probably
              become obsolete soon.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  output_volume_free_format(
    STRING         prefix,
    Volume         volume,
    int            axis_ordering[] )
{
    Status             status;
    Real               trans, separations[MAX_DIMENSIONS];
    int                sizes[MAX_DIMENSIONS];
    int                a1, a2, a3;
    int                n_bytes_per_voxel, indices[MAX_DIMENSIONS];
    void               *ptr;
    FILE               *file;
    STRING             header_filename, voxel_filename, abs_voxel_filename;
    STRING             filename_no_dirs;
    int                axis;
    progress_struct    progress;
    General_transform  *voxel_to_world;
    Transform          *transform;

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

    if( status == OK )
        status = output_int( file, n_bytes_per_voxel );

    if( status == OK )
        status = output_newline( file );

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    voxel_to_world = get_voxel_to_world_transform( volume );

    if( get_transform_type(voxel_to_world) == LINEAR )
        transform = get_linear_transform_ptr( voxel_to_world );
    else
        transform = (Transform *) NULL;

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( status == OK )
        {
            if( transform != (Transform *) NULL )
            {
                trans = Transform_elem(*transform,axis,3);
                if( separations[axis] < 0.0 )
                    trans += separations[axis] * (Real) (sizes[axis]-1);
            }
            else
                trans = 0.0;

            status = output_float( file, (float) trans );
        }
    }

    if( status == OK )
        status = output_newline( file );

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( status == OK )
            status = output_int( file, sizes[axis_ordering[axis]] );

        if( status == OK )
            status = output_real( file, separations[axis_ordering[axis]]);

        if( status == OK )
            status = output_character( file, (char) ' ' );

        if( status == OK )
            status = output_character( file, (char) ('x'+axis_ordering[axis]) );

        if( status == OK )
            status = output_newline( file );
    }

    if( status == OK )
        status = output_string( file, voxel_filename  );

    if( status == OK )
        status = output_newline( file );

    if( status == OK )
        status = close_file( file );

    if( status == OK )
        status = open_file( abs_voxel_filename, WRITE_FILE, BINARY_FORMAT,
                            &file );

    if( status == OK )
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
                                      indices[X],indices[Y],indices[Z] );
                    status = io_binary_data( file, WRITE_FILE,
                                             ptr, n_bytes_per_voxel, 1 );
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
