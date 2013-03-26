/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/crop_volume.c,v 1.10 2005-08-17 22:26:19 bert Exp $";
#endif

#define  MAX_BUFFER_SIZE  100000

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_volume_crop_bounds
@INPUT      : volume
              min_crop_threshold
              max_crop_threshold
@OUTPUT     : limits
@RETURNS    : TRUE if volume can be cropped
@DESCRIPTION: Checks if the volume can be cropped, where values between
              min_crop_threshold and max_crop_threshold can be cropped.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1995    David MacDonald
@MODIFIED   : Aug. 1, 1995    D. MacDonald   - made it faster
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  find_volume_crop_bounds(
    VIO_Volume          volume,
    VIO_Real            min_crop_threshold,
    VIO_Real            max_crop_threshold,
    int             limits[2][VIO_MAX_DIMENSIONS] )
{
    int      dim, n_dims, lim, voxel[VIO_MAX_DIMENSIONS], sizes[VIO_MAX_DIMENSIONS];
    int      start, end, step, voxel_pos, new_limits[2];
    int      start0, start1, start2, start3, start4;
    int      end0, end1, end2, end3, end4;
    VIO_Real     value;
    VIO_BOOL  found;

    n_dims = get_volume_n_dimensions( volume );
    get_volume_sizes( volume, sizes );

    for_less( dim, 0, VIO_MAX_DIMENSIONS )
    {
        limits[0][dim] = 0;

        if( dim < n_dims )
            limits[1][dim] = sizes[dim]-1;
        else
            limits[1][dim] = 0;
    }

    for_less( dim, 0, n_dims )
    {
        limits[0][dim] = 0;
        limits[1][dim] = 0;

        start0 = limits[0][0];
        start1 = limits[0][1];
        start2 = limits[0][2];
        start3 = limits[0][3];
        start4 = limits[0][4];
        end0 = limits[1][0];
        end1 = limits[1][1];
        end2 = limits[1][2];
        end3 = limits[1][3];
        end4 = limits[1][4];

        for_less( lim, 0, 2 )
        {
            if( lim == 0 )
            {
                start = 0;
                end = sizes[dim];
                step = 1;
            }
            else
            {
                start = sizes[dim]-1;
                end = -1;
                step = -1;
            }

            found = FALSE;

            for( voxel_pos = start;  voxel_pos != end;  voxel_pos += step )
            {
                for_inclusive( voxel[0], start0, end0 )
                {
                 for_inclusive( voxel[1], start1, end1 )
                 {
                  for_inclusive( voxel[2], start2, end2 )
                  {
                   for_inclusive( voxel[3], start3, end3 )
                   {
                    for_inclusive( voxel[4], start4, end4 )
                    {
                        voxel[dim] = voxel_pos;

                        value = get_volume_real_value( volume,
                                               voxel[0], voxel[1],
                                               voxel[2], voxel[3], voxel[4] );

                        if( value < min_crop_threshold ||
                            value > max_crop_threshold )
                        {
                            found = TRUE;
                            break;
                        }
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

            new_limits[lim] = voxel_pos;
        }

        limits[0][dim] = new_limits[0];
        limits[1][dim] = new_limits[1];
    }

    return( limits[0][X] <= limits[1][X] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_cropped_volume
@INPUT      : volume
              limits
@OUTPUT     : 
@RETURNS    : cropped volume
@DESCRIPTION: Crops a volume to the specified limits.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_Volume  create_cropped_volume(
    VIO_Volume          volume,
    int             limits[2][VIO_MAX_DIMENSIONS] )
{
    int                dim, n_dims;
    int                sizes[VIO_MAX_DIMENSIONS], new_sizes[VIO_MAX_DIMENSIONS];
    int                v0, v1, v2, v3, v4, offset[VIO_MAX_DIMENSIONS];
    int                start[VIO_MAX_DIMENSIONS], end[VIO_MAX_DIMENSIONS];
    nc_type            nc_data_type;
    VIO_Real               separations[VIO_MAX_DIMENSIONS];
    VIO_Real               start_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real               xyz[VIO_N_DIMENSIONS], voxel_value;
    VIO_Real               min_voxel, max_voxel;
    VIO_BOOL            signed_flag, is_fully_inside;
    STRING             *dim_names;
    VIO_Volume             cropped_volume;
    VIO_General_transform  cropped_transform, offset_transform;
    VIO_Transform          translation;

    n_dims = get_volume_n_dimensions( volume );
    get_volume_sizes( volume, sizes );

    dim_names = get_volume_dimension_names( volume );

    nc_data_type = get_volume_nc_data_type( volume, &signed_flag );
    get_volume_voxel_range( volume, &min_voxel, &max_voxel );

    cropped_volume = create_volume( n_dims, dim_names,
                                    nc_data_type, signed_flag,
                                    min_voxel, max_voxel );

    delete_dimension_names( volume, dim_names );

    is_fully_inside = TRUE;

    for_less( dim, 0, n_dims )
    {
        new_sizes[dim] = limits[1][dim] - limits[0][dim] + 1;

        if( limits[0][dim] < 0 || limits[1][dim] >= sizes[dim] )
            is_fully_inside = FALSE;
    }

    set_volume_sizes( cropped_volume, new_sizes );
    alloc_volume_data( cropped_volume );

    set_volume_real_range( cropped_volume,
                           get_volume_real_min(volume),
                           get_volume_real_max(volume) );

    get_volume_separations( volume, separations );
    set_volume_separations( cropped_volume, separations );

    for_less( dim, 0, n_dims )
        start_voxel[dim] = (VIO_Real) limits[0][dim];

    reorder_voxel_to_xyz( volume, start_voxel, xyz );

    make_translation_transform( xyz[X], xyz[Y], xyz[Z], &translation );
    create_linear_transform( &offset_transform, &translation );
    concat_general_transforms( &offset_transform,
                               get_voxel_to_world_transform(volume),
                               &cropped_transform );

    set_voxel_to_world_transform( cropped_volume, &cropped_transform );

    delete_general_transform( &offset_transform );

    for_less( dim, 0, n_dims )
    {
        offset[dim] = limits[0][dim];
        start[dim] = MAX( 0, -limits[0][dim] );
        end[dim] = MIN( new_sizes[dim]-1, sizes[dim] - 1 - limits[0][dim] );
    }

    for_less( dim, n_dims, VIO_MAX_DIMENSIONS )
    {
        start[dim] = 0;
        end[dim] = 0;
        offset[dim] = 0;
    }

    if( !is_fully_inside )
    {
        BEGIN_ALL_VOXELS( cropped_volume, v0, v1, v2, v3, v4 )
            set_volume_real_value( cropped_volume, v0, v1, v2, v3, v4, 0.0 );
        END_ALL_VOXELS
    }

    for_inclusive( v0, start[0], end[0] )
    for_inclusive( v1, start[1], end[1] )
    for_inclusive( v2, start[2], end[2] )
    for_inclusive( v3, start[3], end[3] )
    for_inclusive( v4, start[4], end[4] )
    {
        voxel_value = get_volume_voxel_value( volume,
                                    v0 + offset[0], v1 + offset[1],
                                    v2 + offset[2], v3 + offset[3],
                                    v4 + offset[4] );
        set_volume_voxel_value( cropped_volume, v0, v1, v2, v3, v4,
                                voxel_value );
    }

    return( cropped_volume );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : autocrop_volume
@INPUT      : volume
@OUTPUT     :
@RETURNS    : a volume
@DESCRIPTION: Creates a new volume which is the input volume cropped down
              to exclude regions containing only zero values.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : Dec.  8, 1995    David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

BICAPI VIO_Volume  autocrop_volume(
    VIO_Volume    volume )
{
    VIO_Volume  cropped;
    int     dim, limits[2][VIO_MAX_DIMENSIONS];

    if( !find_volume_crop_bounds( volume, 0.0, 0.0, limits ) )
    {
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            limits[0][dim] = 0;
            limits[1][dim] = 0;
        }                     
    }

    cropped = create_cropped_volume( volume, limits );

    return( cropped );
}
