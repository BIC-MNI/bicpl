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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_polygons.c,v 1.8 1996-12-09 20:20:53 david Exp $";
#endif

#define  MAX_TEMP_STORAGE  1000

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
    int        vertex, poly, size, dim, point_index;
    Point      vertices[MAX_TEMP_STORAGE];
    Point      output_vertices[2*MAX_TEMP_STORAGE];
    Real       voxel[N_DIMENSIONS];
    Real       min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    int        min_iv[N_DIMENSIONS], max_iv[N_DIMENSIONS];
    int        sizes[N_DIMENSIONS], int_voxel[N_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        if( size > MAX_TEMP_STORAGE )
            size = MAX_TEMP_STORAGE;

        for_less( vertex, 0, size )
        {
            point_index = polygons->indices[POINT_INDEX(
                                  polygons->end_indices,poly,vertex)];

            convert_world_to_voxel( volume,
                                    RPoint_x(polygons->points[point_index]),
                                    RPoint_y(polygons->points[point_index]),
                                    RPoint_z(polygons->points[point_index]),
                                    voxel );
            fill_Point( vertices[vertex], voxel[X], voxel[Y], voxel[Z] );

            if( vertex == 0 )
            {
                for_less( dim, 0, N_DIMENSIONS )
                {
                    min_voxel[dim] = voxel[dim];
                    max_voxel[dim] = voxel[dim];
                }
            }
            else
            {
                for_less( dim, 0, N_DIMENSIONS )
                {
                    if( voxel[dim] < min_voxel[dim] )
                        min_voxel[dim] = voxel[dim];
                    if( voxel[dim] > max_voxel[dim] )
                        max_voxel[dim] = voxel[dim];
                }
            }
        }

        for_less( dim, 0, N_DIMENSIONS )
        {
            min_iv[dim] = ROUND( min_voxel[dim] );
            if( min_iv[dim] < 0 )
                min_iv[dim] = 0;

            max_iv[dim] = ROUND( max_voxel[dim] );
            if( max_iv[dim] >= sizes[dim] )
                max_iv[dim] = sizes[dim]-1;
        }
/*
min_iv[0] = 12;
max_iv[0] = 12;
min_iv[1] = 36;
max_iv[1] = 36;
min_iv[2] = 25;
max_iv[2] = 25;
*/

        for_inclusive( int_voxel[X], min_iv[X], max_iv[X] ) 
        for_inclusive( int_voxel[Y], min_iv[Y], max_iv[Y] ) 
        for_inclusive( int_voxel[Z], min_iv[Z], max_iv[Z] ) 
        {
            if( clip_polygon_against_box( size, vertices,
                                          (Real) int_voxel[X] - 0.5,
                                          (Real) int_voxel[X] + 0.5,
                                          (Real) int_voxel[Y] - 0.5,
                                          (Real) int_voxel[Y] + 0.5,
                                          (Real) int_voxel[Z] - 0.5,
                                          (Real) int_voxel[Z] + 0.5,
                                          output_vertices ) > 0 )
            {
                set_volume_label_data( label_volume, int_voxel, label );
            }
        }
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

    if( (Real) Point_x(max_point) - (Real) Point_x(min_point) < max_size &&
        (Real) Point_y(max_point) - (Real) Point_y(min_point) < max_size &&
        (Real) Point_z(max_point) - (Real) Point_z(min_point) < max_size )
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
