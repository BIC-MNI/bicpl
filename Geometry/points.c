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
#include  <geom.h>
#include  <trans.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/points.c,v 1.3 1995-07-31 13:45:06 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : null_Point
@INPUT      : p
@OUTPUT     : 
@RETURNS    : TRUE if point is ( 0, 0, 0 ).
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  null_Point(
    Point   *p )
{
    return( Point_x(*p) == 0.0 &&
            Point_y(*p) == 0.0 &&
            Point_z(*p) == 0.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : null_Vector
@INPUT      : 
@OUTPUT     : 
@RETURNS    : TRUE if vector is ( 0, 0, 0 ).
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  null_Vector(
    Vector   *v )
{
    return( Vector_x(*v) == 0.0 &&
            Vector_y(*v) == 0.0 &&
            Vector_z(*v) == 0.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : distance_between_points
@INPUT      : p1
              p2
@OUTPUT     : 
@RETURNS    : Euclidean distance between points
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  distance_between_points(
    Point  *p1,
    Point  *p2 )
{
    Real   dx, dy, dz;

    dx = Point_x(*p2) - Point_x(*p1);
    dy = Point_y(*p2) - Point_y(*p1);
    dz = Point_z(*p2) - Point_z(*p1);

    return( sqrt( dx * dx + dy * dy + dz * dz ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : points_within_distance
@INPUT      : p1
              p2
              distance
@OUTPUT     : 
@RETURNS    : TRUE if points within distance
@DESCRIPTION: Determines if two points are within the specified distance.  Tries
              to be efficient by not performing a sqrt.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  points_within_distance(
    Point  *p1,
    Point  *p2,
    Real   distance )
{
    Real  dx, dy, dz;

    dx = Point_x(*p1) - Point_x(*p2);
    dy = Point_y(*p1) - Point_y(*p2);
    dz = Point_z(*p1) - Point_z(*p2);

    return( dx * dx + dy * dy + dz * dz <= distance * distance );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : apply_point_to_min_and_max
@INPUT      : point
              min_point   - current min position
              max_point   - current max position
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the range of space specified by min_point and max_point
              to the smallest rectangular range containing both the current
              range and the specified point.  This is
              performed by expanding the rectangular range if needed, to
              contain the specified point.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  apply_point_to_min_and_max(
    Point   *point,
    Point   *min_point,
    Point   *max_point )
{
    int    c;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( Point_coord(*point,c) < Point_coord(*min_point,c) )
            Point_coord(*min_point,c) = Point_coord(*point,c);

        if( Point_coord(*point,c) > Point_coord(*max_point,c) )
            Point_coord(*max_point,c) = Point_coord(*point,c);
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : expand_min_and_max_points
@INPUT      : min_point
              max_point
@OUTPUT     : min_to_check
              max_to_check
@RETURNS    : 
@DESCRIPTION: Sets the rectangular range of space specified by min_point and
              max_point to the rectangular range containing the four argument
              points.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  expand_min_and_max_points(
    Point   *min_point,
    Point   *max_point,
    Point   *min_to_check,
    Point   *max_to_check )
{
    int    c;

    for_less( c, 0, N_DIMENSIONS )
    {
        if( Point_coord(*min_to_check,c) < Point_coord(*min_point,c) )
            Point_coord(*min_point,c) = Point_coord(*min_to_check,c);
        if( Point_coord(*max_to_check,c) > Point_coord(*max_point,c) )
            Point_coord(*max_point,c) = Point_coord(*max_to_check,c);
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_range_points
@INPUT      : n_points
              points
@OUTPUT     : min_corner
              max_corner
@RETURNS    : 
@DESCRIPTION: Finds the rectangular range containing all the points.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_range_points(
    int                n_points,
    Point              points[],
    Point              *min_corner,
    Point              *max_corner )
{
    int     i;

    if( n_points > 0 )
    {
        *min_corner = points[0];
        *max_corner = points[0];

        for_less( i, 1, n_points )
        {
            apply_point_to_min_and_max( &points[i], min_corner, max_corner );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_points_centroid
@INPUT      : n_points
              points
@OUTPUT     : centroid
@RETURNS    : 
@DESCRIPTION: Finds the centroid of the set of points.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_points_centroid(
    int     n_points,
    Point   points[],
    Point   *centroid )
{
    int   i;

    Point_x(*centroid) = 0.0;
    Point_y(*centroid) = 0.0;
    Point_z(*centroid) = 0.0;

    for_less( i, 0, n_points )
    {
        Point_x(*centroid) += Point_x(points[i]);
        Point_y(*centroid) += Point_y(points[i]);
        Point_z(*centroid) += Point_z(points[i]);
    }

    Point_x(*centroid) /= (Real) n_points;
    Point_y(*centroid) /= (Real) n_points;
    Point_z(*centroid) /= (Real) n_points;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : reverse_vectors
@INPUT      : n_vectors
              vectors
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Reverses the direction of each vector in the list.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public   void     reverse_vectors(
    int       n_vectors,
    Vector    vectors[] )
{
    int     i;

    for_less( i, 0, n_vectors )
        SCALE_VECTOR( vectors[i], vectors[i], -1.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_angle_between_points
@INPUT      : prev
              this
              next
@OUTPUT     : 
@RETURNS    : angle in radians.
@DESCRIPTION: Returns the angle between the three points, in the range 0 to
              PI.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  get_angle_between_points(
    Point  *prev,
    Point  *this,
    Point  *next )
{
    Real    angle, x, y;
    Vector  v1, v2, x_axis, y_axis;

    SUB_POINTS( v1, *prev, *this );
    SUB_POINTS( v2, *next, *this );

    NORMALIZE_VECTOR( v1, v1 );
    NORMALIZE_VECTOR( v2, v2 );

    x = DOT_VECTORS( v1, v2 );

    SCALE_VECTOR( x_axis, v1, x );

    SUB_VECTORS( y_axis, v2, x_axis );

    y = MAGNITUDE( y_axis );

    angle = compute_clockwise_rotation( x, y );

    return( angle );
}
