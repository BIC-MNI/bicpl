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
#include  <vols.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/create_slice.c,v 1.40 1995-10-19 15:48:13 david Exp $";
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

private  int  clip_points(
    int      n_dims,
    int      sizes[],
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    int      n_points,
    Real     points[][2],
    Real     clipped_points[][2] )
{
    int       i, c, n_input_points, n_output_points, this, lim;
    Real      prev_dist, dist, ratio, first_dist;
    Real      input_points[2*MAX_DIMENSIONS][2];
    Real      output_points[2*MAX_DIMENSIONS][2];

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
                            dist = (Real) sizes[c] - 0.5 -
                               (origin[c] + input_points[i][0] * x_axis[c] +
                                            input_points[i][1] * y_axis[c]);
                        }
                    }
                    else
                        dist = first_dist;

                    if( i > 0 )
                    {
                        if( dist < -DISTANCE_THRESHOLD &&
                            prev_dist > DISTANCE_THRESHOLD ||
                            dist > DISTANCE_THRESHOLD &&
                            prev_dist < -DISTANCE_THRESHOLD )
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

private  int    get_cross_section(
    Volume   volume,
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    Real     clipped_points[][2] )
{
    Real    points[4][2], voxel[MAX_DIMENSIONS];
    int     d, sizes[MAX_DIMENSIONS], n_dims, n_points;
    int     n_limits[MAX_DIMENSIONS], lim[MAX_DIMENSIONS];
    Real    x_pixel, y_pixel, x_min, x_max, y_min, y_max, dx, dy;
    BOOLEAN first;

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
            voxel[d] = -0.5 + (Real) (sizes[d] * lim[d]);

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

public  int    get_volume_cross_section(
    Volume   volume,
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    Real     clipped_voxels[][MAX_DIMENSIONS] )
{
    Real    clipped_points[2*MAX_DIMENSIONS][2];
    int     i, c, n_dims, n_points;
    Real    real_origin[MAX_DIMENSIONS];
    Real    real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];

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

private  void    get_volume_slice_range(
    Volume   volume,
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    Real     *x_pixel_start,
    Real     *x_pixel_end,
    Real     *y_pixel_start,
    Real     *y_pixel_end )
{
    Real    clipped_points[2*MAX_DIMENSIONS][2];
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

public  void    get_volume_mapping_range(
    Volume   volume,
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    Real     x_trans,
    Real     y_trans,
    Real     x_scale,
    Real     y_scale,
    Real     *x_pixel_start,
    Real     *x_pixel_end,
    Real     *y_pixel_start,
    Real     *y_pixel_end )
{
    Real    real_origin[MAX_DIMENSIONS];
    Real    real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];

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

private  void    clip_viewport_to_volume(
    Volume   volume,
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    int      *x_pixel_start,
    int      *x_pixel_end,
    int      *y_pixel_start,
    int      *y_pixel_end )
{
    int     int_x_min, int_x_max, int_y_min, int_y_max;
    Real    x_min, x_max, y_min, y_max;

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

private  void  set_pixel_range(
    Volume          volume1,
    int             n_slices1,
    Real            **origins1,
    Real            x_axis1[],
    Real            y_axis1[],
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    int             n_slices2,
    Real            **origins2,
    Real            x_axis2[],
    Real            y_axis2[],
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    Real            real_x_axis1[],
    Real            real_y_axis1[],
    Real            ***real_origins1,
    Real            real_x_axis2[],
    Real            real_y_axis2[],
    Real            ***real_origins2,
    Pixel_types     pixel_type,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    int          n_dimensions1, n_dimensions2, s, x_size, y_size;
    int          x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;

    x_pixel_start = 0;
    x_pixel_end = x_viewport_size - 1;
    y_pixel_start = 0;
    y_pixel_end = y_viewport_size - 1;

    n_dimensions1 = get_volume_n_dimensions( volume1 );

    ALLOC2D( *real_origins1, n_slices1, n_dimensions1 );

    for_less( s, 0, n_slices1 )
    {
        get_mapping( volume1, origins1[s], x_axis1, y_axis1,
                     x_translation1, y_translation1, x_scale1, y_scale1,
                     (*real_origins1)[s], real_x_axis1, real_y_axis1 );

        clip_viewport_to_volume( volume1, (*real_origins1)[s],
                                 real_x_axis1, real_y_axis1,
                                 &x_pixel_start, &x_pixel_end,
                                 &y_pixel_start, &y_pixel_end );
    }

    if( volume2 != NULL )
    {
        n_dimensions2 = get_volume_n_dimensions( volume2 );

        ALLOC2D( *real_origins2, n_slices2, n_dimensions2 );

        for_less( s, 0, n_slices2 )
        {
            get_mapping( volume2, origins2[s], x_axis2, y_axis2,
                         x_translation2, y_translation2, x_scale2, y_scale2,
                         (*real_origins2)[s], real_x_axis2, real_y_axis2 );

            clip_viewport_to_volume( volume2, (*real_origins2)[s],
                                     real_x_axis2, real_y_axis2,
                                     &x_pixel_start, &x_pixel_end,
                                     &y_pixel_start, &y_pixel_end );
        }
    }

    x_size = x_pixel_end - x_pixel_start + 1;
    y_size = y_pixel_end - y_pixel_start + 1;
    if( x_size < 0 )
        x_size = 0;
    if( y_size < 0 )
        y_size = 0;

    modify_pixels_size( n_pixels_alloced, pixels, x_size, y_size, pixel_type );
    pixels->x_position = x_pixel_start;
    pixels->y_position = y_pixel_start;
    pixels->x_zoom = 1.0;
    pixels->y_zoom = 1.0;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_weighted_volume_slices
@INPUT      : volume1,
              n_slices1   - number of parallel slices to weighted-sum.
              origins1
              x_axis1
              y_axis1
              weights1
              x_translation1
              y_translation1
              x_scale1
              y_scale1
              volume2,
              n_slices2   - number of parallel slices to weighted-sum.
              origins2
              x_axis2
              y_axis2
              weights2
              x_translation2
              y_translation2
              x_scale2
              y_scale2
              x_viewport_size
              y_viewport_size
              pixel_type
              degrees_continuity
              cmode_colour_map
              rgb_colour_map
              empty_colour
              n_pixels_alloced
@OUTPUT     : pixels
@RETURNS    : 
@DESCRIPTION: Renders a volume or a merge of 2 volumes to pixels.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  create_weighted_volume_slices(
    Volume          volume1,
    int             n_slices1,
    Real            **origins1,
    Real            x_axis1[],
    Real            y_axis1[],
    Real            weights1[],
    Volume          volume2,
    int             n_slices2,
    Real            **origins2,
    Real            x_axis2[],
    Real            y_axis2[],
    Real            weights2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    void            *render_storage,
    pixels_struct   *pixels )
{
    Data_types   volume1_data_type, volume2_data_type;
    void         *volume_data1, *volume_data2;
    int          strides1[MAX_DIMENSIONS], strides2[MAX_DIMENSIONS];
    int          n_dimensions1, n_dimensions2;
    int          axis, s, c;
    int          sizes1[MAX_DIMENSIONS], sizes2[MAX_DIMENSIONS];

    if( pixels->x_size > 0 && pixels->y_size > 0 )
    {
        n_dimensions1 = get_volume_n_dimensions( volume1 );

        for_less( s, 0, n_slices1 )
        {
            for_less( c, 0, n_dimensions1 )
            {
                origins1[s][c] += (Real) pixels->x_position * x_axis1[c] +
                                  (Real) pixels->y_position * y_axis1[c];
            }
        }

        if( !volume1->is_cached_volume )
            GET_VOXEL_PTR( volume_data1, volume1, 0, 0, 0, 0, 0 );
        volume1_data_type = get_volume_data_type( volume1 );

        get_volume_sizes( volume1, sizes1 );

        strides1[n_dimensions1-1] = 1;
        for_down( axis, n_dimensions1 - 2,  0 )
             strides1[axis] = strides1[axis+1] * sizes1[axis+1];

        if( volume2 != NULL )
        {
            n_dimensions2 = get_volume_n_dimensions( volume2 );

            for_less( s, 0, n_slices2 )
            {
                for_less( c, 0, n_dimensions2 )
                {
                    origins2[s][c] += (Real) pixels->x_position * x_axis2[c] +
                                      (Real) pixels->y_position * y_axis2[c];
                }
            }

            if( !volume2->is_cached_volume )
                GET_VOXEL_PTR( volume_data2, volume2, 0, 0, 0, 0, 0 );
            volume2_data_type = get_volume_data_type( volume2 );
            get_volume_sizes( volume2, sizes2 );

            strides2[n_dimensions2-1] = 1;
            for_down( axis, n_dimensions2 - 2, 0 )
                 strides2[axis] = strides2[axis+1] * sizes2[axis+1];
        }
        else
            volume_data2 = NULL;

        if( x_pixel_start > x_pixel_end || y_pixel_start > y_pixel_end )
        {
            x_pixel_start = 0;
            x_pixel_end = pixels->x_size-1;
            y_pixel_start = 0;
            y_pixel_end = pixels->y_size-1;
        }
        else
        {
            if( x_pixel_start < 0 )
                x_pixel_start = 0;
            if( x_pixel_end >= pixels->x_size )
                x_pixel_end = pixels->x_size-1;

            if( y_pixel_start < 0 )
                y_pixel_start = 0;
            if( y_pixel_end >= pixels->y_size )
                y_pixel_end = pixels->y_size-1;
        }

        if( volume1->is_cached_volume ||
            (volume2 != NULL && volume2->is_cached_volume) ||
            degrees_continuity != -1 && n_slices1 == 1 &&
            (volume_data2 == NULL || n_slices2 == 1) )
        {
            interpolate_volume_to_slice( volume1, n_dimensions1, sizes1,
                                         origins1[0],
                                         x_axis1, y_axis1,
                                         volume2, n_dimensions2, sizes2,
                                         (volume_data2 == NULL) ? NULL :
                                         origins2[0],
                                         x_axis2, y_axis2,
                                         x_pixel_start, x_pixel_end,
                                         y_pixel_start, y_pixel_end,
                                         degrees_continuity,
                                         cmode_colour_map, rgb_colour_map,
                                         empty_colour, pixels );
        }
        else
        {
            render_volume_to_slice( n_dimensions1, sizes1, volume_data1,
                                    volume1_data_type, n_slices1, weights1,
                                    strides1, origins1, x_axis1, y_axis1,
                                    n_dimensions2, sizes2, volume_data2,
                                    volume2_data_type, n_slices2, weights2,
                                    strides2, origins2, x_axis2, y_axis2,
                                    x_pixel_start, x_pixel_end,
                                    y_pixel_start, y_pixel_end,
                                    cmode_colour_map,
                                    rgb_colour_map, empty_colour,
                                    render_storage, pixels );
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_filter_slices
@INPUT      : volume
              position
              x_axis
              y_axis
              filter_type
              filter_width
@OUTPUT     : n_slices
              origins
              weights
@RETURNS    : TRUE if successful
@DESCRIPTION: Converts the filter type and width to a list of parallel slices
              to be used in a weighted-sum rendering.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  BOOLEAN  get_filter_slices(
    Volume          volume,
    Real            position[],
    Real            x_axis[],
    Real            y_axis[],
    Filter_types    filter_type,
    Real            filter_width,
    int             *n_slices,
    Real            ***origins,
    Real            **weights )
{
    int    c, a1, a2;
    Real   direction[N_DIMENSIONS], separations[MAX_DIMENSIONS];

    if( filter_type != NEAREST_NEIGHBOUR )
    {
        if( get_volume_n_dimensions( volume ) != N_DIMENSIONS )
        {
            print_error(
              "If volume not 3D, can only do nearest neighbour filtering.\n" );
            return( FALSE );
        }

        get_volume_separations( volume, separations );
        for_less( c, 0, N_DIMENSIONS )
        {
            a1 = (c + 1) % N_DIMENSIONS;
            a2 = (c + 2) % N_DIMENSIONS;
            direction[c] = x_axis[a1] * y_axis[a2] - x_axis[a2] * y_axis[a1];
            direction[c] *= ABS( separations[a1] * separations[a2] /
                                 separations[c] );
        }
    }

    *n_slices = get_slice_weights_for_filter( volume, position, direction,
                                              filter_type, filter_width,
                                              origins, weights );

    return( TRUE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_volume_slice
@INPUT      : volume1          - the volume to create a slice for
              filter_type1     - filter_type, usually NEAREST_NEIGHBOUR
              filter_width1    - width of filter, used for BOX, TRIANGLE, GAUSS
              slice_position1  - the voxel coordinate of the slice
              x_axis1          - the x axis in voxels
              y_axis1          - the y axis in voxels
              x_translation1   - pixel translation for viewing
              y_translation1   - pixel translation for viewing
              x_scale1         - pixel zoom for viewing
              y_scale1         - pixel zoom for viewing
              volume2          - second volume to be merged with first, or null
              filter_type2     - filter_type, usually NEAREST_NEIGHBOUR
              filter_width2    - width of filter, used for BOX, TRIANGLE, GAUSS
              slice_position2  - the voxel coordinate of the slice
              x_axis2          - the x axis in voxels
              y_axis2          - the y axis in voxels
              x_translation2   - pixel translation for viewing
              y_translation2   - pixel translation for viewing
              x_scale2         - pixel zoom for viewing
              y_scale2         - pixel zoom for viewing
              x_axis_index     - X,Y, or Z
              y_axis_index     - X,Y, or Z
              axis_index       - X,Y, or Z
              x_viewport_size  - will be clipped to this size
              y_viewport_size  - will be clipped to this size
              pixel_type       - RGB_PIXEL or COLOUR_INDEX_PIXEL for rgb/cmap
              interpolation_flag - ignored for now
              cmode_colour_map - if pixel_type == COLOUR_INDEX_PIXEL, then
                          2d array of 16 bit colour indices for merged slices,
                          or pointer to 1d array of colour indices for volume1
              rgb_colour_map - if pixel_type == RGB_PIXEL, then
                          2d array of 24 bit colours for merged slices,
                          or pointer to 1d array of colours for volume1
@OUTPUT     : n_pixels_alloced - a pointer to the size alloced.  Before first
                          call, set size alloced to zero, and all calls,
                          pass pointer to size alloced, and pointer to pixels.
              pixels           - 2d pixels array created, and realloced as
                                 necessary, assuming, n_pixels_alloced is a
                                 pointer to the current alloc size of pixels.
@RETURNS    : 
@DESCRIPTION: Creates a slice of one volume or merged slice of two, suitable
              for graphics display.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  create_volume_slice(
    Volume          volume1,
    Filter_types    filter_type1,
    Real            filter_width1,
    Real            slice_position1[],
    Real            x_axis1[],
    Real            y_axis1[],
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    Filter_types    filter_type2,
    Real            filter_width2,
    Real            slice_position2[],
    Real            x_axis2[],
    Real            y_axis2[],
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    Pixel_types     pixel_type,
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    void            *render_storage,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    int          n_slices1, n_slices2;
    Real         **positions1, **positions2, *weights1, *weights2;
    Real         **real_origins1, **real_origins2;
    Real         real_x_axis1[MAX_DIMENSIONS], real_y_axis1[MAX_DIMENSIONS];
    Real         real_x_axis2[MAX_DIMENSIONS], real_y_axis2[MAX_DIMENSIONS];

    if( !get_filter_slices( volume1, slice_position1, x_axis1, y_axis1,
                            filter_type1, filter_width1, &n_slices1,
                            &positions1, &weights1 ) )
    {
        modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
        return;
    }

    if( volume2 != NULL )
    {
        if( !get_filter_slices( volume2, slice_position2, x_axis2, y_axis2,
                                filter_type2, filter_width2, &n_slices2,
                                &positions2, &weights2 ) )
        {
            modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
            return;
        }
    }

    set_pixel_range( volume1, n_slices1, positions1, x_axis1, y_axis1,
                     x_translation1, y_translation1, x_scale1, y_scale1,
                     volume2, n_slices2, positions2, x_axis2, y_axis2,
                     x_translation2, y_translation2, x_scale2, y_scale2,
                     x_viewport_size, y_viewport_size,
                     real_x_axis1, real_y_axis1, &real_origins1,
                     real_x_axis2, real_y_axis2, &real_origins2,
                     pixel_type, n_pixels_alloced, pixels );

    create_weighted_volume_slices( volume1, n_slices1,
                                   real_origins1, real_x_axis1, real_y_axis1,
                                   weights1,
                                   volume2, n_slices2,
                                   real_origins2, real_x_axis2, real_y_axis2,
                                   weights2,
                                   x_pixel_start, x_pixel_end,
                                   y_pixel_start, y_pixel_end,
                                   degrees_continuity,
                                   cmode_colour_map, rgb_colour_map,
                                   empty_colour, render_storage,
                                   pixels );

    if( volume2 != NULL )
    {
        FREE2D( positions2 );
        FREE( weights2 );
        FREE2D( real_origins2 );
    }

    FREE2D( positions1 );
    FREE( weights1 );
    FREE2D( real_origins1 );
}

public  void  set_volume_slice_pixel_range(
    Volume          volume1,
    Filter_types    filter_type1,
    Real            filter_width1,
    Real            slice_position1[],
    Real            x_axis1[],
    Real            y_axis1[],
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    Filter_types    filter_type2,
    Real            filter_width2,
    Real            slice_position2[],
    Real            x_axis2[],
    Real            y_axis2[],
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    int          n_slices1, n_slices2;
    Real         **positions1, **positions2, *weights1, *weights2;
    Real         **real_origins1, **real_origins2;
    Real         real_x_axis1[MAX_DIMENSIONS], real_y_axis1[MAX_DIMENSIONS];
    Real         real_x_axis2[MAX_DIMENSIONS], real_y_axis2[MAX_DIMENSIONS];

    if( !get_filter_slices( volume1, slice_position1, x_axis1, y_axis1,
                            filter_type1, filter_width1, &n_slices1,
                            &positions1, &weights1 ) )
    {
        modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
        return;
    }

    if( volume2 != NULL )
    {
        if( !get_filter_slices( volume2, slice_position2, x_axis2, y_axis2,
                                filter_type2, filter_width2, &n_slices2,
                                &positions2, &weights2 ) )
        {
            modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
            return;
        }
    }

    set_pixel_range( volume1, n_slices1, positions1, x_axis1, y_axis1,
                     x_translation1, y_translation1, x_scale1, y_scale1,
                     volume2, n_slices2, positions2, x_axis2, y_axis2,
                     x_translation2, y_translation2, x_scale2, y_scale2,
                     x_viewport_size, y_viewport_size,
                     real_x_axis1, real_y_axis1, &real_origins1,
                     real_x_axis2, real_y_axis2, &real_origins2,
                     pixel_type, n_pixels_alloced, pixels );

    if( volume2 != NULL )
    {
        FREE2D( positions2 );
        FREE( weights2 );
        FREE2D( real_origins2 );
    }

    FREE2D( positions1 );
    FREE( weights1 );
    FREE2D( real_origins1 );
}
