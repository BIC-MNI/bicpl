
#include  <internal_volume_io.h>
#include  <geom.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Transforms/transforms.c,v 1.1 1994-11-30 15:08:31 david Exp $";
#endif

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

public  void  make_scale_transform( 
    Real        sx,
    Real        sy,
    Real        sz,
    Transform   *transform )
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

public  void  set_transform_x_and_z_axes(
    Transform   *transform,
    Vector      *x_axis,
    Vector      *z_axis )
{
    Vector  n_z, n_y, n_x;

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

public  void  make_translation_transform(
    Real        x_trans,
    Real        y_trans,
    Real        z_trans,
    Transform   *transform )
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

public  void  make_origin_transform(
    Point       *origin,
    Transform   *transform )
{
    make_identity_transform( transform );

    Transform_elem( *transform, 0, 3 ) = Point_x(*origin);
    Transform_elem( *transform, 1, 3 ) = Point_y(*origin);
    Transform_elem( *transform, 2, 3 ) = Point_z(*origin);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_inverse_of_orthogonal_transform
@INPUT      : transform
@OUTPUT     : inverse
@RETURNS    : 
@DESCRIPTION: Computes the inverse of the transform.  The transform
              must be a unit orthogonal transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   compute_inverse_of_orthogonal_transform(
    Transform  *transform,
    Transform  *inverse )
{
    Real   x_trans, y_trans, z_trans;

    x_trans = -Transform_elem( *transform, 0, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 0, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 0, 2 ) *
               Transform_elem( *transform, 2, 3 );

    y_trans = -Transform_elem( *transform, 1, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 1, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 1, 2 ) *
               Transform_elem( *transform, 2, 3 );

    z_trans = -Transform_elem( *transform, 2, 0 ) *
               Transform_elem( *transform, 0, 3 ) -
               Transform_elem( *transform, 2, 1 ) *
               Transform_elem( *transform, 1, 3 ) -
               Transform_elem( *transform, 2, 2 ) *
               Transform_elem( *transform, 2, 3 );

    Transform_elem( *inverse, 0, 0 ) = Transform_elem( *transform, 0, 0 );
    Transform_elem( *inverse, 0, 1 ) = Transform_elem( *transform, 1, 0 );
    Transform_elem( *inverse, 0, 2 ) = Transform_elem( *transform, 2, 0 );
    Transform_elem( *inverse, 0, 3 ) = x_trans;
    Transform_elem( *inverse, 1, 0 ) = Transform_elem( *transform, 0, 1 );
    Transform_elem( *inverse, 1, 1 ) = Transform_elem( *transform, 1, 1 );
    Transform_elem( *inverse, 1, 2 ) = Transform_elem( *transform, 2, 1 );
    Transform_elem( *inverse, 1, 3 ) = y_trans;
    Transform_elem( *inverse, 2, 0 ) = Transform_elem( *transform, 0, 2 );
    Transform_elem( *inverse, 2, 1 ) = Transform_elem( *transform, 1, 2 );
    Transform_elem( *inverse, 2, 2 ) = Transform_elem( *transform, 2, 2 );
    Transform_elem( *inverse, 2, 3 ) = z_trans;
    Transform_elem( *inverse, 3, 0 ) = 0.0;
    Transform_elem( *inverse, 3, 1 ) = 0.0;
    Transform_elem( *inverse, 3, 2 ) = 0.0;
    Transform_elem( *inverse, 3, 3 ) = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : make_rotation_transform
@INPUT      : radians
              axis
@OUTPUT     : transform
@RETURNS    : 
@DESCRIPTION: Makes a transform which rotates the specified number of radians
              around the given axis (X, Y, or Z).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  make_rotation_transform(
    Real       radians,
    int        axis,
    Transform  *transform )
{
    int   a1, a2;
    Real  c, s;

    a1 = (axis + 1) % N_DIMENSIONS;
    a2 = (axis + 2) % N_DIMENSIONS;

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

public  void  make_transform_relative_to_point(
    Point      *point,
    Transform  *transform,
    Transform  *rel_transform )
{
    Transform  to_origin, to_point;

    make_translation_transform( Point_x(*point), Point_y(*point),
                                Point_z(*point), &to_point );

    make_translation_transform( -Point_x(*point), -Point_y(*point),
                                -Point_z(*point), &to_origin );

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

public  void  make_transform_in_coordinate_system(
    Point      *origin,
    Vector     *x_axis,
    Vector     *y_axis,
    Vector     *z_axis,
    Transform  *transform,
    Transform  *rel_transform )
{
    Transform  to_bases, from_bases;

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
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  make_rotation_about_axis(
    Vector     *axis,
    Real       angle,
    Transform  *transform )
{
    Real  c, s, t;
    Real  txy, txz, tyz, sx, sy, sz;
    Real  x, y, z;

    c = cos( (double) -angle );
    s = sin( (double) -angle );
    t = 1.0 - c;

    x = Point_x( *axis );
    y = Point_y( *axis );
    z = Point_z( *axis );

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

public  void  convert_2d_transform_to_rotation_translation(
    Transform  *transform,
    Real       *degrees_clockwise,
    Real       *x_trans,
    Real       *y_trans )
{
    Real   x, y;

    x = Transform_elem(*transform, X, X );
    y = Transform_elem(*transform, Y, X );

    *degrees_clockwise = RAD_TO_DEG * compute_clockwise_rotation( x, y );
    *x_trans = Transform_elem( *transform, X, 3 );
    *y_trans = Transform_elem( *transform, Y, 3 );
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

public  Real  compute_clockwise_rotation( Real x, Real y )
{
    Real   radians;

    if( x == 0.0 )
    {
        if( y < 0.0 )
            return( PI / 2.0 );
        else if( y > 0.0 )
            return( 3.0 * PI / 2.0 );
        else
            return( 0.0 );
    }
    else if( y == 0.0 )
    {
        if( x > 0.0 )
            return( 0.0 );
        else
            return( PI );
    }
    else
    {
        radians = - (Real) atan2( (double) y, (double) x );

        if( radians < 0.0 )
            radians += 2.0 * PI;

        return( radians );
    }
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

public  void  make_identity_transform_2d(
    Transform_2d *transform )
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

public  void  get_inverse_transform_2d(
    Transform_2d   *transform,
    Transform_2d   *inverse )
{
    Real  determinant;

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

public  void  transform_point_2d(
    Transform_2d   *transform,
    Real           x,
    Real           y,
    Real           *x_trans,
    Real           *y_trans )
{
    *x_trans = Transform_2d_elem(*transform,0,0) * x +
               Transform_2d_elem(*transform,0,1) * y +
               Transform_2d_elem(*transform,0,2);
    *y_trans = Transform_2d_elem(*transform,1,0) * x +
               Transform_2d_elem(*transform,1,1) * y +
               Transform_2d_elem(*transform,1,2);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : write_transform_file
@INPUT      : filename
              comments
              transform
@OUTPUT     : 
@RETURNS    : OK or ERROR
@DESCRIPTION: Writes the specified transform to the file, in MNI format.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  write_transform_file(
    char       filename[],
    char       comments[],
    Transform  *transform )
{
    Status              status;
    FILE                *file;
    General_transform   gen_transform;

    create_linear_transform( &gen_transform, transform );

    status = open_file_with_default_suffix( filename, "xfm",
                                 WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        status = output_transform( file, comments, &gen_transform );

        (void) close_file( file );
    }

    delete_general_transform( &gen_transform );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : read_transform_file
@INPUT      : filename
@OUTPUT     : transform
@RETURNS    : OK or ERROR
@DESCRIPTION: Reads a transform from a file.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  read_transform_file(
    char       filename[],
    Transform  *transform )
{
    Status             status;
    FILE               *file;
    General_transform  gen_transform;

    status = open_file_with_default_suffix( filename, "xfm",
                         READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        if( input_transform( file, &gen_transform ) != OK )
            status = ERROR;

        (void) close_file( file );
    }

    if( status == OK && get_transform_type(&gen_transform) != LINEAR )
    {
        print( "File %s does not contain a linear transform.\n", filename );
        status = ERROR;
    }

    if( status == OK )
        *transform = *get_linear_transform_ptr(&gen_transform);

    delete_general_transform( &gen_transform );

    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : least_squares
@INPUT      : n
              x_list
              y_list
              trans_list
@OUTPUT     : a
              b
              c
@RETURNS    : 
@DESCRIPTION: Performs a linear least squares fit to determine the coefficients
              that map (x,y) values to trans values (a*x+b*y+c=trans).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  least_squares(
    int     n,
    Real    x_list[],
    Real    y_list[],
    Real    trans_list[],
    Real    *a,
    Real    *b,
    Real    *c )
{
    int   i;
    Real  x, y, xp;
    Real  sumx, sumy, sumxx, sumyy, sumxy, sumxp, sumyxp, sumxxp;
    Real  denom;

    sumx = 0.0;
    sumy = 0.0;
    sumxx = 0.0;
    sumxy = 0.0;
    sumyy = 0.0;
    sumxp = 0.0;
    sumxxp = 0.0;
    sumyxp = 0.0;

    for_less( i, 0, n )
    {
        x = x_list[i];
        y = y_list[i];
        xp = trans_list[i];

        sumx += x;
        sumy += y;
        sumxx += x * x;
        sumxy += x * y;
        sumyy += y * y;
        sumxp += xp;
        sumxxp += x * xp;
        sumyxp += y * xp;
    }

    denom = n*sumxx*sumyy - n*sumxy*sumxy - sumx*sumx*sumyy +
            2.0*sumxy*sumx*sumy - sumxx*sumy*sumy;

    if( denom == 0.0 )
    {
        handle_internal_error( "least_squares_transform_2d: division by zero.");
        *a = 0.0;
        *b = 0.0;
        *c = 0.0;
        return;
    }

    *a = (n*sumxxp*sumyy - n*sumxy*sumyxp - sumxxp*sumy*sumy +
          sumxy*sumy*sumxp + sumy*sumx*sumyxp - sumx*sumxp*sumyy) / denom;

    *b = -(-sumxy*sumx*sumxp + sumx*sumx*sumyxp + n*sumxy*sumxxp -
           n*sumxx*sumyxp - sumy*sumx*sumxxp + sumxx*sumy*sumxp) / denom;

    *c = (-sumx*sumxxp*sumyy + sumx*sumxy*sumyxp + sumy*sumxy*sumxxp -
           sumy*sumxx*sumyxp + sumxp*sumxx*sumyy - sumxp*sumxy*sumxy) / denom;
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

public  void  get_least_squares_transform_2d(
    int           n_points,
    Real          x[],
    Real          y[],
    Real          x_trans[],
    Real          y_trans[],
    Transform_2d  *transform_2d )
{
    Real  a, b, c;
    
    least_squares( n_points, x, y, x_trans, &a, &b, &c );
    Transform_2d_elem( *transform_2d, 0, 0 ) = a;
    Transform_2d_elem( *transform_2d, 0, 1 ) = b;
    Transform_2d_elem( *transform_2d, 0, 2 ) = c;
    
    least_squares( n_points, x, y, y_trans, &a, &b, &c );
    Transform_2d_elem( *transform_2d, 1, 0 ) = a;
    Transform_2d_elem( *transform_2d, 1, 1 ) = b;
    Transform_2d_elem( *transform_2d, 1, 2 ) = c;
}
