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
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Volumes/scan_markers.c,v 1.8 2005-08-17 22:26:19 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_marker_to_voxels
@INPUT      : marker
              volume
              label_volume
              label
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Scans a marker to a label volume.  Simply treats the marker
              as a rectangular box.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  scan_marker_to_voxels(
    marker_struct    *marker,
    VIO_Volume           volume,
    VIO_Volume           label_volume,
    int              label )
{
    VIO_Real           low[VIO_N_DIMENSIONS], high[VIO_N_DIMENSIONS];
    int            min_voxel[VIO_N_DIMENSIONS], max_voxel[VIO_N_DIMENSIONS];
    VIO_Real           voxel[VIO_N_DIMENSIONS], min_v, max_v;
    int            c, int_voxel[VIO_N_DIMENSIONS];

    convert_world_to_voxel( volume,
                       (VIO_Real) Point_x(marker->position) - (VIO_Real) marker->size,
                       (VIO_Real) Point_y(marker->position) - (VIO_Real) marker->size,
                       (VIO_Real) Point_z(marker->position) - (VIO_Real) marker->size,
                       low );

    convert_world_to_voxel( volume,
                       (VIO_Real) Point_x(marker->position) + (VIO_Real) marker->size,
                       (VIO_Real) Point_y(marker->position) + (VIO_Real) marker->size,
                       (VIO_Real) Point_z(marker->position) + (VIO_Real) marker->size,
                       high );

    for_less( c, 0, VIO_N_DIMENSIONS )
    {
        min_v = MIN( low[c], high[c] );
        max_v = MAX( low[c], high[c] );

        min_voxel[c] = VIO_FLOOR( min_v + 0.5 );
        max_voxel[c] = VIO_FLOOR( max_v + 0.5 );
    }

    for_inclusive( int_voxel[VIO_X], min_voxel[VIO_X], max_voxel[VIO_X] )
    {
        for_inclusive( int_voxel[VIO_Y], min_voxel[VIO_Y], max_voxel[VIO_Y] )
        {
            for_inclusive( int_voxel[VIO_Z], min_voxel[VIO_Z], max_voxel[VIO_Z] )
            {
                convert_int_to_real_voxel( VIO_N_DIMENSIONS, int_voxel, voxel );
                if( voxel_is_within_volume( volume, voxel ) )
                {
                    set_volume_label_data( label_volume, int_voxel, label );
                }
            }
        }
    }
}
