
#include  <mni.h>

private  Real  get_amount_in_box(
    Volume    volume,
    int       min_voxel[],
    int       max_voxel[],
    Real      min_pos[],
    Real      max_pos[] );

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
    int                min_voxel[MAX_DIMENSIONS], max_voxel[MAX_DIMENSIONS];
    Real               min_pos[MAX_DIMENSIONS], max_pos[MAX_DIMENSIONS];
    Real               sum, voxel, sample_volume;
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

    for_less( x, 0, sizes[X] )
    {
        min_pos[X] = x - x_width;
        max_pos[X] = x + x_width;

        if( min_pos[X] < -0.5 )
            min_pos[X] = 0.5;
        if( max_pos[X] > sizes[X]-0.5 )
            max_pos[X] = sizes[X]-0.5;

        min_voxel[X] = ROUND( min_pos[X] );

        if( IS_INT( max_pos[X] + 0.5 ) )
            max_voxel[X] = (int) max_pos[X];
        else
            max_voxel[X] = ROUND( max_pos[X] );

        for_less( y, 0, sizes[Y] )
        {
            min_pos[Y] = y - y_width;
            max_pos[Y] = y + y_width;

            if( min_pos[Y] < -0.5 )
                min_pos[Y] = 0.5;
            if( max_pos[Y] > sizes[Y]-0.5 )
                max_pos[Y] = sizes[Y]-0.5;

            min_voxel[Y] = ROUND( min_pos[Y] );

            if( IS_INT( max_pos[Y] + 0.5 ) )
                max_voxel[Y] = (int) max_pos[Y];
            else
                max_voxel[Y] = ROUND( max_pos[Y] );

            for_less( z, 0, sizes[Z] )
            {
                min_pos[Z] = z - z_width;
                max_pos[Z] = z + z_width;

                if( min_pos[Z] < -0.5 )
                    min_pos[Z] = 0.5;
                if( max_pos[Z] > sizes[Z]-0.5 )
                    max_pos[Z] = sizes[Z]-0.5;

                min_voxel[Z] = ROUND( min_pos[Z] );

                if( IS_INT( max_pos[Z] + 0.5 ) )
                    max_voxel[Z] = (int) max_pos[Z];
                else
                    max_voxel[Z] = ROUND( max_pos[Z] );

                sum = get_amount_in_box( volume, min_voxel, max_voxel,
                                         min_pos, max_pos );

                voxel = sum / sample_volume;

                SET_VOXEL_3D( resampled_volume, x, y, z, voxel );
            }

            update_progress_report( &progress, x * sizes[Y] + y + 1 );
        }
    }

    terminate_progress_report( &progress );

    return( resampled_volume );
}

private  Real  get_amount_in_box(
    Volume    volume,
    int       min_voxel[],
    int       max_voxel[],
    Real      min_pos[],
    Real      max_pos[] )
{
    int   x, y, z;
    Real  sum, start, end, x_weight, xy_weight, xyz_weight, voxel;

    sum = 0.0;

    for_inclusive( x, min_voxel[X], max_voxel[X] )
    {
        if( x == min_voxel[X] )
            start = min_pos[X];
        else
            start = (Real) x - 0.5;

        if( x == max_voxel[X] )
            end = max_pos[X];
        else
            end = (Real) x + 0.5;

        x_weight = end - start;

        for_inclusive( y, min_voxel[Y], max_voxel[Y] )
        {
            if( y == min_voxel[Y] )
                start = min_pos[Y];
            else
                start = (Real) y - 0.5;

            if( y == max_voxel[Y] )
                end = max_pos[Y];
            else
                end = (Real) y + 0.5;

            xy_weight = x_weight * (end - start);

            for_inclusive( z, min_voxel[Z], max_voxel[Z] )
            {
                if( z == min_voxel[Z] )
                    start = min_pos[Z];
                else
                    start = (Real) z - 0.5;

                if( z == max_voxel[Z] )
                    end = max_pos[Z];
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
