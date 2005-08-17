#include  "bicpl_internal.h"

BICAPI Status  input_volume_header_only(
    STRING               filename,
    int                  n_dimensions,
    STRING               dim_names[],
    Volume               *volume,
    minc_input_options   *options )
{
    Status               status;
    volume_input_struct  input_info;

    status = start_volume_input( filename, n_dimensions, dim_names,
                                 NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                 TRUE, volume, options,
                                 &input_info );

    if( status == OK )
        delete_volume_input( &input_info );

    return( status );
}
