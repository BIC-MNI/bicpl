#include  <def_mni.h>

public  Status  output_volume(
    char           prefix[],
    volume_struct  *volume,
    int            axis_ordering[] )
{
    Status           status;
    Real             trans;
    int              a1, a2, a3;
    int              n_bytes_per_voxel, indices[N_DIMENSIONS];
    FILE             *file;
    String           header_filename, voxel_filename, abs_voxel_filename;
    String           filename_no_dirs;
    int              axis;
    progress_struct  progress;

    (void) sprintf( header_filename, "%s.fre", prefix );
    (void) sprintf( abs_voxel_filename, "%s.img", prefix );

    remove_directories_from_filename( prefix, filename_no_dirs );
    (void) sprintf( voxel_filename, "%s.img", filename_no_dirs );

    status = open_file( header_filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( volume->data_type == UNSIGNED_BYTE )
        n_bytes_per_voxel = 1;
    else
        n_bytes_per_voxel = 2;

    if( status == OK )
        status = output_int( file, n_bytes_per_voxel );

    if( status == OK )
        status = output_newline( file );

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( status == OK )
        {
            trans = Transform_elem(volume->voxel_to_world_transform,axis,3);

            if( volume->thickness[axis] < 0.0 )
            {
                trans += volume->thickness[axis] *
                         (Real) (volume->sizes[axis]-1);
            }

            status = output_float( file, trans );
        }
    }

    if( status == OK )
        status = output_newline( file );

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( status == OK )
            status = output_int( file, volume->sizes[axis_ordering[axis]] );

        if( status == OK )
            status = output_real( file, volume->thickness[axis_ordering[axis]]);

        if( status == OK )
            status = output_character( file, ' ' );

        if( status == OK )
            status = output_character( file, 'x' + axis_ordering[axis] );

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

        initialize_progress_report( &progress, FALSE,
                                    volume->sizes[a1] * volume->sizes[a2],
                                    "Writing Volume" );

        for_less( indices[a1], 0, volume->sizes[a1] )
        {
            for_less( indices[a2], 0, volume->sizes[a2] )
            {
                for_less( indices[a3], 0, volume->sizes[a3] )
                {
                    status = io_binary_data( file, WRITE_FILE,
                                    GET_VOLUME_DATA_PTR(*volume,
                                    indices[X],indices[Y],indices[Z]),
                                    n_bytes_per_voxel, 1 );
                }

                update_progress_report( &progress,
                                        indices[a1] * volume->sizes[a2] +
                                        indices[a2] + 1 );
            }
        }

        terminate_progress_report( &progress );
    }
                        
    return( status );
}
