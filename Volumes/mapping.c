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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/mapping.c,v 1.34 2005-08-17 22:26:19 bert Exp $";
#endif

#define  DISTANCE_THRESHOLD  1.0e-10

/* ----------------------------- MNI Header -----------------------------------
@NAME       : clip_points
@INPUT      : n_dims
              sizes
              origin
              x_axis
              y_axis
              n_points
              points
@OUTPUT     : clipped_points
@RETURNS    : number of points output
@DESCRIPTION: Clips the points against the 2 * n_dims edges of the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : Mar. 6, 1995    D. MacDonald  -- removed complicated recursive
                              edge clipping, replaced with iterative edge
                              clipping.
---------------------------------------------------------------------------- */

static int  clip_points(
    int      n_dims,
    int      sizes[],
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    int      n_points,
    VIO_Real     points[][2],
    VIO_Real     clipped_points[][2] )
{
    int       i, c, n_input_points, n_output_points, this, lim;
    VIO_Real      prev_dist, dist, ratio, first_dist;
    VIO_Real      input_points[2*MAX_DIMENSIONS][2];
    VIO_Real      output_points[2*MAX_DIMENSIONS][2];

    for_less( i, 0, n_points )
    {
        input_points[i][0] = points[i][0];
        input_points[i][1] = points[i][1];
    }

    n_input_points = n_points;

    dist = 0.0;
    first_dist = 0.0;

    for_less( c, 0, n_dims )
    {
        for_less( lim, 0, 2 )
        {
            n_output_points = 0;
            if( n_input_points > 0 )
            {
                for_less( i, 0, n_input_points+1 )
                {
                    prev_dist = dist;
                    this = i % n_input_points;

                    if( i < n_input_points )
                    {
                        if( lim == 0 )
                        {
                            dist = origin[c] + input_points[i][0] * x_axis[c] +
                                           input_points[i][1] * y_axis[c] + 0.5;
                        }
                        else
                        {
                            dist = (VIO_Real) sizes[c] - 0.5 -
                               (origin[c] + input_points[i][0] * x_axis[c] +
                                            input_points[i][1] * y_axis[c]);
                        }
                    }
                    else
                        dist = first_dist;

                    if( i > 0 )
                    {
                        if( (dist < -DISTANCE_THRESHOLD &&
                            prev_dist > DISTANCE_THRESHOLD) ||
                            (dist > DISTANCE_THRESHOLD &&
                            prev_dist < -DISTANCE_THRESHOLD) )
                        {
                            ratio = prev_dist / (prev_dist - dist);
                            if( ratio > DISTANCE_THRESHOLD &&
                                ratio < 1.0 - DISTANCE_THRESHOLD )
                            {
                                output_points[n_output_points][0] =
                                  input_points[i-1][0] + ratio *
                                 (input_points[this][0] - input_points[i-1][0]);
                                output_points[n_output_points][1] =
                                  input_points[i-1][1] + ratio *
                                 (input_points[this][1] - input_points[i-1][1]);
                                ++n_output_points;
                            }
                        }
                    }
                    else
                        first_dist = dist;

                    if( dist >= 0.0 && i != n_input_points )
                    {
                        output_points[n_output_points][0] = input_points[i][0];
                        output_points[n_output_points][1] = input_points[i][1];
                        ++n_output_points;
                    }
                }
            }

            for_less( i, 0, n_output_points )
            {
                input_points[i][0] = output_points[i][0];
                input_points[i][1] = output_points[i][1];
            }

            n_input_points = n_output_points;
        }
    }

    for_less( i, 0, n_output_points )
    {
        clipped_points[i][0] = output_points[i][0];
        clipped_points[i][1] = output_points[i][1];
    }

    return( n_output_points );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_cross_section
@INPUT      : volume
              origin
              x_axis
              y_axis
@OUTPUT     : clipped_points
@RETURNS    : number of clipped points
@DESCRIPTION: Gets the points defining the outline of the cross section of the
              plane with the volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static int    get_cross_section(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     clipped_points[][2] )
{
    VIO_Real    points[4][2], voxel[MAX_DIMENSIONS];
    int     d, sizes[MAX_DIMENSIONS], n_dims, n_points;
    int     n_limits[MAX_DIMENSIONS], lim[MAX_DIMENSIONS];
    VIO_Real    x_pixel, y_pixel, x_min, x_max, y_min, y_max, dx, dy;
    VIO_BOOL first;

    get_volume_sizes( volume, sizes );
    n_dims = get_volume_n_dimensions( volume );

    for_less( d, 0, MAX_DIMENSIONS )
    {
        if( d < n_dims )
            n_limits[d] = 2;
        else
        {
            n_limits[d] = 1;
            sizes[d] = 1;
        }
    }

    first = TRUE;

    for_less( lim[0], 0, n_limits[0] )
    for_less( lim[1], 0, n_limits[1] )
    for_less( lim[2], 0, n_limits[2] )
    for_less( lim[3], 0, n_limits[3] )
    for_less( lim[4], 0, n_limits[4] )
    {
        for_less( d, 0, n_dims )
            voxel[d] = -0.5 + (VIO_Real) (sizes[d] * lim[d]);

        map_voxel_to_pixel( get_volume_n_dimensions(volume),
                            voxel, origin, x_axis, y_axis, &x_pixel, &y_pixel );

        if( first )
        {
            x_min = x_pixel;
            x_max = x_pixel;
            y_min = y_pixel;
            y_max = y_pixel;
            first = FALSE;
        }
        else
        {
            if( x_pixel < x_min )
                x_min = x_pixel;
            else if( x_pixel > x_max )
                x_max = x_pixel;
            if( y_pixel < y_min )
                y_min = y_pixel;
            else if( y_pixel > y_max )
                y_max = y_pixel;
        }
    }

    dx = x_max - x_min;
    dy = y_max - y_min;

    x_min -= dx;
    x_max += dx;
    y_min -= dy;
    y_max += dy;

    points[0][0] = x_min;
    points[0][1] = y_min;
    points[1][0] = x_max;
    points[1][1] = y_min;
    points[2][0] = x_max;
    points[2][1] = y_max;
    points[3][0] = x_min;
    points[3][1] = y_max;

    n_points = clip_points( n_dims, sizes, origin, x_axis, y_axis,
                            4, points, clipped_points );

    if( n_points == 1 || n_points == 2 )
    {
/*
        print_error( "N points = %d\n", n_points );
        handle_internal_error( "clipping" );
*/
        n_points = 0;
    }

    return( n_points );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_cross_section
@INPUT      : volume
              origin
              x_axis
              y_axis
@OUTPUT     : clipped_voxels
@RETURNS    : number of voxels
@DESCRIPTION: Gets the cross section of the plane with the volume, in terms
              of a list of voxels defining the vertices of the polygon.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI int    get_volume_cross_section(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     clipped_voxels[][MAX_DIMENSIONS] )
{
    VIO_Real    clipped_points[2*MAX_DIMENSIONS][2];
    int     i, c, n_dims, n_points;
    VIO_Real    real_origin[MAX_DIMENSIONS];
    VIO_Real    real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];

    get_mapping( volume, origin, x_axis, y_axis,
                 0.0, 0.0, 1.0, 1.0,
                 real_origin, real_x_axis, real_y_axis );

    n_points = get_cross_section( volume, real_origin, real_x_axis, real_y_axis,
                                  clipped_points );

    n_dims = get_volume_n_dimensions( volume );

    for_less( i, 0, n_points )
    {
        for_less( c, 0, n_dims )
        {
            clipped_voxels[i][c] = real_origin[c] +
                                   clipped_points[i][0] * real_x_axis[c] +
                                   clipped_points[i][1] * real_y_axis[c];
        }
    }

    return( n_points );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_slice_range
@INPUT      : volume
              origin
              x_axis
              y_axis
@OUTPUT     : x_pixel_start
              x_pixel_end
              y_pixel_start
              y_pixel_end
@RETURNS    : 
@DESCRIPTION: Gets the range of the cross section of the volume with the
              plane.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void    get_volume_slice_range(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     *x_pixel_start,
    VIO_Real     *x_pixel_end,
    VIO_Real     *y_pixel_start,
    VIO_Real     *y_pixel_end )
{
    VIO_Real    clipped_points[2*MAX_DIMENSIONS][2];
    int     i, n_points;

    n_points = get_cross_section( volume, origin, x_axis, y_axis,
                                  clipped_points );

    if( n_points == 0 )
    {
        *x_pixel_start = 1.0;
        *x_pixel_end = 0.0;
        *y_pixel_start = 1.0;
        *y_pixel_end = 0.0;
    }
    else
    {
        *x_pixel_start = clipped_points[0][0];
        *x_pixel_end = clipped_points[0][0];
        *y_pixel_start = clipped_points[0][1];
        *y_pixel_end = clipped_points[0][1];
        for_less( i, 0, n_points )
        {
            if( clipped_points[i][0] < *x_pixel_start )
                *x_pixel_start = clipped_points[i][0];
            else if( clipped_points[i][0] > *x_pixel_end )
                *x_pixel_end = clipped_points[i][0];
            if( clipped_points[i][1] < *y_pixel_start )
                *y_pixel_start = clipped_points[i][1];
            else if( clipped_points[i][1] > *y_pixel_end )
                *y_pixel_end = clipped_points[i][1];
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_mapping_range
@INPUT      : volume
              origin
              x_axis
              y_axis
              x_trans
              y_trans
              x_scale
              y_scale
@OUTPUT     : x_pixel_start
              x_pixel_end
              y_pixel_start
              y_pixel_end
@RETURNS    : 
@DESCRIPTION: Gets the pixel limits of the cross section, based on the scale
              and translation parameters.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void    get_volume_mapping_range(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    VIO_Real     x_trans,
    VIO_Real     y_trans,
    VIO_Real     x_scale,
    VIO_Real     y_scale,
    VIO_Real     *x_pixel_start,
    VIO_Real     *x_pixel_end,
    VIO_Real     *y_pixel_start,
    VIO_Real     *y_pixel_end )
{
    VIO_Real    real_origin[MAX_DIMENSIONS];
    VIO_Real    real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];

    get_mapping( volume, origin, x_axis, y_axis,
                 x_trans, y_trans, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

    get_volume_slice_range( volume, real_origin, real_x_axis, real_y_axis,
                            x_pixel_start, x_pixel_end,
                            y_pixel_start, y_pixel_end );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : clip_viewport_to_volume
@INPUT      : volume
              origin
              x_axis
              y_axis
@OUTPUT     : x_pixel_start
              x_pixel_end
              y_pixel_start
              y_pixel_end
@RETURNS    : 
@DESCRIPTION: Finds the pixel range of the volume cross section.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void    clip_viewport_to_volume(
    Volume   volume,
    VIO_Real     origin[],
    VIO_Real     x_axis[],
    VIO_Real     y_axis[],
    int      *x_pixel_start,
    int      *x_pixel_end,
    int      *y_pixel_start,
    int      *y_pixel_end )
{
    int     int_x_min, int_x_max, int_y_min, int_y_max;
    VIO_Real    x_min, x_max, y_min, y_max;

    get_volume_slice_range( volume, origin, x_axis, y_axis,
                            &x_min, &x_max, &y_min, &y_max );

    int_x_min = CEILING( x_min );
    int_x_max = FLOOR( x_max );
    int_y_min = CEILING( y_min );
    int_y_max = FLOOR( y_max );

    if( int_x_min > *x_pixel_start )
        *x_pixel_start = int_x_min;
    if( int_x_max < *x_pixel_end )
        *x_pixel_end = int_x_max;

    if( int_y_min > *y_pixel_start )
        *y_pixel_start = int_y_min;
    if( int_y_max < *y_pixel_end )
        *y_pixel_end = int_y_max;
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_mapping
@INPUT      : volume
              origin
              x_axis
              y_axis
              x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : pix_origin
              pix_x_axis
              pix_y_axis
@RETURNS    : 
@DESCRIPTION: Computes the mapping from pixels to voxels.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  get_mapping(
    Volume          volume,
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            pix_origin[],
    VIO_Real            pix_x_axis[],
    VIO_Real            pix_y_axis[] )
{
    int   c, n_dims;
    VIO_Real  separations[MAX_DIMENSIONS], x_len, y_len, comp;

    n_dims = get_volume_n_dimensions( volume );
    get_volume_separations( volume, separations );

    x_len = 0.0;
    y_len = 0.0;
    for_less( c, 0, n_dims )
    {
        comp = x_axis[c] * separations[c];
        x_len += comp * comp;
        comp = y_axis[c] * separations[c];
        y_len += comp * comp;
    }

    x_len = sqrt( x_len );
    if( x_len == 0.0 )
        x_len = 1.0;
    y_len = sqrt( y_len );
    if( y_len == 0.0 )
        y_len = 1.0;

    x_scale *= x_len;
    y_scale *= y_len;

    for_less( c, 0, n_dims )
    {
        pix_x_axis[c] = x_axis[c] / x_scale;
        pix_y_axis[c] = y_axis[c] / y_scale;
        pix_origin[c] = origin[c] - pix_x_axis[c] * x_translation
                                  - pix_y_axis[c] * y_translation;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : map_pixel_to_voxel
@INPUT      : x_pixel
              y_pixel
              n_dims
              voxel_origin
              voxel_x_axis
              voxel_y_axis
@OUTPUT     : voxel
@RETURNS    : 
@DESCRIPTION: Converts a pixel to a voxel.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  map_pixel_to_voxel(
    VIO_Real   x_pixel,
    VIO_Real   y_pixel,
    int    n_dims,
    VIO_Real   voxel_origin[],
    VIO_Real   voxel_x_axis[],
    VIO_Real   voxel_y_axis[],
    VIO_Real   voxel[] )
{
    int   c;

    for_less( c, 0, n_dims )
    {
        voxel[c] = voxel_origin[c] + (VIO_Real) x_pixel * voxel_x_axis[c] +
                                     (VIO_Real) y_pixel * voxel_y_axis[c];
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : dot
@INPUT      : n
              v1
              v2
@OUTPUT     : 
@RETURNS    : Dot product
@DESCRIPTION: Computes the dot product of 2 n-dimensional vectors.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static VIO_Real   dot(
    int    n,
    VIO_Real   v1[],
    VIO_Real   v2[] )
{
    int   i;
    VIO_Real  d;

    d = 0.0;
    for_less( i, 0, n )
        d += v1[i] * v2[i];

    return( d );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_two_axes_coordinates
@INPUT      : n
              vector
              x_axis
              y_axis
@OUTPUT     : x_pos
              y_pos
@RETURNS    : 
@DESCRIPTION: Computes the (x,y) coordinate given a vector and an x and y
              axis.  Finds the values such that
                      x_pos * x_axis + y_pos * y_axis = vector
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  get_two_axes_coordinates(
    int   n,
    VIO_Real  vector[],
    VIO_Real  x_axis[],
    VIO_Real  y_axis[],
    VIO_Real  *x_pos,
    VIO_Real  *y_pos )
{
    VIO_Real  x_dot_x, x_dot_v, x_dot_y, y_dot_v, y_dot_y, bottom;

    x_dot_x = dot( n, x_axis, x_axis );
    x_dot_v = dot( n, x_axis, vector );
    x_dot_y = dot( n, x_axis, y_axis );
    y_dot_y = dot( n, y_axis, y_axis );
    y_dot_v = dot( n, y_axis, vector );

    bottom = x_dot_x * y_dot_y - x_dot_y * x_dot_y;

    *x_pos = (x_dot_v * y_dot_y - x_dot_y * y_dot_v) / bottom;
    *y_pos = (y_dot_v * x_dot_x - x_dot_y * x_dot_v) / bottom;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : map_voxel_to_pixel
@INPUT      : n
              voxel
              origin
              x_axis
              y_axis
@OUTPUT     : x_pixel
              y_pixel
@RETURNS    : 
@DESCRIPTION: Converts a voxel coordinate to a pixel coordinate.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  map_voxel_to_pixel(
    int    n,
    VIO_Real   voxel[],
    VIO_Real   origin[],
    VIO_Real   x_axis[],
    VIO_Real   y_axis[],
    VIO_Real   *x_pixel,
    VIO_Real   *y_pixel )
{
    int   c;
    VIO_Real  vector[MAX_DIMENSIONS];

    for_less( c, 0, n )
        vector[c] = voxel[c] - origin[c];

    get_two_axes_coordinates( n, vector, x_axis, y_axis,
                              x_pixel, y_pixel );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_slice_pixel_to_voxel
@INPUT      : volume
              x_pixel
              y_pixel
              slice_position
              x_axis_index
              y_axis_index
              x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : pixel_slice_position
@RETURNS    : 
@DESCRIPTION: Converts pixel from the viewport to a voxel position.  The
              centres of pixels correspond to .0, as opposed to .5.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  convert_slice_pixel_to_voxel(
    Volume          volume,
    VIO_Real            x_pixel,
    VIO_Real            y_pixel,
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            voxel[] )
{
    int          n_dims;
    VIO_Real         real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];
    VIO_Real         real_origin[MAX_DIMENSIONS];

    get_mapping( volume, origin, x_axis, y_axis,
                 x_translation, y_translation, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

    n_dims = get_volume_n_dimensions( volume );

    map_pixel_to_voxel( x_pixel, y_pixel, n_dims, real_origin,
                        real_x_axis, real_y_axis, voxel );

    return( voxel_is_within_volume( volume, voxel ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_to_slice_pixel
@INPUT      : volume
              voxel_position
              x_axis_index
              y_axis_index
              x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : x_pixel
              y_pixel
@RETURNS    : 
@DESCRIPTION: Converts the voxel position (centres of voxels are at .0,
              not .5) to a pixel position in the viewport.  To get the
              integer pixel position, take ROUND( x_pixel ).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_voxel_to_slice_pixel(
    Volume          volume,
    VIO_Real            voxel[],
    VIO_Real            origin[],
    VIO_Real            x_axis[],
    VIO_Real            y_axis[],
    VIO_Real            x_translation,
    VIO_Real            y_translation,
    VIO_Real            x_scale,
    VIO_Real            y_scale,
    VIO_Real            *x_pixel,
    VIO_Real            *y_pixel )
{
    int          n_dims;
    VIO_Real         real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];
    VIO_Real         real_origin[MAX_DIMENSIONS];

    n_dims = get_volume_n_dimensions( volume );

    get_mapping( volume, origin, x_axis, y_axis,
                 x_translation, y_translation, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

    map_voxel_to_pixel( n_dims, voxel, real_origin,
                        real_x_axis, real_y_axis, x_pixel, y_pixel );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : resize_volume_slice
@INPUT      : old_x_viewport_size
              old_y_viewport_size
              old_used_x_viewport_size
              old_used_y_viewport_size
              new_x_viewport_size
              new_y_viewport_size
@MODIFIED   : x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : used_x_viewport_size
              used_y_viewport_size
@RETURNS    : 
@DESCRIPTION: Computes the new transformation required after resizing the
              viewport containing a slice.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  resize_volume_slice(
    int          old_x_viewport_size,
    int          old_y_viewport_size,
    int          old_used_x_viewport_size,
    int          old_used_y_viewport_size,
    int          new_x_viewport_size,
    int          new_y_viewport_size,
    VIO_Real         *x_translation,
    VIO_Real         *y_translation,
    VIO_Real         *x_scale,
    VIO_Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size )
{
    VIO_Real      x_scale_factor, y_scale_factor, scale_factor;

    if( old_used_x_viewport_size <= 0 )
        old_used_x_viewport_size = 1;
    if( old_used_y_viewport_size <= 0 )
        old_used_y_viewport_size = 1;

    x_scale_factor = (VIO_Real) new_x_viewport_size /
                     (VIO_Real) old_used_x_viewport_size;
    y_scale_factor = (VIO_Real) new_y_viewport_size /
                     (VIO_Real) old_used_y_viewport_size;
    scale_factor = MIN( x_scale_factor, y_scale_factor );

    if( used_x_viewport_size != NULL )
    {
        *used_x_viewport_size = ROUND( scale_factor *
                                       (VIO_Real) old_used_x_viewport_size);
    }
    if( used_y_viewport_size != NULL )
    {
        *used_y_viewport_size = ROUND( scale_factor *
                                       (VIO_Real) old_used_y_viewport_size );
    }

    scale_slice_about_viewport_centre( scale_factor,
                                       old_x_viewport_size, old_y_viewport_size,
                                       x_translation, y_translation,
                                       x_scale, y_scale );

    *x_translation += (VIO_Real) (new_x_viewport_size - old_x_viewport_size) / 2.0;
    *y_translation += (VIO_Real) (new_y_viewport_size - old_y_viewport_size) / 2.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : fit_volume_slice_to_viewport
@INPUT      : volume
              origin
              x_axis
              y_axis
              x_viewport_size
              y_viewport_size
              fraction_oversize
@OUTPUT     : x_translation
              y_translation
              x_scale
              y_scale
              used_x_viewport_size
              used_y_viewport_size
@RETURNS    : 
@DESCRIPTION: Computes the transformation required to fit a slice to the
              viewport.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  fit_volume_slice_to_viewport(
    Volume       volume,
    VIO_Real         origin[],
    VIO_Real         x_axis[],
    VIO_Real         y_axis[],
    int          x_viewport_size,
    int          y_viewport_size,
    VIO_Real         fraction_oversize,
    VIO_Real         *x_translation,
    VIO_Real         *y_translation,
    VIO_Real         *x_scale,
    VIO_Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size )
{
    VIO_Real         x_min, x_max, y_min, y_max;

    get_volume_mapping_range( volume, origin, x_axis, y_axis,
                              0.0, 0.0, 1.0, 1.0,
                              &x_min, &x_max, &y_min, &y_max );

    if( x_min == x_max || y_min == y_max )
    {
        *x_translation = 0.0;
        *y_translation = 0.0;
        *x_scale = 0.0;
        *y_scale = 0.0;
        return;
    }

    *x_scale = (VIO_Real) x_viewport_size / (x_max - x_min) /
               (1.0 + fraction_oversize);
    *y_scale = (VIO_Real) y_viewport_size / (y_max - y_min) /
               (1.0 + fraction_oversize);

    if( *x_scale < *y_scale )
        *y_scale = *x_scale;
    else
        *x_scale = *y_scale;

    if( used_x_viewport_size != NULL )
    {
        *used_x_viewport_size = (int) (*x_scale * (VIO_Real) (x_max - x_min) *
                                       (1.0 + fraction_oversize));
    }

    if( used_y_viewport_size != NULL )
    {
        *used_y_viewport_size = (int) (*y_scale * (VIO_Real) (y_max - y_min) *
                                       (1.0 + fraction_oversize));
    }

    *x_translation = ((VIO_Real) x_viewport_size - *x_scale * (x_max - x_min))/2.0 -
                     *x_scale * x_min;
    *y_translation = ((VIO_Real) y_viewport_size - *y_scale * (y_max - y_min))/2.0 -
                     *y_scale * y_min;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scale_slice_about_viewport_centre
@INPUT      : scale_factor
              x_viewport_size
              y_viewport_size
@OUTPUT     : x_translation
              y_translation
              x_scale
              y_scale
@RETURNS    : 
@DESCRIPTION: Modifies the scale and translation to scale a slice about
              the centre pixel of a viewport.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void   scale_slice_about_viewport_centre(
    VIO_Real        scale_factor,
    int         x_viewport_size,
    int         y_viewport_size,
    VIO_Real        *x_translation,
    VIO_Real        *y_translation,
    VIO_Real        *x_scale,
    VIO_Real        *y_scale )
{
    VIO_Real  x_centre, y_centre;

    x_centre = (VIO_Real) x_viewport_size / 2.0;
    y_centre = (VIO_Real) y_viewport_size / 2.0;

    *x_translation = x_centre - scale_factor * (x_centre - *x_translation);
    *y_translation = y_centre - scale_factor * (y_centre - *y_translation);
    *x_scale *= scale_factor;
    *y_scale *= scale_factor;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_is_within_volume
@INPUT      : volume
              voxel_position
@OUTPUT     : 
@RETURNS    : TRUE if voxel is within volume.
@DESCRIPTION: Determines if a voxel position is within the volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  voxel_is_within_volume(
    Volume   volume,
    VIO_Real     voxel_position[] )
{
    int      i, sizes[MAX_DIMENSIONS];
    VIO_BOOL  inside;

    inside = TRUE;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, get_volume_n_dimensions(volume) )
    {
        if( voxel_position[i] < -0.5 ||
            voxel_position[i] >= (VIO_Real) sizes[i] - 0.5 )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : int_voxel_is_within_volume
@INPUT      : volume
              indices
@OUTPUT     : 
@RETURNS    : TRUE if voxel within volume
@DESCRIPTION: Determines if the voxel with integer coordinates is within the
              volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  int_voxel_is_within_volume(
    Volume   volume,
    int      indices[] )
{
    int      i, sizes[MAX_DIMENSIONS];
    VIO_BOOL  inside;

    inside = TRUE;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, get_volume_n_dimensions(volume) )
    {
        if( indices[i] < 0 || indices[i] >= sizes[i] )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_real_to_int_voxel
@INPUT      : n_dimensions
              voxel
@OUTPUT     : int_voxel
@RETURNS    : 
@DESCRIPTION: Converts real valued voxel positions to integer positions, by
              rounding.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_real_to_int_voxel(
    int      n_dimensions,
    VIO_Real     voxel[],
    int      int_voxel[] )
{
    int   i;

    for_less( i, 0, n_dimensions )
        int_voxel[i] = ROUND( voxel[i] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_int_to_real_voxel
@INPUT      : n_dimensions
              int_voxel
@OUTPUT     : voxel
@RETURNS    : 
@DESCRIPTION: Converts int valued voxel positions to real positions.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Apr. 16, 1996  David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  convert_int_to_real_voxel(
    int      n_dimensions,
    int      int_voxel[],
    VIO_Real     voxel[] )
{
    int   i;

    for_less( i, 0, n_dimensions )
        voxel[i] = (VIO_Real) int_voxel[i];
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_contains_range
@INPUT      : volume
              int_voxel
              target_value
@OUTPUT     : 
@RETURNS    : TRUE if voxel contains this value
@DESCRIPTION: Determines if the voxel contains this value, by assuming
              trilinear interpolation between the 8 corner values of this
              voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  voxel_contains_range(
    Volume   volume,
    int      int_voxel[],
    VIO_Real     min_value,
    VIO_Real     max_value )
{               
    VIO_BOOL  less, greater;
    int      i, n_values;
    VIO_Real     value, values[1 << MAX_DIMENSIONS];

    less = FALSE;
    greater = FALSE;

    get_volume_value_hyperslab( volume,
                                int_voxel[0],
                                int_voxel[1],
                                int_voxel[2],
                                int_voxel[3],
                                int_voxel[4],
                                2, 2, 2, 2, 2, values );

    n_values = 1 << get_volume_n_dimensions( volume );

    for_less( i, 0, n_values )
    {
        value = values[i];

        if( value < min_value )
        {
            if( greater )
                return( TRUE );
            less = TRUE;
        }
        else if( value > max_value )
        {
            if( less )
                return( TRUE );
            greater = TRUE;
        }
        else
            return( TRUE );
    }

    return( FALSE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : volumes_are_same_grid
@INPUT      : volume1
              volume2
@OUTPUT     : 
@RETURNS    : TRUE or FALSE
@DESCRIPTION: Tests if the volumes represent the same tessellation, i.e.,
              the same number of voxels, and the same voxel to world transform.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : Aug. 1, 1995    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI VIO_BOOL  volumes_are_same_grid(
    Volume    volume1,
    Volume    volume2 )
{
    int                 c, n_dims, i, j;
    int                 sizes1[MAX_DIMENSIONS], sizes2[MAX_DIMENSIONS];
    VIO_General_transform   *transform1, *transform2;
    Transform           *lin_transform1, *lin_transform2;

    n_dims = get_volume_n_dimensions( volume1 );
    if( n_dims != get_volume_n_dimensions( volume2 ) )
        return( FALSE );

    get_volume_sizes( volume1, sizes1 );
    get_volume_sizes( volume2, sizes2 );

    for_less( c, 0, n_dims )
    {
        if( sizes1[c] != sizes2[c] )
            return( FALSE );
    }

    transform1 = get_voxel_to_world_transform( volume1 );
    transform2 = get_voxel_to_world_transform( volume2 );

    if( get_transform_type(transform1) != get_transform_type(transform2) )
        return( FALSE );

    /*--- for now, we will not support checking non-linear transforms for
          equality */

    if( get_transform_type(transform1) != LINEAR ||
        get_transform_type(transform2) != LINEAR )
        return( FALSE );

    lin_transform1 = get_linear_transform_ptr( transform1 );
    lin_transform2 = get_linear_transform_ptr( transform2 );

    for_less( i, 0, 4 )
    {
        for_less( j, 0, 4 )
        {
            if( Transform_elem(*lin_transform1,i,j) != 
                Transform_elem(*lin_transform2,i,j) )
            {
                return( FALSE );
            }
        }
    }

    return( TRUE );
}
