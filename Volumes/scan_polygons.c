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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_polygons.c,v 1.10 1997-03-23 21:11:36 david Exp $";
#endif

#define  MAX_TEMP_STORAGE  1000

private  void  recursive_scan_polygon_to_voxels(
    int                 size,
    Point               points[],
    Volume              label_volume,
    int                 label,
    int                 min_voxel[],
    int                 max_voxel[] )
{
    int        n_left, n_right, dim, max_dim, save, pos;
    Real       slice_pos;
    Point      right_vertices[MAX_TEMP_STORAGE];
    Point      left_vertices[MAX_TEMP_STORAGE];
    Vector     normal;

    max_dim = 0;
    for_less( dim, 1, N_DIMENSIONS )
    {
        if( max_voxel[dim] - min_voxel[dim] >
            max_voxel[max_dim] - min_voxel[max_dim] )
            max_dim = dim;
    }

    if( min_voxel[max_dim] == max_voxel[max_dim] )
    {
        set_volume_label_data( label_volume, min_voxel, label );
        return;
    }

    pos = (min_voxel[max_dim] + max_voxel[max_dim]) / 2;
    slice_pos = (Real) pos + 0.5;

    fill_Vector( normal, 0.0, 0.0, 0.0 );
    Vector_coord( normal, max_dim ) = -1.0f;

    split_polygon_with_plane( size, points, slice_pos,
                              &normal, &n_left, left_vertices,
                              &n_right, right_vertices );

    if( n_left > 0 )
    {
        save = max_voxel[max_dim];
        max_voxel[max_dim] = pos;
        recursive_scan_polygon_to_voxels( n_left, left_vertices,
                                          label_volume, label,
                                          min_voxel, max_voxel );
        max_voxel[max_dim] = save;
    }

    if( n_right > 0 )
    {
        save = min_voxel[max_dim];
        min_voxel[max_dim] = pos+1;
        recursive_scan_polygon_to_voxels( n_right, right_vertices,
                                          label_volume, label,
                                          min_voxel, max_voxel );
        min_voxel[max_dim] = save;
    }
}

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
    int        vertex, poly, size, dim, point_index, n_clip;
    Point      vertices[MAX_TEMP_STORAGE];
    Point      output_vertices[2*MAX_TEMP_STORAGE];
    Real       voxel[N_DIMENSIONS];
    Real       min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    int        min_iv[N_DIMENSIONS], max_iv[N_DIMENSIONS];
    int        sizes[N_DIMENSIONS];

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

        n_clip = clip_polygon_against_box( size, vertices,
                                           (Real) min_iv[X] - 0.5,
                                           (Real) max_iv[X] + 0.5,
                                           (Real) min_iv[Y] - 0.5,
                                           (Real) max_iv[Y] + 0.5,
                                           (Real) min_iv[Z] - 0.5,
                                           (Real) max_iv[Z] + 0.5,
                                           output_vertices );

        if( n_clip > 0 )
        {
            recursive_scan_polygon_to_voxels( n_clip, output_vertices,
                                              label_volume, label,
                                              min_iv, max_iv );
        }
    }
}
