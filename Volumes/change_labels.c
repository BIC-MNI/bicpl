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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : modify_labels_in_range
@INPUT      : volume
              src_min
              src_max
              dest_label
              min_threshold
              max_threshold
@OUTPUT     : label_volume
@RETURNS    : 
@DESCRIPTION: Changes all labels which have the value within src_min and
              src_max and a
              volume value within the threshold, to the dest_label.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  modify_labels_in_range(
    VIO_Volume   volume,
    VIO_Volume   label_volume,
    int      src_min,
    int      src_max,
    int      dest_label,
    VIO_Real     min_threshold,
    VIO_Real     max_threshold,
    int      range_changed[2][VIO_N_DIMENSIONS] )
{
    int              voxel[VIO_MAX_DIMENSIONS], sizes[VIO_MAX_DIMENSIONS], dim;
    int              label;
    VIO_BOOL          must_change, first;
    VIO_Real             value;
    VIO_progress_struct  progress;

    get_volume_sizes( label_volume, sizes );

    initialize_progress_report( &progress, FALSE, sizes[VIO_X] * sizes[VIO_Y],
                                "Modifying Labels" );

    first = TRUE;

    for_less( voxel[VIO_X], 0, sizes[VIO_X] )
    {
        for_less( voxel[VIO_Y], 0, sizes[VIO_Y] )
        {
            for_less( voxel[VIO_Z], 0, sizes[VIO_Z] )
            {
                if( src_min <= src_max )
                {
                    label = get_volume_label_data( label_volume, voxel );
                    must_change = (src_min <= label && label <= src_max);
                }
                else
                    must_change = TRUE;

                if( must_change && min_threshold < max_threshold )
                {
                    value = get_volume_real_value( volume, voxel[VIO_X], voxel[VIO_Y],
                                                   voxel[VIO_Z], 0, 0 );
                    if( value < min_threshold || value > max_threshold )
                        must_change = FALSE;
                }

                if( must_change )
                {
                    set_volume_label_data( label_volume, voxel, dest_label );
                    for_less( dim, 0, VIO_N_DIMENSIONS )
                    {
                        if( first || voxel[dim] < range_changed[0][dim] )
                            range_changed[0][dim] = voxel[dim];
                        if( first || voxel[dim] > range_changed[1][dim] )
                            range_changed[1][dim] = voxel[dim];
                    }
                    first = FALSE;
                }
            }

            update_progress_report( &progress, voxel[VIO_X] * sizes[VIO_Y] +
                                    voxel[VIO_Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}
