#include  <def_mni.h>

public  void  initialize_resample_volume(
    resample_struct  *resample,
    volume_struct    *src_volume,
    Transform        *dest_to_src_transform,
    volume_struct    *dest_volume )
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
    Real            end_time;
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
                    value = GET_VOLUME_DATA( *resample->src_volume, ROUND( xv ),
                                             ROUND( yv ), ROUND( zv ) );
                }
            }
            else
            {
                xi = (int) xv;
                yi = (int) yv;
                zi = (int) zv;
                u = FRACTION( xv );
                v = FRACTION( yv );
                w = FRACTION( zv );

                v000 = GET_VOLUME_DATA(*resample->src_volume, xi  , yi  , zi );
                v001 = GET_VOLUME_DATA(*resample->src_volume, xi  , yi  , zi+1);
                v010 = GET_VOLUME_DATA(*resample->src_volume, xi  , yi+1, zi );
                v011 = GET_VOLUME_DATA(*resample->src_volume, xi  , yi+1, zi+1);
                v100 = GET_VOLUME_DATA(*resample->src_volume, xi+1, yi  , zi );
                v101 = GET_VOLUME_DATA(*resample->src_volume, xi+1, yi  , zi+1);
                v110 = GET_VOLUME_DATA(*resample->src_volume, xi+1, yi+1, zi );
                v111 = GET_VOLUME_DATA(*resample->src_volume, xi+1, yi+1, zi+1);
                
                v00 = (Real) v000 + (Real) (v001 - v000) * w;
                v01 = (Real) v010 + (Real) (v011 - v010) * w;
                v10 = (Real) v100 + (Real) (v101 - v100) * w;
                v11 = (Real) v110 + (Real) (v111 - v110) * w;

                v0 = (Real) v00 + (Real) (v01 - v00) * v;
                v1 = (Real) v10 + (Real) (v11 - v10) * v;

                value = ROUND( (Real) v0 + (Real) (v1 - v0) * u );
            }

            ASSIGN_VOLUME_DATA( *resample->dest_volume,
                                resample->x, resample->y, z, value );

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
    volume_struct    *src_volume,
    Transform        *dest_to_src_transform,
    volume_struct    *dest_volume )
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
