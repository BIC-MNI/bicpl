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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_polygons.c,v 1.4 1995-07-31 13:45:51 david Exp $";
#endif

#define  MAX_TEMP_STORAGE  100

private  void  recursive_polygon_scan(
    int                 size,
    Point               vertices[],
    Volume              volume,
    Volume              label_volume,
    int                 label,
    Real                max_size );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_polygons_to_voxels
@INPUT      : polygons
              volume
              label_volume
              label
              max_size
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Subdivides polygons until they are smaller than max_size
              size, then labels the voxel corresponding to the centre of
              each fragment.  Only polygons with 3 or 4 vertices will be
              scanned, should be rewritten to handle more.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  scan_polygons_to_voxels(
    polygons_struct     *polygons,
    Volume              volume,
    Volume              label_volume,
    int                 label,
    Real                max_size )
{
    int        i, poly, size;
    Point      vertices[MAX_TEMP_STORAGE];

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        if( size > MAX_TEMP_STORAGE )
            size = MAX_TEMP_STORAGE;

        for_less( i, 0, size )
        {
            vertices[i] = polygons->points[
              polygons->indices[POINT_INDEX(polygons->end_indices,poly,i)]];
        }

        if( size == 3 || size == 4 )
            recursive_polygon_scan( size, vertices, volume, label_volume,
                                    label, max_size );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : recursive_polygon_scan
@INPUT      : size
              vertices
              volume
              label_volume
              label
              max_size
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Recursviely Subdivides a polygon until it is smaller than
              max_size size, then labels the voxel corresponding to the
              centre of each fragment.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  recursive_polygon_scan(
    int                 size,
    Point               vertices[],
    Volume              volume,
    Volume              label_volume,
    int                 label,
    Real                max_size )
{
    Point            midpoints[4], min_point, max_point, centre;
    Point            sub_points[4];
    Real             voxel[MAX_DIMENSIONS];
    int              edge, int_voxel[MAX_DIMENSIONS];

    get_range_points( size, vertices, &min_point, &max_point );

    if( Point_x(max_point)-Point_x(min_point) < max_size &&
        Point_y(max_point)-Point_y(min_point) < max_size &&
        Point_z(max_point)-Point_z(min_point) < max_size )
    {
        get_points_centroid( size, vertices, &centre );
        convert_world_to_voxel( volume,
                                (Real) Point_x(centre),
                                (Real) Point_y(centre),
                                (Real) Point_z(centre), voxel );
        fill_Point( centre, voxel[X], voxel[Y], voxel[Z] );

        if( voxel_is_within_volume( volume, voxel ) )
        {
            convert_real_to_int_voxel( N_DIMENSIONS, voxel, int_voxel );
            set_volume_label_data( label_volume, int_voxel, label );
        }
        return;
    }

    for_less( edge, 0, size )
    {
        INTERPOLATE_POINTS( midpoints[edge], vertices[edge],
                            vertices[(edge+1)%size], 0.5 );
    }

    if( size == 3 )
    {
        sub_points[0] = vertices[0];
        sub_points[1] = midpoints[0];
        sub_points[2] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = midpoints[0];
        sub_points[1] = vertices[1];
        sub_points[2] = midpoints[1];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = midpoints[2];
        sub_points[1] = midpoints[1];
        sub_points[2] = vertices[2];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = midpoints[0];
        sub_points[1] = midpoints[1];
        sub_points[2] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );
    }
    else
    {
        get_points_centroid( size, vertices, &centre );

        sub_points[0] = vertices[0];
        sub_points[1] = midpoints[0];
        sub_points[2] = centre;
        sub_points[3] = midpoints[3];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = midpoints[0];
        sub_points[1] = vertices[1];
        sub_points[2] = midpoints[1];
        sub_points[3] = centre;
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = midpoints[3];
        sub_points[1] = centre;
        sub_points[2] = midpoints[2];
        sub_points[3] = vertices[3];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );

        sub_points[0] = centre;
        sub_points[1] = midpoints[1];
        sub_points[2] = vertices[2];
        sub_points[3] = midpoints[2];
        recursive_polygon_scan( size, sub_points, volume, label_volume,
                                label, max_size );
    }
}
