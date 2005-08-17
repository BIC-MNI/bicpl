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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/fill_volume.c,v 1.11 2005-08-17 22:26:19 bert Exp $";
#endif

typedef struct
{
    int  x, y, z;
} xyz_struct;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fill_connected_voxels
@INPUT      : volume
              label_volume
              connectivity
              voxel
              min_label_threshold
              max_label_threshold
              desired_label
              min_threshold
              max_threshold
@OUTPUT     : 
@RETURNS    : TRUE if changed
@DESCRIPTION: Performs a 3D fill from the specified voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI BOOLEAN  fill_connected_voxels(
    Volume              volume,
    Volume              label_volume,
    Neighbour_types     connectivity,
    int                 voxel[],
    int                 min_label_threshold,
    int                 max_label_threshold,
    int                 desired_label,
    Real                min_threshold,
    Real                max_threshold,
    int                 range_changed[2][N_DIMENSIONS] )
{
    int                          dir, n_dirs, *dx, *dy, *dz, dim;
    int                          x, y, z, tx, ty, tz;
    int                          sizes[N_DIMENSIONS];
    int                          voxel_index[MAX_DIMENSIONS];
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            checked_flags, change_flags;
    BOOLEAN                      first;

    if( !should_change_this_one( volume, label_volume, voxel,
                                 min_threshold, max_threshold,
                                 min_label_threshold, max_label_threshold,
                                 desired_label ) )
        return( FALSE );

    n_dirs = get_3D_neighbour_directions( connectivity, &dx, &dy, &dz );

    get_volume_sizes( volume, sizes );

    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &checked_flags );
    create_bitlist_3d( sizes[X], sizes[Y], sizes[Z], &change_flags );

    INITIALIZE_QUEUE( queue );

    set_bitlist_bit_3d( &checked_flags, voxel[X], voxel[Y], voxel[Z], TRUE );
    set_bitlist_bit_3d( &change_flags, voxel[X], voxel[Y], voxel[Z], TRUE );

    entry.x = voxel[X];
    entry.y = voxel[Y];
    entry.z = voxel[Z];
    INSERT_IN_QUEUE( queue, entry );

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        z = entry.z;

        for_less( dir, 0, n_dirs )
        {
            tx = x + dx[dir];
            ty = y + dy[dir];
            tz = z + dz[dir];

            if( tx >= 0 && tx < sizes[X] &&
                ty >= 0 && ty < sizes[Y] &&
                tz >= 0 && tz < sizes[Z] &&
                !get_bitlist_bit_3d( &checked_flags, tx, ty, tz ) )
            {
                set_bitlist_bit_3d( &checked_flags, tx, ty, tz, TRUE );

                voxel_index[X] = tx;
                voxel_index[Y] = ty;
                voxel_index[Z] = tz;
                if( should_change_this_one( volume, label_volume, voxel_index,
                                 min_threshold, max_threshold,
                                 min_label_threshold, max_label_threshold,
                                 desired_label ) )
                {
                    set_bitlist_bit_3d( &change_flags, tx, ty, tz, TRUE );
                    entry.x = tx;
                    entry.y = ty;
                    entry.z = tz;
                    INSERT_IN_QUEUE( queue, entry );
                }
            }
        }
    }

    first = TRUE;

    for_less( voxel_index[X], 0, sizes[X] )
    for_less( voxel_index[Y], 0, sizes[Y] )
    for_less( voxel_index[Z], 0, sizes[Z] )
    {
        if( get_bitlist_bit_3d( &change_flags, voxel_index[X], voxel_index[Y],
                                voxel_index[Z] ) )
        {
            set_volume_label_data( label_volume, voxel_index, desired_label );

            for_less( dim, 0, N_DIMENSIONS )
            {
                if( first || voxel_index[dim] < range_changed[0][dim] )
                    range_changed[0][dim] = voxel_index[dim];
                if( first || voxel_index[dim] > range_changed[1][dim] )
                    range_changed[1][dim] = voxel_index[dim];
            }

            first = FALSE;
        }
    }

    delete_bitlist_3d( &checked_flags );
    delete_bitlist_3d( &change_flags );

    DELETE_QUEUE( queue );

    return( TRUE );
}

