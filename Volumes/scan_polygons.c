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

#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_polygons.c,v 1.12 2000-02-05 21:27:29 stever Exp $";
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
@NAME       : scan_a_polygon
@INPUT      : size
              vertices
              voxels
              output_vertices
              polygons
              volume
              label_volume
              sizes
              label
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Scans a polygon into the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Jul. 23, 1997    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  scan_a_polygon(
    int                 size,
    Point               vertices[],
    Point               voxels[],
    Point               output_vertices[],
    Volume              volume,
    Volume              label_volume,
    int                 sizes[],
    int                 label )
{
    int        vertex, dim, n_clip;
    Real       voxel[N_DIMENSIONS];
    Real       min_voxel[N_DIMENSIONS], max_voxel[N_DIMENSIONS];
    int        min_iv[N_DIMENSIONS], max_iv[N_DIMENSIONS];

    min_voxel[X] = 0.0;     /*--- to avoid warnings */
    max_voxel[X] = 0.0;

    for_less( vertex, 0, size )
    {
        convert_world_to_voxel( volume,
                                RPoint_x(vertices[vertex]),
                                RPoint_y(vertices[vertex]),
                                RPoint_z(vertices[vertex]),
                                voxel );

        fill_Point( voxels[vertex], voxel[X], voxel[Y], voxel[Z] );

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

    n_clip = clip_polygon_against_box( size, voxels,
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
@MODIFIED   : Jul. 27, 1997   D. MacDonald  - broke into 2 parts
---------------------------------------------------------------------------- */

public  void  scan_polygons_to_voxels(
    polygons_struct     *polygons,
    Volume              volume,
    Volume              label_volume,
    int                 label,
    Real                max_distance )
{
    int        vertex, poly, size, point_index, max_size;
    Point      *vertices, *voxels, *output_vertices;
    int        sizes[N_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );

    max_size = 0;
    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        max_size = MAX( max_size, size );
    }

    ALLOC( vertices, max_size );
    ALLOC( voxels, max_size );
    ALLOC( output_vertices, 2 * max_size );

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( vertex, 0, size )
        {
            point_index = polygons->indices[POINT_INDEX(
                                  polygons->end_indices,poly,vertex)];
            vertices[vertex] = polygons->points[point_index];
        }

        scan_a_polygon( size, vertices, voxels, output_vertices,
                        volume, label_volume, sizes, label );

    }

    FREE( vertices );
    FREE( voxels );
    FREE( output_vertices );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_quadmesh_to_voxels
@INPUT      : quadmesh
              volume
              label_volume
              label
              max_distance        - ignored
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Scans a quadmesh into the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Jul. 23, 1997    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  scan_quadmesh_to_voxels(
    quadmesh_struct     *quadmesh,
    Volume              volume,
    Volume              label_volume,
    int                 label,
    Real                max_distance )
{
    int        i, j, m, n;
    Point      vertices[4], voxels[4], output_vertices[4];
    int        sizes[N_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );

    get_quadmesh_n_objects( quadmesh, &m, &n );

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            get_quadmesh_patch( quadmesh, i, j, vertices );

            scan_a_polygon( 4, vertices, voxels, output_vertices,
                            volume, label_volume, sizes, label );

        }
    }
}
