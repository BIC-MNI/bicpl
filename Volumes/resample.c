#include  <internal_volume_io.h>
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
    int             value;
    BOOLEAN         linear;
    Vector          z_axis;
    int             z;
    Real            xv, yv, zv;
    Real            end_time, real_value;
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
                                         resample->x, resample->y, (Real) z,
                                         &xv, &yv, &zv );

            if( xv < 0.0 || xv >= (Real) (src_sizes[X]-1) ||
                yv < 0.0 || yv >= (Real) (src_sizes[Y]-1) ||
                zv < 0.0 || zv >= (Real) (src_sizes[Z]-1) )
            {
                if( xv < -0.5 || xv >= (Real) src_sizes[X] - 0.5 ||
                    yv < -0.5 || yv >= (Real) src_sizes[Y] - 0.5 ||
                    zv < -0.5 || zv >= (Real) src_sizes[Z] - 0.5 )
                {
                    value = get_volume_voxel_min( resample->src_volume );
                }
                else
                {
                    GET_VOXEL_3D( value, resample->src_volume,
                                  ROUND( xv ), ROUND( yv ), ROUND( zv ) );
                }
            }
            else
            {
                evaluate_3D_volume( resample->src_volume, xv, yv, zv,
                                    0, &real_value,
                                    (Real *) 0, (Real *) 0, (Real *) 0,
                                    (Real *) 0, (Real *) 0, (Real *) 0,
                                    (Real *) 0, (Real *) 0, (Real *) 0 );

                value = ROUND( real_value );
            }

            SET_VOXEL_3D( resample->dest_volume, resample->x, resample->y, z,
                          value );

            if( linear )
            {
                xv += Vector_x(z_axis);
                yv += Vector_y(z_axis);
                zv += Vector_z(z_axis);
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
