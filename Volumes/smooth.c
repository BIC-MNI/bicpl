
#include  <internal_volume_io.h>
#include  <vols.h>
#include  <geom.h>

private  Real  calculate_weight(
    int      x,
    Real     dx,
    Real     x_min,
    Real     x_max );

public  Volume  smooth_resample_volume(
    Volume              volume,
    int                 new_nx,
    int                 new_ny,
    int                 new_nz )
{
    Volume             resampled_volume;
    int                sizes[MAX_DIMENSIONS], new_sizes[MAX_DIMENSIONS];
    int                dest_voxel[N_DIMENSIONS], src_voxel[N_DIMENSIONS], c;
    Real               x_min, x_max, y_min, y_max, z_min, z_max;
    Real               separations[N_DIMENSIONS];
    Real               dx, dy, dz;
    Real               voxel;
    Real               x_weight, xy_weight, weight;
    Real               *y_weights, *z_weights;
    Real               val;
    BOOLEAN            voxel_valid;
    Transform          scale_transform, translation_transform, transform;
    General_transform  general_transform, tmp;
    progress_struct    progress;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error( "smooth_resample_volume: volume must be 3D.\n" );
    }

    get_volume_sizes( volume, sizes );

    new_sizes[X] = new_nx;
    new_sizes[Y] = new_ny;
    new_sizes[Z] = new_nz;

    for_less( c, 0, N_DIMENSIONS )
        if( new_sizes[c] <= 0 )
            new_sizes[c] = sizes[c];

    resampled_volume = create_volume( 3, volume->dimension_names,
                                      volume->nc_data_type,
                                      volume->signed_flag,
                                      get_volume_voxel_min(volume),
                                      get_volume_voxel_max(volume) );

    set_volume_sizes( resampled_volume, new_sizes );

    set_volume_real_range( resampled_volume, get_volume_real_min(volume),
                           get_volume_real_max(volume) );

    dx = (Real) sizes[X] / (Real) new_sizes[X];
    dy = (Real) sizes[Y] / (Real) new_sizes[Y];
    dz = (Real) sizes[Z] / (Real) new_sizes[Z];

    get_volume_separations( volume, separations );

    separations[X] *= dx;
    separations[Y] *= dy;
    separations[Z] *= dz;

    set_volume_separations( resampled_volume, separations );

    make_translation_transform( dx / 2.0 - 0.5, dy / 2.0 - 0.5, dz / 2.0 - 0.5,
                                &translation_transform );
    make_scale_transform( dx, dy, dz, &scale_transform );

    concat_transforms( &transform, &scale_transform, &translation_transform );

    create_linear_transform( &tmp, &transform );

    concat_general_transforms( &tmp, get_voxel_to_world_transform(volume),
                               &general_transform );
    delete_general_transform( &tmp );

    set_voxel_to_world_transform( resampled_volume, &general_transform );

    alloc_volume_data( resampled_volume );

    ALLOC( y_weights, (int) dy + 2 );
    ALLOC( z_weights, (int) dz + 2 );

    initialize_progress_report( &progress, FALSE, new_nx * new_ny,
                                "Resampling" );

    for_less( dest_voxel[X], 0, new_nx )
    {
        x_min = (Real)   dest_voxel[X]    * dx;
        x_max = (Real)  (dest_voxel[X]+1) * dx;

        for_less( dest_voxel[Y], 0, new_ny )
        {
            y_min = (Real)  dest_voxel[Y]    * dy;
            y_max = (Real) (dest_voxel[Y]+1) * dy;

            for_less( dest_voxel[Z], 0, new_nz )
            {
                z_min = (Real)  dest_voxel[Z]    * dz;
                z_max = (Real) (dest_voxel[Z]+1) * dz;

                for_inclusive( src_voxel[Y], (int) y_min, (int) y_max )
                {
                    y_weights[src_voxel[Y]-(int)y_min] =
                        calculate_weight( src_voxel[Y], dy, y_min, y_max );
                }

                for_inclusive( src_voxel[Z], (int) z_min, (int) z_max )
                {
                    z_weights[src_voxel[Z]-(int)z_min] =
                        calculate_weight( src_voxel[Z], dz, z_min, z_max );
                }

                val = 0.0;

                voxel_valid = FALSE;

                for_inclusive( src_voxel[X], (int) x_min, (int) x_max )
                {
                    x_weight = calculate_weight( src_voxel[X], dx,
                                                 x_min, x_max );

                    for_inclusive( src_voxel[Y], (int) y_min, (int) y_max )
                    {
                        xy_weight = x_weight *
                                    y_weights[src_voxel[Y]-(int)y_min];

                        for_inclusive( src_voxel[Z], (int) z_min, (int) z_max )
                        {
                            weight = xy_weight *
                                     z_weights[src_voxel[Z]-(int)z_min];

                            if( weight > 0.0 )
                            {
                                GET_VOXEL_3D( voxel, volume,
                                              src_voxel[X], src_voxel[Y],
                                              src_voxel[Z]);
                                val += weight * voxel;
                                if( get_voxel_activity_flag(volume,src_voxel) )
                                    voxel_valid = TRUE;
                            }
                        }
                    }
                }

                SET_VOXEL_3D( resampled_volume,
                              dest_voxel[X], dest_voxel[Y], dest_voxel[Z],
                              val + 0.5 );

                if( !voxel_valid )
                    set_voxel_activity_flag( resampled_volume, dest_voxel,
                                             voxel_valid );
            }

            update_progress_report( &progress,
                                   dest_voxel[X] * new_ny + dest_voxel[Y] + 1 );
        }
    }

    terminate_progress_report( &progress );

    FREE( y_weights );
    FREE( z_weights );

    return( resampled_volume );
}

private  Real  calculate_weight(
    int      x,
    Real     dx,
    Real     x_min,
    Real     x_max )
{
    Real   start, end, weight;

    start = MAX( x_min, (Real) x );
    end = MIN( x_max, (Real) (x+1) );

    if( end < start || end - start > 1.0 )
    {
        handle_internal_error( "calculate_weight" );
    }

    weight = (end - start) / dx;

    return( weight );
}
