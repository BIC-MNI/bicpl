#include  <def_mni.h>

public  Status  output_volume(
    char           prefix[],
    volume_struct  *volume,
    int            axis_ordering[] )
{
    Status   status;
    Real     trans;
    int      a1, a2, a3;
    int      n_bytes_per_voxel, indices[N_DIMENSIONS];
    FILE     *file;
    String   header_filename, voxel_filename;
    int      axis;

    (void) sprintf( header_filename, "%s.fre", prefix );
    (void) sprintf( voxel_filename, "%s.img", prefix );

    status = open_file( header_filename, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = output_string( file, voxel_filename  );

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
            status = output_int( file, volume->sizes[axis_ordering[axis]] );

        if( status == OK )
            status = output_real( file, volume->thickness[axis_ordering[axis]]);

        if( status == OK && volume->flip_axis[axis_ordering[axis]] )
            status = output_character( file, '-' );

        if( status == OK )
            status = output_character( file, ' ' );

        if( status == OK )
            status = output_character( file, 'x' + axis_ordering[axis] );

        if( status == OK )
            status = output_newline( file );
    }

    for_less( axis, 0, N_DIMENSIONS )
    {
        if( status == OK )
        {
            trans = Transform_elem(volume->voxel_to_world_transform,axis,3);

            if( volume->flip_axis[axis] )
            {
                trans -= (Real) (volume->sizes[axis]-1) *
                         volume->thickness[axis];
            }

            status = output_float( file,
                     Transform_elem(volume->voxel_to_world_transform,axis,3) );
        }
    }

    if( status == OK )
        status = output_newline( file );

    if( status == OK )
        status = close_file( file );

    if( status == OK )
        status = open_file( voxel_filename, WRITE_FILE, BINARY_FORMAT, &file );

    if( status == OK )
    {
        a1 = axis_ordering[0];
        a2 = axis_ordering[1];
        a3 = axis_ordering[2];
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
            }
        }
    }
                        
    return( status );
}
