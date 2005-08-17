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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/change_labels.c,v 1.10 2005-08-17 22:26:19 bert Exp $";
#endif

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
    Volume   volume,
    Volume   label_volume,
    int      src_min,
    int      src_max,
    int      dest_label,
    Real     min_threshold,
    Real     max_threshold,
    int      range_changed[2][N_DIMENSIONS] )
{
    int              voxel[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS], dim;
    int              label;
    BOOLEAN          must_change, first;
    Real             value;
    progress_struct  progress;

    get_volume_sizes( label_volume, sizes );

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Modifying Labels" );

    first = TRUE;

    for_less( voxel[X], 0, sizes[X] )
    {
        for_less( voxel[Y], 0, sizes[Y] )
        {
            for_less( voxel[Z], 0, sizes[Z] )
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
                    value = get_volume_real_value( volume, voxel[X], voxel[Y],
                                                   voxel[Z], 0, 0 );
                    if( value < min_threshold || value > max_threshold )
                        must_change = FALSE;
                }

                if( must_change )
                {
                    set_volume_label_data( label_volume, voxel, dest_label );
                    for_less( dim, 0, N_DIMENSIONS )
                    {
                        if( first || voxel[dim] < range_changed[0][dim] )
                            range_changed[0][dim] = voxel[dim];
                        if( first || voxel[dim] > range_changed[1][dim] )
                            range_changed[1][dim] = voxel[dim];
                    }
                    first = FALSE;
                }
            }

            update_progress_report( &progress, voxel[X] * sizes[Y] +
                                    voxel[Y] + 1 );
        }
    }

    terminate_progress_report( &progress );
}
