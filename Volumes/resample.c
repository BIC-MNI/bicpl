#include  <def_mni.h>

public  void  initialize_resample_volume(
    resample_struct  *resample,
    Volume           src_volume,
    Transform        *dest_to_src_transform,
    Volume           dest_volume )
{
    Transform  inverse;

    resample->src_volume = src_volume;
    resample->dest_volume = dest_volume;
    resample->x = 0;
    resample->y = 0;

    resample->transform = dest_volume->voxel_to_world_transform;

    if( dest_to_src_transform != (Transform *) NULL )
    {
        compute_transform_inverse( dest_to_src_transform, &inverse );
        concat_transforms( &resample->transform, &resample->transform,
                           &inverse );
    }

    concat_transforms( &resample->transform, &resample->transform,
                       &src_volume->world_to_voxel_transform );
}

public  Boolean  do_more_resampling(
    resample_struct  *resample,
    Real             max_seconds,
    Real             *fraction_done )
{
    int             value;
    int             z, xi, yi, zi;
    int             v000, v001, v010, v011, v100, v101, v110, v111;
    Real            v00, v01, v10, v11, v0, v1;
    Real            xv, yv, zv;
    Real            u, v, w;
    Real            end_time, real_value;
    Point           point;
    Vector          z_axis;
    int             *dest_sizes, *src_sizes;

    if( max_seconds >= 0.0 )
        end_time = current_realtime_seconds() + max_seconds;

    dest_sizes = resample->dest_volume->sizes;

    while( resample->x < dest_sizes[X] )
    {
        src_sizes = resample->src_volume->sizes;

        get_transform_z_axis( &resample->transform, &z_axis );

        fill_Point( point, (Real) resample->x, (Real) resample->y, 0.0 );
        transform_point( &resample->transform, &point, &point );
        xv = Point_x(point);
        yv = Point_y(point);
        zv = Point_z(point);
        for_less( z, 0, dest_sizes[Z] )
        {
            if( xv < 0.0 || xv >= (Real) (src_sizes[X]-1) ||
                yv < 0.0 || yv >= (Real) (src_sizes[Y]-1) ||
                zv < 0.0 || zv >= (Real) (src_sizes[Z]-1) )
            {
                if( xv < -0.5 || xv >= (Real) src_sizes[X] - 0.5 ||
                    yv < -0.5 || yv >= (Real) src_sizes[Y] - 0.5 ||
                    zv < -0.5 || zv >= (Real) src_sizes[Z] - 0.5 )
                {
                    value = resample->src_volume->min_value;
                }
                else
                {
                    GET_VOXEL_3D( value, resample->src_volume,
                                     ROUND( xv ), ROUND( yv ), ROUND( zv ) );
                }
            }
            else
            {
                (void) evaluate_volume( resample->src_volume, xv, yv, zv,
                                        0, FALSE, &real_value,
                                        (Real *) 0, (Real *) 0, (Real *) 0 );

                value = ROUND( real_value );
            }

            SET_VOXEL_3D( resample->dest_volume, resample->x, resample->y, z,
                          value );

            xv += Point_x(z_axis);
            yv += Point_y(z_axis);
            zv += Point_z(z_axis);
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
    Volume           src_volume,
    Transform        *dest_to_src_transform,
    Volume           dest_volume )
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
