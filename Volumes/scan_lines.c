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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_lines.c,v 1.5 1995-10-19 15:48:33 david Exp $";
#endif

private  void  scan_line_segment_to_voxels(
    Volume    volume,
    Volume    label_volume,
    int       label,
    Point     *p1,
    Point     *p2,
    Real      radius );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_lines_to_voxels
@INPUT      : lines
              volume
              label_volume
              label
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Fills in the label_volume with label, where the line
              intersects it.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  scan_lines_to_voxels(
    lines_struct     *lines,
    Volume           volume,
    Volume           label_volume,
    int              label )
{
    int        i, line, size;
    Point      p1, p2;

    for_less( line, 0, lines->n_items )
    {
        size = GET_OBJECT_SIZE( *lines, line );

        for_less( i, 0, size-1 )
        {
            p1 = lines->points[
              lines->indices[POINT_INDEX(lines->end_indices,line,i)]];
            p2 = lines->points[
              lines->indices[POINT_INDEX(lines->end_indices,line,i+1)]];

            scan_line_segment_to_voxels( volume, label_volume, label,
                                         &p1, &p2,
                                         (Real) lines->line_thickness );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Real  point_segment_distance_squared(
    Point    *point,
    Point    *p1,
    Point    *p2 )
{
    Vector   offset;
    Point    closest;

    get_closest_point_on_line_segment( point, p1, p2, &closest );

    SUB_POINTS( offset, *point, closest );

    return( DOT_VECTORS( offset, offset ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_line_segment_to_voxels
@INPUT      : volume
              label_volume
              label
              p1
              p2
              radius
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Scans a cylinder with spheres on both ends into the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  scan_line_segment_to_voxels(
    Volume    volume,
    Volume    label_volume,
    int       label,
    Point     *p1,
    Point     *p2,
    Real      radius )
{
    int        c, sizes[MAX_DIMENSIONS];
    Point      min_point, max_point;
    Real       min_voxel[MAX_DIMENSIONS], min_v, max_v;
    Real       max_voxel[MAX_DIMENSIONS];
    Real       x_world, y_world, z_world;
    int        int_voxel[MAX_DIMENSIONS];
    int        int_min_voxel[MAX_DIMENSIONS];
    int        int_max_voxel[MAX_DIMENSIONS];
    Point      world_voxel;

    for_less( c, 0, N_DIMENSIONS )
    {
        Point_coord(min_point,c) = MIN(Point_coord(*p1,c),Point_coord(*p2,c)) -
                                   radius;
        Point_coord(max_point,c) = MAX(Point_coord(*p1,c),Point_coord(*p2,c)) +
                                   radius;
    }

    convert_world_to_voxel( volume,
                            (Real) Point_x(min_point),
                            (Real) Point_y(min_point),
                            (Real) Point_z(min_point), min_voxel );
    convert_world_to_voxel( volume,
                            (Real) Point_x(max_point), 
                            (Real) Point_y(max_point),
                            (Real) Point_z(max_point), max_voxel );

    get_volume_sizes( volume, sizes );

    for_less( c, 0, N_DIMENSIONS )
    {
        min_v = MIN( min_voxel[c], max_voxel[c] );
        max_v = MAX( min_voxel[c], max_voxel[c] );

        int_min_voxel[c] = ROUND( min_v ) - 1.0;
        int_max_voxel[c] = ROUND( max_v ) + 1.0;

        if( int_min_voxel[c] < 0 )
            int_min_voxel[c] = 0;
        if( int_max_voxel[c] > sizes[c]-1 )
            int_max_voxel[c] = sizes[c]-1;
    }

    for_inclusive( int_voxel[X], int_min_voxel[X], int_max_voxel[X] )
    for_inclusive( int_voxel[Y], int_min_voxel[Y], int_max_voxel[Y] )
    for_inclusive( int_voxel[Z], int_min_voxel[Z], int_max_voxel[Z] )
    {
        convert_3D_voxel_to_world( volume, (Real) int_voxel[X],
                                   (Real) int_voxel[Y], (Real) int_voxel[Z],
                                   &x_world, &y_world, &z_world );

        fill_Point( world_voxel, x_world, y_world, z_world );

        if( point_segment_distance_squared( &world_voxel, p1, p2 ) <
                     radius * radius )
        {
            set_volume_label_data( label_volume, int_voxel, label );
        }
    }
}
