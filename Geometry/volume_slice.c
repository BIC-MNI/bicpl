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

#include "bicpl_internal.h"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_slice_quadmesh
@INPUT      : volume
              axis_index
              voxel_position
              x_tess
              y_tess
              x_min   : optional ranges, ignored if x_min > x_max
              x_max
              y_min   : optional ranges, ignored if y_min > y_max
              y_max
@OUTPUT     : quadmesh
@RETURNS    : 
@DESCRIPTION: Creates a quadmesh slice corresponding to a slice through
              the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void   create_slice_quadmesh(
    VIO_Volume           volume,
    int              axis_index,
    VIO_Real             voxel_position,
    int              x_tess,
    int              y_tess,
    VIO_Real             x_min,
    VIO_Real             x_max,
    VIO_Real             y_min,
    VIO_Real             y_max,
    quadmesh_struct  *quadmesh )
{
    VIO_Point            point;
    VIO_Vector           normal;
    VIO_Real             x_w, y_w, z_w;
    int              sizes[VIO_N_DIMENSIONS];
    VIO_Real             voxel[VIO_N_DIMENSIONS];
    int              x, y, x_axis, y_axis;
    VIO_Surfprop         spr;

    x_axis = (axis_index + 1) % VIO_N_DIMENSIONS;
    y_axis = (axis_index + 2) % VIO_N_DIMENSIONS;
    get_volume_sizes( volume, sizes );

    if( x_tess <= 1 )
        x_tess = MAX( 2, sizes[x_axis] );
    if( y_tess <= 1 )
        y_tess = MAX( 2, sizes[y_axis] );

    get_default_surfprop( &spr );
    initialize_quadmesh( quadmesh, WHITE, &spr, x_tess, y_tess );

    voxel[axis_index] = voxel_position;

    fill_Vector( normal, 0.0, 0.0, 0.0 );
    Vector_coord( normal, axis_index ) = 1.0f;

    if( x_min >= x_max )
    {
        x_min = 0.0;
        x_max = (VIO_Real) (sizes[x_axis] - 1);
    }

    if( y_min >= y_max )
    {
        y_min = 0.0;
        y_max = (VIO_Real) (sizes[y_axis] - 1);
    }

    for_less( x, 0, x_tess )
    {
        voxel[x_axis] = x_min + (x_max - x_min) * (VIO_Real) x / (VIO_Real) (x_tess-1);
        for_less( y, 0, y_tess )
        {
            voxel[y_axis] = y_min + (y_max - y_min) * (VIO_Real)y/(VIO_Real)(y_tess-1);

            convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
            fill_Point( point, x_w, y_w, z_w );
            set_quadmesh_point( quadmesh, x, y, &point, &normal );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_slice_3d
@INPUT      : volume
              origin
              normal
@OUTPUT     : polygons
@RETURNS    : 
@DESCRIPTION: Creates a polygon of 3 to 6 points which is the intersection
              of an arbitrarily oriented plane with a volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void   create_slice_3d(
    VIO_Volume           volume,
    VIO_Point            *origin,
    VIO_Vector           *normal,
    polygons_struct  *polygons )
{
    int              i, n_points;
    VIO_Point            point;
    VIO_Real             xw, yw, zw;
    VIO_Real             voxel[VIO_MAX_DIMENSIONS];
    VIO_Real             origin_voxel[VIO_MAX_DIMENSIONS];
    VIO_Real             y_axis[VIO_MAX_DIMENSIONS];
    VIO_Real             x_axis[VIO_MAX_DIMENSIONS];
    VIO_Real             outline[6][VIO_MAX_DIMENSIONS];
    VIO_Vector           v1, v2;

    create_two_orthogonal_vectors( normal, &v1, &v2 );

    convert_world_vector_to_voxel( volume,
                                   (VIO_Real) Vector_x(v1),
                                   (VIO_Real) Vector_y(v1),
                                   (VIO_Real) Vector_z(v1),
                                   x_axis );
    convert_world_vector_to_voxel( volume,
                                   (VIO_Real) Vector_x(v2),
                                   (VIO_Real) Vector_y(v2),
                                   (VIO_Real) Vector_z(v2),
                                   y_axis );

    convert_world_to_voxel( volume,
                            (VIO_Real) Point_x(*origin), (VIO_Real) Point_y(*origin),
                            (VIO_Real) Point_z(*origin), origin_voxel );

    n_points = get_volume_cross_section( volume, origin_voxel, x_axis, y_axis,
                                         outline );

    initialize_polygons( polygons, WHITE, NULL );

    for_less( i, 0, n_points )
    {
        voxel[0] = outline[i][0];
        voxel[1] = outline[i][1];
        voxel[2] = outline[i][2];

        convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );

        fill_Point( point, xw, yw, zw );

        add_point_to_polygon( polygons, &point, normal );
    }
}
