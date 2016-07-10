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

#define  MAX_TEMP_STORAGE  1000


/*! \brief Helper for scan_a_polygon
 *
 * Assign the label value to each voxel that intersects the
 * polygon.  I suspect the algorithm assumes a planar, convex polygon.
 * The input parameters min_voxel and max_voxel 
 * specify a bounding box around the polygon,
 * in voxel coordinates
 *
 * \param size number of points in polygon
 * \param points array of polygon vertex coordinates
 * \param label_volume output volume
 * \param label value to store in label_volume
 * \param min_voxel array of minimum voxel values
 * \param max_voxel array of maximum voxel values
 */
static void  recursive_scan_polygon_to_voxels(
    int                 size,
    VIO_Point               points[],
    VIO_Volume              label_volume,
    int                 label,
    int                 min_voxel[],
    int                 max_voxel[] )
{
    int        n_left, n_right, dim, max_dim, save, pos;
    VIO_Real       slice_pos;
    VIO_Point      right_vertices[MAX_TEMP_STORAGE];
    VIO_Point      left_vertices[MAX_TEMP_STORAGE];
    VIO_Vector     normal;

    max_dim = 0;
    for_less( dim, 1, VIO_N_DIMENSIONS )
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
    slice_pos = (VIO_Real) pos + 0.5;

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

/*! \brief Label all voxels of a single polygon.
 *
 * \param size number of vertices in the polygon
 * \param vertices array of vertex coordinates
 * \param voxels array of vertex coordinates in voxel space
 * \param n_output_vertices size of output_vertices array
 * \param output_vertices array of vertex coordinates (in voxel space)
 *                        after clipping input polygon to volume boundaries
 * \param volume specifies world-to-volume transformation
 * \param label_volume output volume
 * \param sizes number of voxels in each dimension of volume
 * \param label value to store in label_volume
 */

BICAPI void  scan_a_polygon(
    int                 size,
    VIO_Point               vertices[],
    VIO_Point               voxels[],
    int                 n_output_vertices,
    VIO_Point               output_vertices[],
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 sizes[],
    int                 label )
{
    int        vertex, dim, n_clip;
    VIO_Real   voxel[VIO_MAX_DIMENSIONS];
    VIO_Real   min_voxel[VIO_MAX_DIMENSIONS], max_voxel[VIO_MAX_DIMENSIONS];
    int        min_iv[VIO_MAX_DIMENSIONS], max_iv[VIO_MAX_DIMENSIONS];

    min_voxel[VIO_X] = 0.0;     /*--- to avoid warnings */
    max_voxel[VIO_X] = 0.0;

    for_less( vertex, 0, size )
    {
        convert_world_to_voxel( volume,
                                RPoint_x(vertices[vertex]),
                                RPoint_y(vertices[vertex]),
                                RPoint_z(vertices[vertex]),
                                voxel );

        fill_Point( voxels[vertex], voxel[VIO_X], voxel[VIO_Y], voxel[VIO_Z] );

        if( vertex == 0 )
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                min_voxel[dim] = voxel[dim];
                max_voxel[dim] = voxel[dim];
            }
        }
        else
        {
            for_less( dim, 0, VIO_N_DIMENSIONS )
            {
                if( voxel[dim] < min_voxel[dim] )
                    min_voxel[dim] = voxel[dim];
                if( voxel[dim] > max_voxel[dim] )
                    max_voxel[dim] = voxel[dim];
            }
        }
    }

    for_less( dim, 0, VIO_N_DIMENSIONS )
    {
        min_iv[dim] = VIO_ROUND( min_voxel[dim] );
        if( min_iv[dim] < 0 )
            min_iv[dim] = 0;

        max_iv[dim] = VIO_ROUND( max_voxel[dim] );
        if( max_iv[dim] >= sizes[dim] )
            max_iv[dim] = sizes[dim]-1;
    }

    n_clip = clip_polygon_against_box( size, voxels,
                                       (VIO_Real) min_iv[VIO_X] - 0.5,
                                       (VIO_Real) max_iv[VIO_X] + 0.5,
                                       (VIO_Real) min_iv[VIO_Y] - 0.5,
                                       (VIO_Real) max_iv[VIO_Y] + 0.5,
                                       (VIO_Real) min_iv[VIO_Z] - 0.5,
                                       (VIO_Real) max_iv[VIO_Z] + 0.5,
				       n_output_vertices,
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

/*! \brief Add label to all voxels that intersect polygons.
 *
 * \param polygons list of polygons to scan
 * \param volume specifies world-to-volume transformation
 * \param label_volume output volume
 * \param label value to store in label_volume
 * \param max_distance ignored
 */
BICAPI void  scan_polygons_to_voxels(
    polygons_struct     *polygons,
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 label,
    VIO_Real                max_distance )
{
    int        vertex, poly, size, point_index, max_size;
    VIO_Point      *vertices, *voxels, *output_vertices;
    int        sizes[VIO_N_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );

    max_size = 0;
    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );
        max_size = MAX( max_size, size );
    }

    ALLOC( vertices, max_size );
    ALLOC( voxels, max_size );
    /* Possible bug here: a triangle can be cut into a 7-gon after
     * clipping against a box.  What is true upper limit on output_vertices?
     */
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

        scan_a_polygon( size, vertices, voxels, 
			2*max_size, output_vertices,
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

/*! \brief Add label to all voxels that intersect a quadmesh.
 *
 * \param quadmesh the mesh to scan
 * \param volume specifies world-to-volume transformation
 * \param label_volume output volume
 * \param label value to store in label_volume
 * \param max_distance ignored
 */
BICAPI void  scan_quadmesh_to_voxels(
    quadmesh_struct     *quadmesh,
    VIO_Volume              volume,
    VIO_Volume              label_volume,
    int                 label,
    VIO_Real                max_distance )
{
    int        i, j, m, n;
    VIO_Point      vertices[4], voxels[4], output_vertices[4];
    int        sizes[VIO_N_DIMENSIONS];

    get_volume_sizes( label_volume, sizes );

    get_quadmesh_n_objects( quadmesh, &m, &n );

    for_less( i, 0, m )
    {
        for_less( j, 0, n )
        {
            get_quadmesh_patch( quadmesh, i, j, vertices );

            scan_a_polygon( 4, vertices, voxels, 
			    4, output_vertices,
                            volume, label_volume, sizes, label );

        }
    }
}
