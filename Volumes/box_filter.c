
#include  <mni.h>

private  Real  get_amount_in_box(
    float     ***volume_cache,
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
private  void  do_yz_slice(
    Volume   resampled_volume,
    float    ***volume_cache,
    int      x,
    Real     x_min_voxel,
    Real     x_max_voxel,
    Real     x_min,
    Real     x_max,
    Real     x_half_width,
    Real     y_half_width,
    Real     z_half_width );

private  void  get_voxel_range(
    int      size,
    Real     min_pos,
    Real     max_pos,
    int      *min_voxel,
    int      *max_voxel,
    Real     *start_weight,
    Real     *end_weight )
{
    if( min_pos < -0.5 )
        min_pos = -0.5;
    if( max_pos > (Real) size - 0.5 )
        max_pos = (Real) size - 0.5;

    *min_voxel = ROUND( min_pos );
    *max_voxel = ROUND( max_pos );

    if( (Real) (*max_voxel) - 0.5 == max_pos )
        --(*max_voxel);

    if( *min_voxel == *max_voxel )
        *start_weight = max_pos - min_pos;
    else
    {
        *start_weight = ((Real) (*min_voxel) + 0.5) - min_pos;
        *end_weight = max_pos - ((Real) (*max_voxel) - 0.5);
    }
}
    

public  Volume  create_box_filtered_volume(
    Volume   volume,
    Real     x_width,
    Real     y_width,
    Real     z_width )
{
    Volume             resampled_volume;
    int                i, x, y, z, n_x_cached, x_cache_end;
    int                sizes[MAX_DIMENSIONS];
    Real               separations[MAX_DIMENSIONS];
    int                x_min_voxel, x_max_voxel, start;
    Real               x_weight_start, x_weight_end;
    float              ***volume_cache;
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

    x_width = x_width / 2.0 / separations[X];
    y_width = y_width / 2.0 / separations[Y];
    z_width = z_width / 2.0 / separations[Z];

    /* --- this is alloced in xzy order so that in the inner loop we can
           step through y */

    ALLOC( volume_cache, sizes[X] );

    n_x_cached = (int) (2.0 * x_width) + 2;

    for_less( x, 0, n_x_cached )
        ALLOC2D( volume_cache[x], sizes[Z], sizes[Y] );

    x_cache_end = -1;

    initialize_progress_report( &progress, FALSE, sizes[X], "Box Filtering" );

    for_less( x, 0, sizes[X] )
    {
        get_voxel_range( sizes[X], x - x_width, x + x_width,
                         &x_min_voxel, &x_max_voxel,
                         &x_weight_start, &x_weight_end );

        if( x_max_voxel > x_cache_end )
        {
            start = x_cache_end + 1;
            if( start < n_x_cached )
                start = n_x_cached;

            for_inclusive( i, start, x_max_voxel )
                volume_cache[i] = volume_cache[i-n_x_cached];

            for_inclusive( i, x_cache_end+1, x_max_voxel )
            {
                for_less( z, 0, sizes[Z] )
                {
                    for_less( y, 0, sizes[Y] )
                        GET_VOXEL_3D( volume_cache[i][z][y], volume, i, y, z );
                }
            }

            x_cache_end = x_max_voxel;
        }

        do_yz_slice( resampled_volume, volume_cache, x,
                     x_min_voxel, x_max_voxel, x_weight_start, x_weight_end,
                     x_width, y_width, z_width );

        update_progress_report( &progress, x + 1 );
    }

    terminate_progress_report( &progress );

    for_less( x, 0, n_x_cached )
        FREE2D( volume_cache[x] );

    FREE( volume_cache );

    return( resampled_volume );
}

private  void  do_yz_slice(
    Volume   resampled_volume,
    float    ***volume_cache,
    int      x,
    Real     x_min_voxel,
    Real     x_max_voxel,
    Real     x_weight_start,
    Real     x_weight_end,
    Real     x_half_width,
    Real     y_half_width,
    Real     z_half_width )
{
    int                y, z;
    int                sizes[MAX_DIMENSIONS];
    int                y_min_voxel, y_max_voxel;
    Real               z_min, z_max;
    Real               sum, voxel, sample_volume;
    Real               y_weight_start, y_weight_end;
    BOOLEAN            short_cut;
    Real               *z_weight_start_add, *z_weight_end_add;
    Real               *z_weight_start_sub, *z_weight_end_sub;
    int                *z_min_voxels_add, *z_max_voxels_add;
    int                *z_min_voxels_sub, *z_max_voxels_sub;
    Real               prev_z_min, prev_z_max, added, removed;

    get_volume_sizes( resampled_volume, sizes );

    sample_volume = 2.0 * x_half_width * 2.0 * y_half_width *
                    2.0 * z_half_width;

    if( z_half_width > 2.0 )
        short_cut = TRUE;
    else
        short_cut = FALSE;

    ALLOC( z_min_voxels_add, sizes[Z] );
    ALLOC( z_max_voxels_add, sizes[Z] );
    ALLOC( z_weight_start_add, sizes[Z] );
    ALLOC( z_weight_end_add, sizes[Z] );

    if( short_cut )
    {
        ALLOC( z_min_voxels_sub, sizes[Z] );
        ALLOC( z_max_voxels_sub, sizes[Z] );
        ALLOC( z_weight_start_sub, sizes[Z] );
        ALLOC( z_weight_end_sub, sizes[Z] );
    }

#ifdef lint
    z_min = 0.0;
    z_max = 0.0;
#endif

    for_less( z, 0, sizes[Z] )
    {
        prev_z_min = z_min;
        prev_z_max = z_max;

        z_min = z - z_half_width;
        z_max = z + z_half_width;

        if( !short_cut || z == 0 )
        {
            get_voxel_range( sizes[Z], z_min, z_max,
                             &z_min_voxels_add[z], &z_max_voxels_add[z],
                             &z_weight_start_add[z], &z_weight_end_add[z] );
        }
        else
        {
            get_voxel_range( sizes[Z], prev_z_max, z_max,
                             &z_min_voxels_add[z], &z_max_voxels_add[z],
                             &z_weight_start_add[z], &z_weight_end_add[z] );
            get_voxel_range( sizes[Z], prev_z_min, z_min,
                             &z_min_voxels_sub[z], &z_max_voxels_sub[z],
                             &z_weight_start_sub[z], &z_weight_end_sub[z] );
        }
    }

    for_less( y, 0, sizes[Y] )
    {
        get_voxel_range( sizes[Y], y - y_half_width, y + y_half_width,
                         &y_min_voxel, &y_max_voxel,
                         &y_weight_start, &y_weight_end );

        for_less( z, 0, sizes[Z] )
        {
            if( !short_cut || z == 0 )
            {
                sum = get_amount_in_box( volume_cache,
                                 x_min_voxel,           x_max_voxel,
                                 y_min_voxel,           y_max_voxel,
                                 z_min_voxels_add[z],   z_max_voxels_add[z],
                                 x_weight_start,        x_weight_end,
                                 y_weight_start,        y_weight_end,
                                 z_weight_start_add[z], z_weight_end_add[z] );
            }
            else
            {
                removed = get_amount_in_box( volume_cache,
                                 x_min_voxel,           x_max_voxel,
                                 y_min_voxel,           y_max_voxel,
                                 z_min_voxels_sub[z],   z_max_voxels_sub[z],
                                 x_weight_start,        x_weight_end,
                                 y_weight_start,        y_weight_end,
                                 z_weight_start_sub[z], z_weight_end_sub[z] );
                added = get_amount_in_box( volume_cache,
                                 x_min_voxel,           x_max_voxel,
                                 y_min_voxel,           y_max_voxel,
                                 z_min_voxels_add[z],   z_max_voxels_add[z],
                                 x_weight_start,        x_weight_end,
                                 y_weight_start,        y_weight_end,
                                 z_weight_start_add[z], z_weight_end_add[z] );

                sum += added - removed;
            }

            voxel = sum / sample_volume;

            SET_VOXEL_3D( resampled_volume, x, y, z, voxel );
        }
    }

    FREE( z_min_voxels_add );
    FREE( z_max_voxels_add );
    FREE( z_weight_start_add );
    FREE( z_weight_end_add );

    if( short_cut )
    {
        FREE( z_min_voxels_sub );
        FREE( z_max_voxels_sub );
        FREE( z_weight_start_sub );
        FREE( z_weight_end_sub );
    }
}

private  Real  get_amount_in_box(
    float     ***volume_cache,
    int       x_min_voxel,
    int       x_max_voxel,
    int       y_min_voxel,
    int       y_max_voxel,
    int       z_min_voxel,
    int       z_max_voxel,
    Real      x_weight_start,
    Real      x_weight_end,
    Real      y_weight_start,
    Real      y_weight_end,
    Real      z_weight_start,
    Real      z_weight_end )
{
    int     x, y, z;
    Real    sum, z_sum, x_sum, voxel;
    float   *column;

    sum = 0.0;

    for_inclusive( z, z_min_voxel, z_max_voxel )
    {
        z_sum = 0.0;

        for_inclusive( x, x_min_voxel, x_max_voxel )
        {
            column = volume_cache[x][z];

            x_sum = 0.0;

            for_inclusive( y, y_min_voxel, y_max_voxel )
            {
                voxel = column[y];

                if( y == y_min_voxel )
                    voxel *= y_weight_start;
                else if( y == y_max_voxel )
                    voxel *= y_weight_end;

                x_sum += voxel;
            }

            if( x == x_min_voxel )
                x_sum *= x_weight_start;
            else if( x == x_max_voxel )
                x_sum *= x_weight_end;

            z_sum += x_sum;
        }

        if( z == z_min_voxel )
            z_sum *= z_weight_start;
        else if( z == z_max_voxel )
            z_sum *= z_weight_end;

        sum += z_sum;
    }

    return( sum );
}
