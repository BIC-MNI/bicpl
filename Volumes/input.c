#include  "bicpl_internal.h"

BICAPI VIO_Status  input_volume_header_only(
    VIO_STR               filename,
    int                  n_dimensions,
    VIO_STR               dim_names[],
    VIO_Volume               *volume,
    minc_input_options   *options )
{
    VIO_Status               status;
    volume_input_struct  input_info;

    status = start_volume_input( filename, n_dimensions, dim_names,
                                 NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                 TRUE, volume, options,
                                 &input_info );

    if( status == VIO_OK )
        delete_volume_input( &input_info );

    return( status );
}
