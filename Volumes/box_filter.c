
#include  <mni.h>

private  Real  get_amount_in_box(
    Volume    volume,
    int       x_min_voxel,
    int       x_max_voxel,
    int       y_min_voxel,
    int       y_max_voxel,
    int       z_min_voxel,
    int       z_max_voxel,
    Real      x_min,
    Real      x_max,
    Real      y_min,
    Real      y_max,
    Real      z_min,
    Real      z_max );

public  Volume  create_box_filtered_volume(
    Volume   volume,
    Real     x_width,
    Real     y_width,
    Real     z_width )
{
    Volume             resampled_volume;
    int                x, y, z;
    int                sizes[MAX_DIMENSIONS];
    Real               separations[MAX_DIMENSIONS];
    int                x_min_voxel, x_max_voxel;
    int                y_min_voxel, y_max_voxel;
    int                z_min_voxel, z_max_voxel;
    Real               x_min, x_max, y_min, y_max, z_min, z_max;
    Real               sum, voxel, sample_volume;
    BOOLEAN            shortcut;
    Real               prev_z_min, prev_z_max, added, removed;
    int                prev_z_min_voxel, prev_z_max_voxel;
    progress_struct    progress;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        HANDLE_INTERNAL_ERROR(
           "create_box_filtered_volume: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    resampled_volume = copy_volume_definition( volume, NC_UNSPECIFIED, FALSE,
                                               0.0, 0.0 );

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Box Filtering" );

    sample_volume = x_width * y_width * z_width;

    x_width /= 2.0;
    y_width /= 2.0;
    z_width /= 2.0;

    if( z_width > 4.0 )
        shortcut = TRUE;
    else
        shortcut = FALSE;

#ifdef lint
    prev_z_min = 0.0;
    prev_z_max = 0.0;
    prev_z_min_voxel = 0;
    prev_z_max_voxel = 0;
#endif

    for_less( x, 0, sizes[X] )
    {
        x_min = x - x_width;
        x_max = x + x_width;

        if( x_min < -0.5 )
            x_min = 0.5;
        if( x_max > sizes[X]-0.5 )
            x_max = sizes[X]-0.5;

        x_min_voxel = ROUND( x_min );

        if( IS_INT( x_max + 0.5 ) )
            x_max_voxel = (int) x_max;
        else
            x_max_voxel = ROUND( x_max );

        for_less( y, 0, sizes[Y] )
        {
            y_min = y - y_width;
            y_max = y + y_width;

            if( y_min < -0.5 )
                y_min = 0.5;
            if( y_max > sizes[Y]-0.5 )
                y_max = sizes[Y]-0.5;

            y_min_voxel = ROUND( y_min );

            if( IS_INT( y_max + 0.5 ) )
                y_max_voxel = (int) y_max;
            else
                y_max_voxel = ROUND( y_max );

            for_less( z, 0, sizes[Z] )
            {
                z_min = z - z_width;
                z_max = z + z_width;

                if( z_min < -0.5 )
                    z_min = 0.5;
                if( z_max > sizes[Z]-0.5 )
                    z_max = sizes[Z]-0.5;

                z_min_voxel = ROUND( z_min );

                if( IS_INT( z_max + 0.5 ) )
                    z_max_voxel = (int) z_max;
                else
                    z_max_voxel = ROUND( z_max );

                if( !shortcut || z == 0 )
                {
                    sum = get_amount_in_box( volume,
                                             x_min_voxel, x_max_voxel,
                                             y_min_voxel, y_max_voxel,
                                             z_min_voxel, z_max_voxel,
                                             x_min, x_max,
                                             y_min, y_max,
                                             z_min, z_max );
                }
                else
                {
                    removed = get_amount_in_box( volume,
                                             x_min_voxel, x_max_voxel,
                                             y_min_voxel, y_max_voxel,
                                             prev_z_min_voxel, z_min_voxel,
                                             x_min, x_max,
                                             y_min, y_max,
                                             prev_z_min, z_min );
                    added = get_amount_in_box( volume,
                                             x_min_voxel, x_max_voxel,
                                             y_min_voxel, y_max_voxel,
                                             prev_z_max_voxel, z_max_voxel,
                                             x_min, x_max,
                                             y_min, y_max,
                                             prev_z_max, z_max );

                    sum += added - removed;
                }

                voxel = sum / sample_volume;

                SET_VOXEL_3D( resampled_volume, x, y, z, voxel );

                if( shortcut )
                {
                    prev_z_min = z_min;
                    prev_z_max = z_max;
                    prev_z_min_voxel = z_min_voxel;
                    prev_z_max_voxel = z_max_voxel;
                }
            }

            update_progress_report( &progress, x * sizes[Y] + y + 1 );
        }
    }

    terminate_progress_report( &progress );

    return( resampled_volume );
}

private  Real  get_amount_in_box(
    Volume    volume,
    int       x_min_voxel,
    int       x_max_voxel,
    int       y_min_voxel,
    int       y_max_voxel,
    int       z_min_voxel,
    int       z_max_voxel,
    Real      x_min,
    Real      x_max,
    Real      y_min,
    Real      y_max,
    Real      z_min,
    Real      z_max )
{
    int   x, y, z;
    Real  sum, start, end, x_weight, xy_weight, xyz_weight, voxel;

    sum = 0.0;

    for_inclusive( x, x_min_voxel, x_max_voxel )
    {
        if( x == x_min_voxel )
            start = x_min;
        else
            start = (Real) x - 0.5;

        if( x == x_max_voxel )
            end = x_max;
        else
            end = (Real) x + 0.5;

        x_weight = end - start;

        for_inclusive( y, y_min_voxel, y_max_voxel )
        {
            if( y == y_min_voxel )
                start = y_min;
            else
                start = (Real) y - 0.5;

            if( y == y_max_voxel )
                end = y_max;
            else
                end = (Real) y + 0.5;

            xy_weight = x_weight * (end - start);

            for_inclusive( z, z_min_voxel, z_max_voxel )
            {
                if( z == z_min_voxel )
                    start = z_min;
                else
                    start = (Real) z - 0.5;

                if( z == z_max_voxel )
                    end = z_max;
                else
                    end = (Real) z + 0.5;

                xyz_weight = xy_weight * (end - start);

                GET_VOXEL_3D( voxel, volume, x, y, z );
                sum += xyz_weight * voxel;
            }
        }
    }

    return( sum );
}
