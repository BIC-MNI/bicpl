#include  <internal_volume_io.h>
#include  <bicpl.h>

public  BOOLEAN  find_volume_crop_bounds(
    Volume          volume,
    Real            min_crop_threshold,
    Real            max_crop_threshold,
    int             limits[2][MAX_DIMENSIONS] )
{
    int      d, dim, n_dims, lim, voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];
    int      dim_size, start, end, step, voxel_pos;
    Real     value;
    BOOLEAN  found;

    n_dims = get_volume_n_dimensions( volume );

    for_less( dim, 0, n_dims )
    {
        get_volume_sizes( volume, sizes );
        for_less( d, n_dims, MAX_DIMENSIONS )
            sizes[d] = 1;
        dim_size = sizes[dim];
        sizes[dim] = 1;

        for_less( lim, 0, 2 )
        {
            if( lim == 0 )
            {
                start = 0;
                end = dim_size;
                step = 1;
            }
            else
            {
                start = dim_size-1;
                end = -1;
                step = -1;
            }

            found = FALSE;

            for( voxel_pos = start;  voxel_pos != end;  voxel_pos += step )
            {
                for_less( voxel[4], 0, sizes[4] )
                {
                 for_less( voxel[3], 0, sizes[3] )
                 {
                  for_less( voxel[2], 0, sizes[2] )
                  {
                   for_less( voxel[1], 0, sizes[1] )
                   {
                    for_less( voxel[0], 0, sizes[0] )
                    {
                        voxel[dim] = voxel_pos;

                        value = get_volume_real_value( volume,
                                           voxel[0], voxel[1],
                                           voxel[2], voxel[3], voxel[4]);

                        if( value < min_crop_threshold ||
                            value > max_crop_threshold )
                        {
                            found = TRUE;
                            break;
                        }

                        voxel[dim] = 0;
                    }
                    if( found )
                        break;
                   }
                   if( found )
                       break;
                  }
                  if( found )
                      break;
                 }
                 if( found )
                     break;
                }

                if( found )
                    break;
            }

            limits[lim][dim] = voxel_pos;
        }
    }

    return( limits[0][X] < limits[1][X] );
}

public  Volume  create_cropped_volume(
    Volume          volume,
    int             limits[2][MAX_DIMENSIONS] )
{
    int                dim, n_dims;
    int                sizes[MAX_DIMENSIONS], new_sizes[MAX_DIMENSIONS];
    int                v0, v1, v2, v3, v4, offset[MAX_DIMENSIONS];
    int                offset0, offset1, offset2, offset3, offset4;
    nc_type            nc_data_type;
    Real               separations[MAX_DIMENSIONS];
    Real               start_voxel[MAX_DIMENSIONS];
    Real               xyz[N_DIMENSIONS], voxel_value;
    Real               min_voxel, max_voxel;
    BOOLEAN            signed_flag;
    char               **dim_names;
    Volume             cropped_volume;
    General_transform  cropped_transform, offset_transform;
    Transform          translation;

    n_dims = get_volume_n_dimensions( volume );
    get_volume_sizes( volume, sizes );

    dim_names = get_volume_dimension_names( volume );

    nc_data_type = get_volume_nc_data_type( volume, &signed_flag );
    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    cropped_volume = create_volume( n_dims, dim_names,
                                    nc_data_type, signed_flag,
                                    min_voxel, max_voxel );

    delete_dimension_names( dim_names );

    for_less( dim, 0, n_dims )
        new_sizes[dim] = limits[1][dim] - limits[0][dim] + 1;

    set_volume_sizes( cropped_volume, new_sizes );
    alloc_volume_data( cropped_volume );

    set_volume_real_range( cropped_volume,
                           get_volume_real_min(volume),
                           get_volume_real_max(volume) );

    get_volume_separations( volume, separations );
    set_volume_separations( cropped_volume, separations );

    for_less( dim, 0, n_dims )
        start_voxel[dim] = (Real) limits[0][dim];

    reorder_voxel_to_xyz( volume, start_voxel, xyz );

    make_translation_transform( xyz[X], xyz[Y], xyz[Z], &translation );
    create_linear_transform( &offset_transform, &translation );
    concat_general_transforms( &offset_transform,
                               get_voxel_to_world_transform(volume),
                               &cropped_transform );

    set_voxel_to_world_transform( cropped_volume, &cropped_transform );

    delete_general_transform( &offset_transform );

    for_less( dim, 0, n_dims )
        offset[dim] = limits[0][dim];

    for_less( dim, n_dims, MAX_DIMENSIONS )
        offset[dim] = 0;

    offset0 = offset[0];
    offset1 = offset[1];
    offset2 = offset[2];
    offset3 = offset[3];
    offset4 = offset[4];

    BEGIN_ALL_VOXELS( cropped_volume, v0, v1, v2, v3, v4 )
        voxel_value = get_volume_voxel_value( volume,
                                    v0 + offset0, v1 + offset1,
                                    v2 + offset2, v3 + offset3, v4 + offset4 );
        set_volume_voxel_value( cropped_volume, v0, v1, v2, v3, v4,
                                voxel_value );
    END_ALL_VOXELS

    return( cropped_volume );
}
