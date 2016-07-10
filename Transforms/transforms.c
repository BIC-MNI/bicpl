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
@NAME       : make_scale_transform
@INPUT      : sx
              sy
              sz
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Assigns the transform a (possibly anisotropic) scaling transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_scale_transform( 
    VIO_Real        sx,
    VIO_Real        sy,
    VIO_Real        sz,
    VIO_Transform   *transform )
{
    make_identity_transform( transform );

    Transform_elem( *transform, 0, 0 ) = sx;
    Transform_elem( *transform, 1, 1 ) = sy;
    Transform_elem( *transform, 2, 2 ) = sz;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_transform_x_and_z_axes
@INPUT      : x_axis
              z_axis
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Sets the transform to the orthogonal coordinate system defined
              by the x_axis and z_axis.  To do this, the z axis is used as
              the z axis, and the cross product of the z_axis and the
              x_axis is used as the y_axis.  Then, the x_axis is taken as
              the cross product of the y and z axes, as opposed to the
              one passed in to the function.  In this way, as long as the
              passed in x_axis is not collinear with the z_axis, it need
              not be perpendicular to the z_axis.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  set_transform_x_and_z_axes(
    VIO_Transform   *transform,
    VIO_Vector      *x_axis,
    VIO_Vector      *z_axis )
{
    VIO_Vector  n_z, n_y, n_x;

    NORMALIZE_VECTOR( n_z, *z_axis );
    CROSS_VECTORS( n_y, n_z, *x_axis );
    NORMALIZE_VECTOR( n_y, n_y );
    CROSS_VECTORS( n_x, n_z, n_y );
    NORMALIZE_VECTOR( n_x, n_x );

    set_transform_x_axis( transform, &n_x );
    set_transform_y_axis( transform, &n_y );
    set_transform_z_axis( transform, &n_z );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_translation_transform
@INPUT      : x_trans
              y_trans
              z_trans
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a translation transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_translation_transform(
    VIO_Real        x_trans,
    VIO_Real        y_trans,
    VIO_Real        z_trans,
    VIO_Transform   *transform )
{
    make_identity_transform( transform );

    Transform_elem( *transform, 0, 3 ) = x_trans;
    Transform_elem( *transform, 1, 3 ) = y_trans;
    Transform_elem( *transform, 2, 3 ) = z_trans;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_origin_transform
@INPUT      : origin
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Makes a transformation which has a different origin.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_origin_transform(
    VIO_Point       *origin,
    VIO_Transform   *transform )
{
    make_identity_transform( transform );

    Transform_elem( *transform, 0, 3 ) = (VIO_Transform_elem_type) Point_x(*origin);
    Transform_elem( *transform, 1, 3 ) = (VIO_Transform_elem_type) Point_y(*origin);
    Transform_elem( *transform, 2, 3 ) = (VIO_Transform_elem_type) Point_z(*origin);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_rotation_transform
@INPUT      : radians
              axis
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Makes a transform which rotates the specified number of radians
              around the given axis (VIO_X, VIO_Y, or VIO_Z).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_rotation_transform(
    VIO_Real       radians,
    int        axis,
    VIO_Transform  *transform )
{
    int   a1, a2;
    VIO_Real  c, s;

    a1 = (axis + 1) % VIO_N_DIMENSIONS;
    a2 = (axis + 2) % VIO_N_DIMENSIONS;

    make_identity_transform( transform );

    c = cos( (double) radians );
    s = sin( (double) radians );

    Transform_elem( *transform, a1, a1 ) = c;
    Transform_elem( *transform, a1, a2 ) = s;
    Transform_elem( *transform, a2, a1 ) = -s;
    Transform_elem( *transform, a2, a2 ) = c;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_transform_relative_to_point
@INPUT      : point
              transform
@OUTPUT     : rel_transform
@RETURNS    : 
@DESCRIPTION: Creates a transform which is the specified transform performed
              relative to the given point.  For instance to rotate around
              a point, one could pass the point, and a rotation transform
              created by make_rotation_transform(),
              and the resulting transform would perform rotation about that
              point.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_transform_relative_to_point(
    VIO_Point      *point,
    VIO_Transform  *transform,
    VIO_Transform  *rel_transform )
{
    VIO_Transform  to_origin, to_point;

    make_translation_transform( (VIO_Real) Point_x(*point), (VIO_Real) Point_y(*point),
                                (VIO_Real) Point_z(*point), &to_point );

    make_translation_transform( -(VIO_Real) Point_x(*point),
                                -(VIO_Real) Point_y(*point),
                                -(VIO_Real) Point_z(*point), &to_origin );

    concat_transforms( rel_transform, &to_origin, transform );
    concat_transforms( rel_transform, rel_transform, &to_point );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_transform_in_coordinate_system
@INPUT      : origin
              x_axis
              y_axis
              z_axis
              transform
@OUTPUT     : rel_transform
@RETURNS    : 
@DESCRIPTION: Creates a transform which is the input transform relative to
              the specified coordinate system.  For instance if the input
              transform is a scaling transform, then the resulting transform
              will scale points about the 'origin', in the directions of
              'x_axis', 'y_axis', and 'z_axis'.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_transform_in_coordinate_system(
    VIO_Point      *origin,
    VIO_Vector     *x_axis,
    VIO_Vector     *y_axis,
    VIO_Vector     *z_axis,
    VIO_Transform  *transform,
    VIO_Transform  *rel_transform )
{
    VIO_Transform  to_bases, from_bases;

    make_change_to_bases_transform( origin, x_axis, y_axis, z_axis, &to_bases );
    make_change_from_bases_transform( origin, x_axis, y_axis, z_axis,
                                      &from_bases );

    concat_transforms( rel_transform, &from_bases, transform );
    concat_transforms( rel_transform, rel_transform, &to_bases );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_rotation_about_axis
@INPUT      : axis
              angle
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Creates a transform which corresponds to a rotation about an
              arbitrary vector.  Code comes from Graphics Gems book.
@METHOD     : Graphics Gems, Page ??
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_rotation_about_axis(
    VIO_Vector     *axis,
    VIO_Real       angle,
    VIO_Transform  *transform )
{
    VIO_Real    c, s, t;
    VIO_Real    txy, txz, tyz, sx, sy, sz;
    VIO_Real    x, y, z;
    VIO_Vector  unit_axis;

    NORMALIZE_VECTOR( unit_axis, *axis );

    c = cos( (double) -angle );
    s = sin( (double) -angle );
    t = 1.0 - c;

    x = (VIO_Real) Vector_x( unit_axis );
    y = (VIO_Real) Vector_y( unit_axis );
    z = (VIO_Real) Vector_z( unit_axis );

    txy = t * x * y;
    txz = t * x * z;
    tyz = t * y * z;

    sx = s * x;
    sy = s * y;
    sz = s * z;

    Transform_elem( *transform, 0, 0 ) = t * x * x + c;
    Transform_elem( *transform, 0, 1 ) = txy + sz;
    Transform_elem( *transform, 0, 2 ) = txz - sy;
    Transform_elem( *transform, 0, 3 ) = 0.0;

    Transform_elem( *transform, 1, 0 ) = txy - sz;
    Transform_elem( *transform, 1, 1 ) = t * y * y + c;
    Transform_elem( *transform, 1, 2 ) = tyz + sx;
    Transform_elem( *transform, 1, 3 ) = 0.0;

    Transform_elem( *transform, 2, 0 ) = txz + sy;
    Transform_elem( *transform, 2, 1 ) = tyz - sx;
    Transform_elem( *transform, 2, 2 ) = t * z * z + c;
    Transform_elem( *transform, 2, 3 ) = 0.0;

    Transform_elem( *transform, 3, 0 ) = 0.0;
    Transform_elem( *transform, 3, 1 ) = 0.0;
    Transform_elem( *transform, 3, 2 ) = 0.0;
    Transform_elem( *transform, 3, 3 ) = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_2d_transform_to_rotation_translation
@INPUT      : transform
@OUTPUT     : degrees_clockwise
              x_trans
              y_trans
@RETURNS    : 
@DESCRIPTION: Converts a 2d transform to a angle of rotation and origin
              of rotation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  convert_2d_transform_to_rotation_translation(
    VIO_Transform  *transform,
    VIO_Real       *degrees_clockwise,
    VIO_Real       *x_trans,
    VIO_Real       *y_trans )
{
    VIO_Real   x, y;

    x = Transform_elem(*transform, VIO_X, VIO_X );
    y = Transform_elem(*transform, VIO_Y, VIO_X );

    *degrees_clockwise = VIO_RAD_TO_DEG * compute_clockwise_rotation( x, y );
    *x_trans = Transform_elem( *transform, VIO_X, 3 );
    *y_trans = Transform_elem( *transform, VIO_Y, 3 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_clockwise_rotation
@INPUT      : x
              y
@OUTPUT     : 
@RETURNS    : Radians
@DESCRIPTION: Computes the clockwise angle of rotation required to rotate
              the x axis to the point (x,y).  Will be value greater than or
              equal to 0 and less than two pi.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Real  compute_clockwise_rotation( VIO_Real x, VIO_Real y )
{
    VIO_Real   radians;

    if( x == 0.0 )
    {
        if( y < 0.0 )
            return( M_PI / 2.0 );
        else if( y > 0.0 )
            return( 3.0 * M_PI / 2.0 );
        else
            return( 0.0 );
    }
    else if( y == 0.0 )
    {
        if( x > 0.0 )
            return( 0.0 );
        else
            return( M_PI );
    }
    else
    {
        radians = - (VIO_Real) atan2( (double) y, (double) x );

        if( radians < 0.0 )
            radians += 2.0 * M_PI;

        return( radians );
    }
}

BICAPI  VIO_BOOL  is_transform_right_handed(
    VIO_Transform   *transform )
{
    VIO_Real     volume;
    VIO_Vector   x_axis, y_axis, z_axis, offset;

    get_transform_x_axis( transform, &x_axis );
    get_transform_y_axis( transform, &y_axis );
    get_transform_z_axis( transform, &z_axis );

    CROSS_VECTORS( offset, x_axis, y_axis );
    volume = DOT_VECTORS( offset, z_axis );

    return( volume > 0.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_identity_transform_2d
@INPUT      : transform
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Fills in the 2d transform with the identity transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  make_identity_transform_2d(
    VIO_Transform_2d *transform )
{
    Transform_2d_elem( *transform, 0, 0 ) = 1.0;
    Transform_2d_elem( *transform, 0, 1 ) = 0.0;
    Transform_2d_elem( *transform, 0, 2 ) = 0.0;
    Transform_2d_elem( *transform, 1, 0 ) = 0.0;
    Transform_2d_elem( *transform, 1, 1 ) = 1.0;
    Transform_2d_elem( *transform, 1, 2 ) = 0.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_inverse_transform_2d
@INPUT      : transform
@OUTPUT     : inverse
@RETURNS    : 
@DESCRIPTION: Passes back the inverse of the 2d transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  get_inverse_transform_2d(
    VIO_Transform_2d   *transform,
    VIO_Transform_2d   *inverse )
{
    VIO_Real  determinant;

    determinant = Transform_2d_elem(*transform,0,0) *
                  Transform_2d_elem(*transform,1,1) -
                  Transform_2d_elem(*transform,1,0) *
                  Transform_2d_elem(*transform,0,1);

    Transform_2d_elem(*inverse,0,0) = Transform_2d_elem(*transform,1,1) /
                                      determinant;
    Transform_2d_elem(*inverse,0,1) = -Transform_2d_elem(*transform,0,1) /
                                      determinant;
    Transform_2d_elem(*inverse,0,2) = (Transform_2d_elem(*transform,1,2) *
                                       Transform_2d_elem(*transform,0,1) -
                                       Transform_2d_elem(*transform,1,1) *
                                       Transform_2d_elem(*transform,0,2)) /
                                      determinant;

    Transform_2d_elem(*inverse,1,0) = -Transform_2d_elem(*transform,1,0) /
                                      determinant;
    Transform_2d_elem(*inverse,1,1) = Transform_2d_elem(*transform,0,0) /
                                      determinant;
    Transform_2d_elem(*inverse,1,2) = (Transform_2d_elem(*transform,1,0) *
                                       Transform_2d_elem(*transform,0,2) -
                                       Transform_2d_elem(*transform,0,0) *
                                       Transform_2d_elem(*transform,1,2)) /
                                      determinant;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : transform_point_2d
@INPUT      : transform
              x
              y
@OUTPUT     : x_trans
              y_trans
@RETURNS    : 
@DESCRIPTION: Transforms a 2d point by the specified transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  transform_point_2d(
    VIO_Transform_2d   *transform,
    VIO_Real           x,
    VIO_Real           y,
    VIO_Real           *x_trans,
    VIO_Real           *y_trans )
{
    *x_trans = Transform_2d_elem(*transform,0,0) * x +
               Transform_2d_elem(*transform,0,1) * y +
               Transform_2d_elem(*transform,0,2);
    *y_trans = Transform_2d_elem(*transform,1,0) * x +
               Transform_2d_elem(*transform,1,1) * y +
               Transform_2d_elem(*transform,1,2);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_least_squares_transform_2d
@INPUT      : n_points
              x
              y
              x_trans
              y_trans
@OUTPUT     : transform_2d
@RETURNS    : 
@DESCRIPTION: Computes the 2d transform that best transforms the points
              ( x[], y[] ) to ( x_trans[], y_trans[] ), using linear least
              squares.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  get_least_squares_transform_2d(
    int           n_points,
    VIO_Real          x[],
    VIO_Real          y[],
    VIO_Real          x_trans[],
    VIO_Real          y_trans[],
    VIO_Transform_2d  *transform_2d )
{
    int   p;
    VIO_Real  coefs[2+1];
    VIO_Real  **coords;

    VIO_ALLOC2D( coords, n_points, 2 );

    for_less( p, 0, n_points )
    {
        coords[p][VIO_X] = x[p];
        coords[p][VIO_Y] = y[p];
    }

    least_squares( n_points, 2, coords, x_trans, coefs );

    Transform_2d_elem( *transform_2d, 0, 0 ) = coefs[1];
    Transform_2d_elem( *transform_2d, 0, 1 ) = coefs[2];
    Transform_2d_elem( *transform_2d, 0, 2 ) = coefs[0];
    
    least_squares( n_points, 2, coords, y_trans, coefs );

    Transform_2d_elem( *transform_2d, 1, 0 ) = coefs[1];
    Transform_2d_elem( *transform_2d, 1, 1 ) = coefs[2];
    Transform_2d_elem( *transform_2d, 1, 2 ) = coefs[0];

    VIO_FREE2D( coords );
}
