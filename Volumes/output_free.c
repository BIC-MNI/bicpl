#include  <def_mni.h>

public  Status  output_volume(
    char           prefix[],
    volume_struct  *volume )
{
    Status   status;
    int      x, y, z, n_bytes_per_voxel;
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

    for( axis = Z;  axis >= X;  --axis )
    {
        if( status == OK )
            status = output_int( file, volume->sizes[axis] );

        if( status == OK )
            status = output_real( file, volume->thickness[axis] );

        if( status == OK && volume->flip_axis[axis] )
            status = output_character( file, '-' );

        if( status == OK )
            status = output_character( file, ' ' );

        if( status == OK )
            status = output_character( file, 'x' + axis );

        if( status == OK )
            status = output_newline( file );
    }

    if( status == OK )
        status = close_file( file );

    if( status == OK )
        status = open_file( voxel_filename, WRITE_FILE, BINARY_FORMAT, &file );

    if( status == OK )
    {
        for_less( z, 0, volume->sizes[Z] )
        {
            for_less( y, 0, volume->sizes[Y] )
            {
                for_less( x, 0, volume->sizes[X] )
                {
                    status = io_binary_data( file, WRITE_FILE,
                                             GET_VOLUME_DATA_PTR(*volume,x,y,z),
                                             n_bytes_per_voxel, 1 );
                }
            }
        }
    }
                        
    return( status );
}
