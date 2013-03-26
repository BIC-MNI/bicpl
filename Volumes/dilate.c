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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/dilate.c,v 1.14 2005-08-17 22:26:19 bert Exp $";
#endif

typedef enum { NOT_INVOLVED, INSIDE_REGION, CANDIDATE }
             Voxel_classes;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dilate_voxels_3d
@INPUT      : volume
              label_volume
              min_inside_label
              max_inside_label
              min_inside_value
              max_inside_value
              min_outside_label
              max_outside_label
              min_outside_value
              max_outside_value
              new_label
              connectivity
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Dilates the label volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI int  dilate_voxels_3d(
    VIO_Volume          volume,
    VIO_Volume          label_volume,
    VIO_Real            min_inside_label,
    VIO_Real            max_inside_label,
    VIO_Real            min_inside_value,
    VIO_Real            max_inside_value,
    VIO_Real            min_outside_label,
    VIO_Real            max_outside_label,
    VIO_Real            min_outside_value,
    VIO_Real            max_outside_value,
    VIO_Real            new_label,
    Neighbour_types connectivity,
    int             range_changed[2][VIO_N_DIMENSIONS] )
{
    int                     n_changed;
    int                     x, y, z, delta_x, tx, ty, tz;
    int                     sizes[VIO_N_DIMENSIONS];
    int                     dir, n_dirs, *dx, *dy, *dz;
    VIO_Real                    value, label, *value_row, *label_row;
    VIO_SCHAR            **voxel_classes[3], **swap;
    progress_struct         progress;
    Voxel_classes           voxel_class;
    VIO_BOOL                 use_label_volume, use_volume, at_end, at_edge_y;
    VIO_BOOL                 inside_specified, outside_specified;
    VIO_BOOL                 inside, outside;

    use_label_volume = (min_inside_label <= max_inside_label ||
                        min_outside_label <= max_outside_label);
    use_volume = (min_inside_value <= max_inside_value ||
                  min_outside_value <= max_outside_value);

    inside_specified = (min_inside_label <= max_inside_label ||
                        min_inside_value <= max_inside_value);
    outside_specified = (min_outside_label <= max_outside_label ||
                         min_outside_value <= max_outside_value);

    if( !inside_specified && !outside_specified )
    {
        min_inside_label = new_label;
        max_inside_label = new_label;
        inside_specified = TRUE;
        use_label_volume = TRUE;
    }

    n_dirs = get_3D_neighbour_directions( connectivity, &dx, &dy, &dz );

    get_volume_sizes( label_volume, sizes );

    for_less( x, 0, 3 )
        VIO_ALLOC2D( voxel_classes[x], sizes[Y]+2, sizes[Z]+2 );

    for_less( x, 0, 1 )
    {
        for_less( y, 0, sizes[Y] + 2 )
        {
            for_less( z, 0, sizes[Z] + 2 )
                voxel_classes[x][y][z] = (VIO_SCHAR) NOT_INVOLVED;
        }
    }

    ALLOC( value_row, sizes[Z] );
    ALLOC( label_row, sizes[Z] );

    initialize_progress_report( &progress, FALSE, sizes[X],
                                "Expanding labeled voxels" );

    n_changed = 0;

    for_less( x, 0, sizes[X] )
    {
        for_less( delta_x, (x == 0) ? 0 : 1, 2 )
        {
            at_end = (x + delta_x == sizes[X]);

            for_less( y, -1, sizes[Y] + 1 )
            {
                at_edge_y = (y == -1 || y == sizes[Y]);
                voxel_classes[delta_x+1][y+1][0] = (VIO_SCHAR) NOT_INVOLVED;
                voxel_classes[delta_x+1][y+1][sizes[Z]+1] = NOT_INVOLVED;

                if( !at_edge_y && !at_end )
                {
                    if( use_label_volume )
                    {
                        get_volume_value_hyperslab_3d( label_volume,
                                                       x + delta_x, y, 0,
                                                       1, 1, sizes[Z],
                                                       label_row );
                    }

                    if( use_volume )
                    {
                        get_volume_value_hyperslab_3d( volume,
                                                       x + delta_x, y, 0,
                                                       1, 1, sizes[Z],
                                                       value_row );
                    }
                }

                for_less( z, 0, sizes[Z] )
                {
                    if( at_edge_y || at_end )
                    {
                        voxel_class = NOT_INVOLVED;
                    }
                    else
                    {
                        if( use_label_volume )
                            label = label_row[z];

                        if( use_volume )
                            value = value_row[z];

                        inside = (min_inside_label > max_inside_label ||
                                  (min_inside_label <= label &&
                                  label <= max_inside_label))           &&
                                 (min_inside_value > max_inside_value ||
                                  (min_inside_value <= value &&
                                  value <= max_inside_value));

                        outside = (min_outside_label > max_outside_label ||
                                   (min_outside_label <= label &&
                                   label <= max_outside_label))           &&
                                  (min_outside_value > max_outside_value ||
                                   (min_outside_value <= value &&
                                   value <= max_outside_value));

                        if( inside_specified )
                        {
                            if( inside )
                                voxel_class = INSIDE_REGION;
                            else if( outside )
                                voxel_class = CANDIDATE;
                            else
                                voxel_class = NOT_INVOLVED;
                        }
                        else
                        {
                            if( outside )
                                voxel_class = CANDIDATE;
                            else
                                voxel_class = INSIDE_REGION;
                        }
                    }

                    voxel_classes[delta_x+1][y+1][z+1] =
                                                  (VIO_SCHAR) voxel_class;
                }
            }
        }

        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                if( voxel_classes[1][y+1][z+1] == CANDIDATE )
                {
                    for_less( dir, 0, n_dirs )
                    {
                        tx = 0 + dx[dir] + 1;
                        ty = y + dy[dir] + 1;
                        tz = z + dz[dir] + 1;

                        if( voxel_classes[tx][ty][tz] == INSIDE_REGION )
                        {
                            set_volume_real_value( label_volume, x, y, z, 0, 0,
                                                   new_label );

                            if( n_changed == 0 || x < range_changed[0][X] )
                                range_changed[0][X] = x;
                            if( n_changed == 0 || x > range_changed[1][X] )
                                range_changed[1][X] = x;

                            if( n_changed == 0 || y < range_changed[0][Y] )
                                range_changed[0][Y] = y;
                            if( n_changed == 0 || y > range_changed[1][Y] )
                                range_changed[1][Y] = y;

                            if( n_changed == 0 || z < range_changed[0][Z] )
                                range_changed[0][Z] = z;
                            if( n_changed == 0 || z > range_changed[1][Z] )
                                range_changed[1][Z] = z;

                            ++n_changed;
                            break;
                        }
                    }
                }
            }
        }

        swap = voxel_classes[0];
        voxel_classes[0] = voxel_classes[1];
        voxel_classes[1] = voxel_classes[2];
        voxel_classes[2] = swap;

        update_progress_report( &progress, x + 1 );
    }

    terminate_progress_report( &progress );

    for_less( x, 0, 3 )
        VIO_FREE2D( voxel_classes[x] );

    FREE( value_row );
    FREE( label_row );

    return( n_changed );
}
