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

#include  <internal_volume_io.h>
#include  <bicpl.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_markers.c,v 1.5 1996-05-17 19:35:53 david Exp $";
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

public  void  scan_marker_to_voxels(
    marker_struct    *marker,
    Volume           volume,
    Volume           label_volume,
    int              label )
{
    Real           low[N_DIMENSIONS], high[N_DIMENSIONS];
    int            min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    Real           voxel[N_DIMENSIONS], min_v, max_v;
    int            c, int_voxel[N_DIMENSIONS];

    convert_world_to_voxel( volume,
                       (Real) Point_x(marker->position) - (Real) marker->size,
                       (Real) Point_y(marker->position) - (Real) marker->size,
                       (Real) Point_z(marker->position) - (Real) marker->size,
                       low );

    convert_world_to_voxel( volume,
                       (Real) Point_x(marker->position) + (Real) marker->size,
                       (Real) Point_y(marker->position) + (Real) marker->size,
                       (Real) Point_z(marker->position) + (Real) marker->size,
                       high );

    for_less( c, 0, N_DIMENSIONS )
    {
        min_v = MIN( low[c], high[c] );
        max_v = MAX( low[c], high[c] );

        min_voxel[c] = FLOOR( min_v + 0.5 );
        max_voxel[c] = FLOOR( max_v + 0.5 );
    }

    for_inclusive( int_voxel[X], min_voxel[X], max_voxel[X] )
    {
        for_inclusive( int_voxel[Y], min_voxel[Y], max_voxel[Y] )
        {
            for_inclusive( int_voxel[Z], min_voxel[Z], max_voxel[Z] )
            {
                convert_int_to_real_voxel( N_DIMENSIONS, int_voxel, voxel );
                if( voxel_is_within_volume( volume, voxel ) )
                {
                    set_volume_label_data( label_volume, int_voxel, label );
                }
            }
        }
    }
}
