#include  <volume_io/internal_volume_io.h>
#include  <vols.h>

public  void  initialize_resample_volume(
    resample_struct      *resample,
    Volume               src_volume,
    General_transform    *dest_to_src_transform,
    Volume               dest_volume )
{
    General_transform  inverse, tmp;

    resample->src_volume = src_volume;
    resample->dest_volume = dest_volume;
    resample->x = 0;
    resample->y = 0;

    copy_general_transform( get_voxel_to_world_transform(dest_volume),
                            &resample->transform );

    if( dest_to_src_transform != (General_transform *) NULL )
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

public  BOOLEAN  do_more_resampling(
    resample_struct  *resample,
    Real             max_seconds,
    Real             *fraction_done )
{
    Real            value;
    BOOLEAN         linear;
    Vector          z_axis;
    int             z;
    Real            xv, yv, zv, voxel[MAX_DIMENSIONS];
    Real            end_time;
    int             dest_sizes[MAX_DIMENSIONS], src_sizes[MAX_DIMENSIONS];

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
                                         (Real) resample->x,
                                         (Real) resample->y,
                                         (Real) z,
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
                xv += (Real) Vector_x(z_axis);
                yv += (Real) Vector_y(z_axis);
                zv += (Real) Vector_z(z_axis);
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

    *fraction_done = (Real) (resample->x * dest_sizes[Y] + resample->y) /
                     (Real) dest_sizes[Y] / (Real) dest_sizes[X];

    return( resample->x < dest_sizes[X] );
}

public  void  resample_volume(
    Volume                   src_volume,
    General_transform        *dest_to_src_transform,
    Volume                   dest_volume )
{
    static const     int  FACTOR = 1000;
    resample_struct  resample;
    Real             amount_done;
    progress_struct  progress;

    initialize_resample_volume( &resample, src_volume, dest_to_src_transform,
                                dest_volume );

    initialize_progress_report( &progress, FALSE, FACTOR, "Resampling" );

    while( do_more_resampling( &resample, 5.0, &amount_done ) )
    {
        update_progress_report( &progress, ROUND(amount_done*(Real)FACTOR) );
    }

    terminate_progress_report( &progress );
}