static  int   Dx4[4] = { 1, 0, -1,  0 };
static  int   Dy4[4] = { 0, 1,  0, -1 };

static  int   Dx8[8] = {  1,  1,  0, -1, -1, -1,  0,  1 };
static  int   Dy8[8] = {  0,  1,  1,  1,  0, -1, -1, -1 };

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_neighbour_directions
@INPUT      : connectivity
@OUTPUT     : dx
              dy
@RETURNS    : number of directions
@DESCRIPTION: Passes back the neighbour directions of a 2D grid.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI int  get_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[] )
{
    int   n_dirs;

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        *dx = Dx4;
        *dy = Dy4;
        n_dirs = 4;
        break;

    case  EIGHT_NEIGHBOURS:
        *dx = Dx8;
        *dy = Dy8;
        n_dirs = 8;
        break;
    }

    return( n_dirs );
}

static  int   Dx6[6] = { 1, 0, 0, -1,  0,  0 };
static  int   Dy6[6] = { 0, 1, 0,  0, -1,  0 };
static  int   Dz6[6] = { 0, 0, 1,  0,  0, -1 };

static  int   Dx26[26];
static  int   Dy26[26];
static  int   Dz26[26];

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_3D_neighbours
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Initializes the 3D neighbours.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  create_3D_neighbours( void )
{
    int   x, y, z, n;

    n = 0;
    for_inclusive( x, -1, 1 )
    for_inclusive( y, -1, 1 )
    for_inclusive( z, -1, 1 )
    {
        if( x != 0 || y != 0 || z != 0 )
        {
            Dx26[n] = x;
            Dy26[n] = y;
            Dz26[n] = z;
            ++n;
        }
    }

    if( n != 26 )
    {
        HANDLE_INTERNAL_ERROR( "create_3D_neighbours" );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_3D_neighbour_directions
@INPUT      : connectivity
@OUTPUT     : dx
              dy
              dz
@RETURNS    : number of directions
@DESCRIPTION: Passes back the neighbour directions of a 3D grid.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI int  get_3D_neighbour_directions(
    Neighbour_types   connectivity,
    int               *dx[],
    int               *dy[],
    int               *dz[] )
{
    static  BOOLEAN  first = TRUE;
    int              n_dirs;

    if( first )
    {
        first = FALSE;
        create_3D_neighbours();
    }

    switch( connectivity )
    {
    case  FOUR_NEIGHBOURS:
        *dx = Dx6;
        *dy = Dy6;
        *dz = Dz6;
        n_dirs = 6;
        break;

    case  EIGHT_NEIGHBOURS:
        *dx = Dx26;
        *dy = Dy26;
        *dz = Dz26;
        n_dirs = 26;
        break;
    }

    return( n_dirs );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : should_change_this_one
@INPUT      : volume
              label_volume
              voxel
              min_threshold
              max_threshold
              label_min_threshold
              label_max_threshold
              desired_label
@OUTPUT     : 
@RETURNS    : TRUE if should change this
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI BOOLEAN  should_change_this_one(
    Volume          volume,
    Volume          label_volume,
    int             voxel[],
    Real            min_threshold,
    Real            max_threshold,
    int             label_min_threshold,
    int             label_max_threshold,
    int             desired_label )
{
    int      label;
    Real     value;

    label = get_volume_label_data( label_volume, voxel );

    if( label == desired_label )
        return( FALSE );

    if( label_min_threshold <= label_max_threshold &&
        (label < label_min_threshold || label > label_max_threshold) )
        return( FALSE );

    if( min_threshold > max_threshold )
        return( TRUE );

    value = get_volume_real_value( volume, voxel[X], voxel[Y], voxel[Z], 0, 0 );

    return( min_threshold <= value && value <= max_threshold );
}
