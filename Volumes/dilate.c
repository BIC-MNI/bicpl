#include  <bicpl.h>

public  void  dilate_labeled_voxels_3d(
    Volume          volume,
    Volume          label_volume,
    int             min_inside_label,
    int             max_inside_label,
    Real            min_inside_value,
    Real            max_inside_value,
    int             min_outside_label,
    int             max_outside_label,
    Real            min_outside_value,
    Real            max_outside_value,
    int             new_label,
    Neighbour_types connectivity )
{
    int                     x, y, z;
    int                     v[N_DIMENSIONS];
    int                     voxel[N_DIMENSIONS], sizes[N_DIMENSIONS];
    int                     dir, n_dirs, *dx, *dy, *dz, label;
    Real                    val;
    BOOLEAN                 changed;
    bitlist_3d_struct       inside_bits, outside_bits;
    progress_struct         progress;

    if( min_inside_label <= max_inside_label &&
        (new_label < min_inside_label || new_label > max_inside_label ) ||
        min_inside_label > max_inside_label &&
        min_outside_label > max_outside_label )
    {
        print( "Invalid parameters for dilation.\n" );
        return;
    }

    n_dirs = get_3D_neighbour_directions( connectivity, &dx, &dy, &dz );

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[X]+2, sizes[Y]+2, sizes[Z]+2, &inside_bits );
    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &outside_bits );

    if( min_inside_value <= max_inside_value ||
        min_outside_value <= max_outside_value )
    {
        for_less( x, 0, sizes[X] )
        {
            for_less( y, 0, sizes[Y] )
            {
                for_less( z, 0, sizes[Z] )
                {
                    GET_VALUE_3D( val, volume, x, y, z );
                    if( min_inside_value <= max_inside_value &&
                        min_inside_value <= val && val <= max_inside_value )
                    {
                        set_bitlist_bit_3d( &inside_bits, x+1, y+1, z+1, TRUE);
                    }
                    if( min_outside_value <= max_outside_value &&
                        min_outside_value <= val && val <= max_outside_value )
                    {
                        set_bitlist_bit_3d( &outside_bits, x, y, z, TRUE);
                    }
                }
            }
        }
    }

    if( min_outside_value > max_outside_value )
        fill_bitlist_3d( &outside_bits );

    if( min_inside_value > max_inside_value )
    {
        fill_bitlist_3d( &inside_bits );

        for_less( x, -1, sizes[X]+1 )
        {
            for_less( y, -1, sizes[Y]+1 )
            {
                if( x == -1 || x == sizes[X] || y == -1 || y == sizes[Y] )
                {
                    for_less( z, -1, sizes[Z]+1 )
                        set_bitlist_bit_3d( &inside_bits, x+1, y+1, z+1,
                                            FALSE );
                }
                else
                {
                    set_bitlist_bit_3d( &inside_bits, x+1, y+1, -1+1, FALSE );
                    set_bitlist_bit_3d( &inside_bits, x+1, y+1, sizes[Z]+1,
                                        FALSE);
                }
            }
        }
    }

    for_less( voxel[X], 0, sizes[X] )
    {
        for_less( voxel[Y], 0, sizes[Y] )
        {
            for_less( voxel[Z], 0, sizes[Z] )
            {
                if( get_bitlist_bit_3d( &inside_bits, voxel[X]+1, voxel[Y]+1,
                                        voxel[Z]+1 ) )
                {
                    label = get_volume_label_data( label_volume, voxel );

                    if( min_inside_label <= max_inside_label &&
                        (label < min_inside_label || label > max_inside_label)
                                     ||
                        min_inside_label > max_inside_label &&
                        label >= min_outside_label &&
                        label <= max_outside_label )
                    {
                        set_bitlist_bit_3d( &inside_bits, voxel[X]+1,
                                            voxel[Y]+1, voxel[Z]+1, FALSE );
                    }

                    if( min_outside_label <= max_outside_label &&
                        (label < min_outside_label || label > max_outside_label)
                                          ||
                        min_outside_label > max_outside_label &&
                        label >= min_inside_label &&
                        label <= max_inside_label )
                    {
                        set_bitlist_bit_3d( &outside_bits, voxel[X], voxel[Y],
                                            voxel[Z], FALSE );
                    }
                }
            }
        }
    }

    changed = FALSE;

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Expanding labeled voxels" );

    for_less( v[X], 0, sizes[X] )
    {
        for_less( v[Y], 0, sizes[Y] )
        {
            for_less( v[Z], 0, sizes[Z] )
            {
                if( get_bitlist_bit_3d( &outside_bits, v[X], v[Y], v[Z] ) )
                {
                    for_less( dir, 0, n_dirs )
                    {
                        voxel[X] = v[X] + dx[dir];
                        voxel[Y] = v[Y] + dy[dir];
                        voxel[Z] = v[Z] + dz[dir];

                        if( get_bitlist_bit_3d( &inside_bits,
                                        voxel[X]+1, voxel[Y]+1, voxel[Z]+1 ) )
                        {
                            set_volume_label_data( label_volume, v,
                                                   new_label );
                            changed = TRUE;
                            break;
                        }
                    }
                }
            }

            update_progress_report( &progress, v[Y]+1+v[X]*sizes[Y] );
        }
    }

    terminate_progress_report( &progress );

    delete_bitlist_3d( &inside_bits );
    delete_bitlist_3d( &outside_bits );
}
