#include  "bicpl_internal.h"

BICAPI void  initialize_resample_volume(
    resample_struct      *resample,
    VIO_Volume               src_volume,
    VIO_General_transform    *dest_to_src_transform,
    VIO_Volume               dest_volume )
{
    VIO_General_transform  inverse, tmp;

    resample->src_volume = src_volume;
    resample->dest_volume = dest_volume;
    resample->x = 0;
    resample->y = 0;

    copy_general_transform( get_voxel_to_world_transform(dest_volume),
                            &resample->transform );

    if( dest_to_src_transform != (VIO_General_transform *) NULL )
    {
        create_inverse_general_transform( dest_to_src_transform, &inverse );

        concat_general_transforms( &resample->transform, &inverse, &tmp );
        delete_general_transform( &resample->transform );
        resample->transform = tmp;
    }

    create_inverse_general_transform( get_voxel_to_world_transform(src_volume),
                                      &inverse );

    concat_general_transforms( &resample->transform, &inverse, &tmp );
    delete_general_transform( &resample->transform );
    resample->transform = tmp;
}

BICAPI VIO_BOOL  do_more_resampling(
    resample_struct  *resample,
    VIO_Real             max_seconds,
    VIO_Real             *fraction_done )
{
    VIO_Real            value;
    VIO_BOOL         linear;
    VIO_Vector          z_axis;
    int             z;
    VIO_Real            xv, yv, zv, voxel[VIO_MAX_DIMENSIONS];
    VIO_Real            end_time;
    int             dest_sizes[VIO_MAX_DIMENSIONS], src_sizes[VIO_MAX_DIMENSIONS];

    if( max_seconds >= 0.0 )
        end_time = current_realtime_seconds() + max_seconds;

    get_volume_sizes( resample->dest_volume, dest_sizes );
    get_volume_sizes( resample->src_volume, src_sizes );

    linear = get_transform_type( &resample->transform ) == LINEAR;
    if( linear )
    {
        get_transform_z_axis( get_linear_transform_ptr(&resample->transform),
                              &z_axis );
    }

    while( resample->x < dest_sizes[X] )
    {
        for_less( z, 0, dest_sizes[Z] )
        {
            if( !linear || z == 0 )
                general_transform_point( &resample->transform,
                                         (VIO_Real) resample->x,
                                         (VIO_Real) resample->y,
                                         (VIO_Real) z,
                                         &xv, &yv, &zv );

            voxel[X] = xv;
            voxel[Y] = yv;
            voxel[Z] = zv;
            evaluate_volume( resample->src_volume, voxel, NULL, 0, FALSE,
                             get_volume_real_min(resample->src_volume),
                             &value, NULL, NULL );

            set_volume_real_value( resample->dest_volume,
                                   resample->x, resample->y, z, 0, 0, value );

            if( linear )
            {
                xv += (VIO_Real) Vector_x(z_axis);
                yv += (VIO_Real) Vector_y(z_axis);
                zv += (VIO_Real) Vector_z(z_axis);
            }
        }

        ++resample->y;
        if( resample->y >= dest_sizes[Y] )
        {
            resample->y = 0;
            ++resample->x;
        }

        if( max_seconds >= 0.0 && current_realtime_seconds() > end_time )
            break;
    }

    *fraction_done = (VIO_Real) (resample->x * dest_sizes[Y] + resample->y) /
                     (VIO_Real) dest_sizes[Y] / (VIO_Real) dest_sizes[X];

    return( resample->x < dest_sizes[X] );
}

BICAPI void  resample_volume(
    VIO_Volume                   src_volume,
    VIO_General_transform        *dest_to_src_transform,
    VIO_Volume                   dest_volume )
{
    static const     int  FACTOR = 1000;
    resample_struct  resample;
    VIO_Real             amount_done;
    progress_struct  progress;

    initialize_resample_volume( &resample, src_volume, dest_to_src_transform,
                                dest_volume );

    initialize_progress_report( &progress, FALSE, FACTOR, "Resampling" );

    while( do_more_resampling( &resample, 5.0, &amount_done ) )
    {
        update_progress_report( &progress, ROUND(amount_done*(VIO_Real)FACTOR) );
    }

    terminate_progress_report( &progress );
}
