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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/geometry.c,v 1.14 1995-09-19 18:23:53 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_polygon_normal_no_normalize
@INPUT      : n_points
              points
@OUTPUT     : normal
@RETURNS    : 
@DESCRIPTION: Finds the normal to a polygon (convex or not), using Newell's
              formula.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  find_polygon_normal_no_normalize(
    int      n_points,
    Point    points[],
    Real     *nx,
    Real     *ny,
    Real     *nz )
{
    int     i, next_i;
    Vector  v1, v2, normal;

    *nx = 0.0;
    *ny = 0.0;
    *nz = 0.0;

    for_less( i, 0, n_points )
    {
        next_i = (i + 1) % n_points;

        *nx -= (Point_y(points[i]) + Point_y(points[next_i])) *
               (Point_z(points[i]) - Point_z(points[next_i]) );

        *ny -= (Point_z(points[i]) + Point_z(points[next_i])) *
               (Point_x(points[i]) - Point_x(points[next_i]) );

        *nz -= (Point_x(points[i]) + Point_x(points[next_i])) *
               (Point_y(points[i]) - Point_y(points[next_i]) );
    }

    /*--- if result is null, try to find one vertex for which a normal can
          be computed */

    if( *nx == 0.0 && *ny == 0.0 && *nz == 0.0 )
    {
        for_less( i, 0, n_points )
        {
            SUB_POINTS( v1, points[(i+1)%n_points], points[i] );
            SUB_POINTS( v2, points[(i-1)%n_points], points[i] );
            CROSS_VECTORS( normal, v1, v2 );
            *nx = Vector_x( normal );
            *ny = Vector_y( normal );
            *nz = Vector_z( normal );
            if( !null_Vector( &normal ) )
                break;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_polygon_normal
@INPUT      : n_points
              points
@OUTPUT     : normal
@RETURNS    : 
@DESCRIPTION: Finds the normal to a polygon (convex or not), using Newell's
              formula, then normalizing the result to unit length.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  find_polygon_normal(
    int      n_points,
    Point    points[],
    Vector   *normal )
{
    Real   nx, ny, nz;

    find_polygon_normal_no_normalize( n_points, points, &nx, &ny, &nz );

    fill_Vector( *normal, nx, ny, nz );

    NORMALIZE_VECTOR( *normal, *normal );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_plane_through_points
@INPUT      : n_points
              points
@OUTPUT     : normal
              plane_constant
@RETURNS    : 
@DESCRIPTION: Computes the plane through the 3D points.  Rather than compute
              the polygon normal, which is dependent on the ordering of
              points, it should do a linear least squares fit of the points.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   get_plane_through_points(
    int      n_points,
    Point    points[],
    Vector   *normal,
    Real     *plane_constant )
{
    Point   centroid;

    find_polygon_normal( n_points, points, normal );

    NORMALIZE_VECTOR( *normal, *normal );

    get_points_centroid( n_points, points, &centroid );

    *plane_constant = - distance_from_plane( &centroid, normal, 0.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : distance_from_plane
@INPUT      : point
              plane_normal
              plane_constant
@OUTPUT     : 
@RETURNS    : Distance
@DESCRIPTION: Returns the distance of a point from a plane.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  distance_from_plane(
    Point    *point,
    Vector   *plane_normal,
    Real     plane_constant )
{
    return( DOT_POINT_VECTOR( *point, *plane_normal ) + plane_constant );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : distance_from_line
@INPUT      : point
              end_point1
              end_point2
@OUTPUT     : 
@RETURNS    : Distance
@DESCRIPTION: Returns the distance from the point to the line.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  distance_from_line(
    Point    *point,
    Point    *end_point1,
    Point    *end_point2 )
{
    Vector   d, v;
    Real     dist, len, v_dot_d, v_dot_v;

    SUB_POINTS( d, *end_point2, *end_point1 );

    len = DOT_VECTORS( d, d );
    if( len == 0.0 )
    {
        dist = distance_between_points( point, end_point1 );
    }
    else
    {
        SUB_POINTS( v, *point, *end_point1 );

        v_dot_d = DOT_VECTORS( v, d );
        v_dot_v = DOT_VECTORS( v, v );

        dist = v_dot_v - v_dot_d * v_dot_d / len;
        dist = sqrt( dist );
    }

    return( dist );
}
