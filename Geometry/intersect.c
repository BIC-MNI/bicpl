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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/intersect.c,v 1.6 1995-07-31 13:45:02 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : line_segment_intersects_plane
@INPUT      : p1
              p2
              plane_origin
              plane_normal
@OUTPUT     : intersection_point
@RETURNS    : TRUE if line segment intersects plane
@DESCRIPTION: Tests if a line segment from p1 to p2 intersects the plane,
              passing back the intersection point, if so.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : Jul 12, 1995    David MacDonald - rewrote to be clearer.
---------------------------------------------------------------------------- */

public  BOOLEAN   line_segment_intersects_plane(
    Point   *p1,
    Point   *p2,
    Point   *plane_origin,
    Vector  *plane_normal,
    Point   *intersection_point )
{
    Real     t, t1, t2;
    Vector   v1, v2;

    SUB_POINTS( v1, *p1, *plane_origin );
    SUB_POINTS( v2, *p2, *plane_origin );

    t1 = DOT_VECTORS( v1, *plane_normal );
    t2 = DOT_VECTORS( v2, *plane_normal );

    if( t1 == t2 )
        return( FALSE );

    if( t1 == 0.0 )
        *intersection_point = *p1;
    else if( t2 == 0.0 )
        *intersection_point = *p2;
    else
    {
        t = t1 / (t1 - t2);
 
        if( t < 0.0 || t > 1.0 )
            return( FALSE );

        INTERPOLATE_POINTS( *intersection_point, *p1, *p2, t );
    }

    return( TRUE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_nearest_point_on_lines
@INPUT      : origin1
              delta1
              origin2
              delta2
@OUTPUT     : nearest_pt
@RETURNS    : TRUE if near point found
@DESCRIPTION: Finds the nearest point on the first line to the second line,
              or returns FALSE if they are parallel.  If it can, passes back
              the intersection point.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  get_nearest_point_on_lines(
    Point   *origin1,
    Vector  *delta1,
    Point   *origin2,
    Vector  *delta2,
    Point   *nearest_point )
{
    BOOLEAN   intersects;
    Real      t, bottom;
    Real      d11, d12, d22;
    Real      o11, o12, o21, o22;

    d11 = DOT_VECTORS( *delta1, *delta1 );
    d12 = DOT_VECTORS( *delta1, *delta2 );
    d22 = DOT_VECTORS( *delta2, *delta2 );

    bottom = d11 * d22 - d12 * d12;

    intersects = (bottom != 0.0);

    if( intersects )
    {
        o11 = DOT_POINT_VECTOR( *origin1, *delta1 );
        o12 = DOT_POINT_VECTOR( *origin1, *delta2 );
        o21 = DOT_POINT_VECTOR( *origin2, *delta1 );
        o22 = DOT_POINT_VECTOR( *origin2, *delta2 );

        t = -(d22 * (o11 - o21) + d12 * (o22 - o12)) / bottom;

        GET_POINT_ON_RAY( *nearest_point, *origin1, *delta1, t );
    }

    return( intersects );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : clip_line_to_box
@INPUT      : origin
              direction
              x_min
              x_max
              y_min
              y_max
              z_min
              z_max
@OUTPUT     : t_min
              t_max
@RETURNS    : TRUE if line intersects box
@DESCRIPTION: Determines if the line intersects the box, and if so, passes
              back the min and max multiple of direction vector for the box
              intersection.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  clip_line_to_box(
    Point    *origin,
    Vector   *direction,
    Real     x_min,
    Real     x_max,
    Real     y_min,
    Real     y_max,
    Real     z_min,
    Real     z_max,
    Real     *t_min,
    Real     *t_max )
{
    BOOLEAN  first;
    int      c, first_ind;
    Real     dir, t1, t2, limits[2][N_DIMENSIONS];

    limits[0][X] = x_min;
    limits[1][X] = x_max;
    limits[0][Y] = y_min;
    limits[1][Y] = y_max;
    limits[0][Z] = z_min;
    limits[1][Z] = z_max;

    first = TRUE;
    *t_min = 0.0;
    *t_max = 0.0;

    for_less( c, 0, 3 )
    {
        dir = Vector_coord( *direction, c );

        if( dir != 0.0 )
        {
            if( dir < 0.0 )
                first_ind = 1;
            else
                first_ind = 0;

            t1 = (limits[first_ind][c] - Point_coord(*origin,c)) / dir;
            t2 = (limits[1-first_ind][c] - Point_coord(*origin,c)) / dir;

            if( first )
            {
                *t_min = t1;
                *t_max = t2;
                first = FALSE;
            }
            else
            {
                if( t1 > *t_min )
                    *t_min = t1;
                if( t2 < *t_max )
                    *t_max = t2;
            }
        }
        else
        {
            if( Point_coord(*origin,c) < limits[0][c] ||
                Point_coord(*origin,c) > limits[1][c] )
            {
                *t_min = 0.0;
                *t_max = -1.0;
                break;
            }
        }
    }

    return( *t_min <= *t_max );
}
