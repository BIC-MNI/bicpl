
#include  <mni.h>

private  void clip_one_edge(
    int      edge,
    int      sizes[],
    Real     origin[],
    Real     x_axis[],
    Real     y_axis[],
    Real     point[],
    BOOLEAN  first[],
    Real     prev_point[][2],
    Real     prev_dist[],
    int      *n_clipped_points,
    Real     clipped_points[][2] )
{
    int   axis;
    Real  dist, ratio, interpolated[2];

    if( edge == -1 )
    {
        clipped_points[*n_clipped_points][0] = point[0];
        clipped_points[*n_clipped_points][1] = point[1];
        ++(*n_clipped_points);
        return;
    }

    axis = edge / 2;
    if( edge % 2 == 0 )
    {
        dist = origin[axis] + point[0] * x_axis[axis] +
                              point[1] * y_axis[axis] + 0.5;
    }
    else
    {
        dist = (Real) sizes[axis] - 0.5 -
               (origin[axis] + point[0] * x_axis[axis] +
                               point[1] * y_axis[axis]);
    }

    if( !first[edge] && (dist != 0.0 && prev_dist[edge] != 0.0) &&
        (dist <= 0.0 && prev_dist[edge] >= 0.0 ||
         dist >= 0.0 && prev_dist[edge] <= 0.0) )
    {
        ratio = prev_dist[edge] / (prev_dist[edge] - dist);
        interpolated[0] = prev_point[edge][0] + ratio *
                          (point[0] - prev_point[edge][0]);
        interpolated[1] = prev_point[edge][1] + ratio *
                          (point[1] - prev_point[edge][1]);
        clip_one_edge( edge-1, sizes, origin, x_axis, y_axis, interpolated,
              first, prev_point, prev_dist, n_clipped_points, clipped_points );
    }

    if( dist >= 0.0 )
        clip_one_edge( edge-1, sizes, origin, x_axis, y_axis, point,
              first, prev_point, prev_dist, n_clipped_points, clipped_points );

    first[edge] = FALSE;
    prev_point[edge][0] = point[0];
    prev_point[edge][1] = point[1];
    prev_dist[edge] = dist;
}

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
    int       i, c, n_clipped_points;
    BOOLEAN   first[2*MAX_DIMENSIONS];
    Real      prev_point[2*MAX_DIMENSIONS][2];
    Real      prev_dist[2*MAX_DIMENSIONS];

    for_less( c, 0, 2 * n_dims )
        first[c] = TRUE;

    n_clipped_points = 0;

    for_less( i, 0, n_points+1 )
    {
        clip_one_edge( 2*n_dims-1, sizes, origin, x_axis, y_axis,
                       points[i%n_points], first, prev_point, prev_dist,
                       &n_clipped_points, clipped_points );
    }

    return( n_clipped_points );
}

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
    Real    points[4][2], clipped_points[2*MAX_DIMENSIONS][2];
    int     i, n_points, sizes[MAX_DIMENSIONS], n_dims;
    Real    x_min, x_max, y_min, y_max;

    get_volume_sizes( volume, sizes );
    points[0][0] = (Real) *x_pixel_start;
    points[0][1] = (Real) *y_pixel_start;
    points[1][0] = (Real) *x_pixel_end;
    points[1][1] = (Real) *y_pixel_start;
    points[2][0] = (Real) *x_pixel_end;
    points[2][1] = (Real) *y_pixel_end;
    points[3][0] = (Real) *x_pixel_start;
    points[3][1] = (Real) *y_pixel_end;

    n_dims = get_volume_n_dimensions( volume );

    n_points = clip_points( n_dims,
                            sizes, origin, x_axis, y_axis,
                            4, points, clipped_points );

    if( n_points > 2 * n_dims )
    {
        HANDLE_INTERNAL_ERROR( "clipping" );
    }

    if( n_points == 0 )
    {
        *x_pixel_start = 1;
        *x_pixel_end = 0;
        *y_pixel_start = 1;
        *y_pixel_end = 0;
    }
    else
    {
        x_min = clipped_points[0][0];
        x_max = clipped_points[0][0];
        y_min = clipped_points[0][1];
        y_max = clipped_points[0][1];
        for_less( i, 0, n_points )
        {
            if( clipped_points[i][0] < x_min )
                x_min = clipped_points[i][0];
            else if( clipped_points[i][0] > x_max )
                x_max = clipped_points[i][0];
            if( clipped_points[i][1] < y_min )
                y_min = clipped_points[i][1];
            else if( clipped_points[i][1] > y_max )
                y_max = clipped_points[i][1];
        }

        *x_pixel_start = CEILING( x_min );
        *x_pixel_end = FLOOR( x_max );
        *y_pixel_start = CEILING( y_min );
        *y_pixel_end = FLOOR( y_max );
    }
}

private  void  create_weighted_volume_slices(
    Volume          volume1,
    int             n_slices1,
    Real            **origins1,
    Real            x_axis1[],
    Real            y_axis1[],
    Real            weights1[],
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    int             n_slices2,
    Real            **origins2,
    Real            x_axis2[],
    Real            y_axis2[],
    Real            weights2[],
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    BOOLEAN         interpolation_flag,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    Data_types   volume1_data_type, volume2_data_type;
    void         *volume_data1, *volume_data2;
    Real         **real_origins1, **real_origins2;
    Real         real_x_axis1[MAX_DIMENSIONS], real_y_axis1[MAX_DIMENSIONS];
    Real         real_x_axis2[MAX_DIMENSIONS], real_y_axis2[MAX_DIMENSIONS];
    int          strides1[MAX_DIMENSIONS], strides2[MAX_DIMENSIONS];
    int          n_dimensions1, n_dimensions2;
    int          axis, x_size, y_size, s, c;
    int          x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;
    int          sizes1[MAX_DIMENSIONS], sizes2[MAX_DIMENSIONS];

    x_pixel_start = 0;
    x_pixel_end = x_viewport_size - 1;
    y_pixel_start = 0;
    y_pixel_end = y_viewport_size - 1;

    n_dimensions1 = get_volume_n_dimensions( volume1 );

    ALLOC2D( real_origins1, n_slices1, n_dimensions1 );

    for_less( s, 0, n_slices1 )
    {
        get_mapping( volume1, origins1[s], x_axis1, y_axis1,
                     x_translation1, x_scale1,
                     y_translation1, y_scale1,
                     real_origins1[s], real_x_axis1, real_y_axis1 );

        clip_viewport_to_volume( volume1, real_origins1[s],
                                 real_x_axis1, real_y_axis1,
                                 &x_pixel_start, &x_pixel_end,
                                 &y_pixel_start, &y_pixel_end );
    }

    if( volume2 != (Volume) NULL )
    {
        n_dimensions2 = get_volume_n_dimensions( volume2 );

        ALLOC2D( real_origins2, n_slices2, n_dimensions2 );

        for_less( s, 0, n_slices2 )
        {
            get_mapping( volume2, origins2[s], x_axis2, y_axis2,
                         x_translation2, x_scale2,
                         y_translation2, y_scale2,
                         real_origins2[s], real_x_axis2, real_y_axis2 );

            clip_viewport_to_volume( volume2, real_origins2[s],
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

    if( x_size > 0 && y_size > 0 )
    {
        for_less( s, 0, n_slices1 )
        {
            for_less( c, 0, n_dimensions1 )
            {
                real_origins1[s][c] += (Real) x_pixel_start * real_x_axis1[c] +
                                       (Real) y_pixel_start * real_y_axis1[c];
            }
        }

        GET_VOXEL_PTR( volume_data1, volume1, 0, 0, 0, 0, 0 );
        volume1_data_type = get_volume_data_type( volume1 );

        get_volume_sizes( volume1, sizes1 );

        strides1[n_dimensions1-1] = 1;
        for( axis = n_dimensions1 - 2;  axis >= 0;  --axis )
             strides1[axis] = strides1[axis+1] * sizes1[axis+1];

        if( volume2 != (Volume) NULL )
        {
            for_less( s, 0, n_slices2 )
            {
                for_less( c, 0, n_dimensions2 )
                {
                    real_origins2[s][c] +=
                              (Real) x_pixel_start * real_x_axis2[c] +
                              (Real) y_pixel_start * real_y_axis2[c];
                }
            }

            GET_VOXEL_PTR( volume_data2, volume2, 0, 0, 0, 0, 0 );
            volume2_data_type = get_volume_data_type( volume2 );
            get_volume_sizes( volume2, sizes2 );

            strides2[n_dimensions2-1] = 1;
            for( axis = n_dimensions2 - 2;  axis >= 0;  --axis )
                 strides2[axis] = strides2[axis+1] * sizes2[axis+1];
        }
        else
            volume_data2 = (void *) NULL;

        render_volume_to_slice( n_dimensions1, sizes1, volume_data1,
                                volume1_data_type, n_slices1, weights1,
                                strides1, real_origins1,
                                real_x_axis1, real_y_axis1,
                                n_dimensions2, sizes2, volume_data2,
                                volume2_data_type, n_slices2, weights2,
                                strides2, real_origins2,
                                real_x_axis2, real_y_axis2,
                                interpolation_flag, cmode_colour_map,
                                rgb_colour_map, empty_colour, pixels );
    }

    FREE2D( real_origins1 );
    if( volume2 != (Volume) NULL )
        FREE2D( real_origins2 );
}

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
            print(
              "If volume not 3D, can only do nearest neighbour filtering.\n" );
            return( FALSE );
        }

        get_volume_separations( volume, separations );
        for_less( c, 0, N_DIMENSIONS )
        {
            a1 = (c + 1) % N_DIMENSIONS;
            a2 = (c + 2) % N_DIMENSIONS;
            direction[c] = x_axis[a1] * y_axis[a2] -
                           x_axis[a2] * y_axis[a1];
            direction[c] *= separations[a1] * separations[a2] / separations[c];
        }
    }

    *n_slices = get_slice_weights_for_filter( volume, position, direction,
                                              filter_type, filter_width,
                                              origins, weights );

    return( TRUE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_volume_slice
@INPUT      : filter_type      - filter_type, usually NEAREST_NEIGHBOUR
              filter_width     - width of filter, used for BOX, TRIANGLE, GAUSS
              volume1          - the volume to create a slice for
              slice_position1  - the voxel coordinate of the slice
              x_translation1   - pixel translation for viewing
              y_translation1   - pixel translation for viewing
              x_scale1         - pixel zoom for viewing
              y_scale1         - pixel zoom for viewing
              volume2          - second volume to be merged with first, or null
              slice_position2  - the voxel coordinate of the slice
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
    Pixel_types     pixel_type,
    BOOLEAN         interpolation_flag,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    int          n_slices1, n_slices2;
    Real         **positions1, **positions2, *weights1, *weights2;

    if( !get_filter_slices( volume1, slice_position1, x_axis1, y_axis1,
                            filter_type1, filter_width1, &n_slices1,
                            &positions1, &weights1 ) )
    {
        modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
        return;
    }

    if( volume2 != (Volume) NULL )
    {
        if( !get_filter_slices( volume2, slice_position2, x_axis2, y_axis2,
                                filter_type2, filter_width2, &n_slices2,
                                &positions2, &weights2 ) )
        {
            modify_pixels_size( n_pixels_alloced, pixels, 0, 0, pixel_type );
            return;
        }
    }

    create_weighted_volume_slices( volume1, n_slices1,
                                   positions1, x_axis1, y_axis1,
                                   weights1,
                                   x_translation1, y_translation1,
                                   x_scale1, y_scale1,
                                   volume2, n_slices2,
                                   positions2, x_axis2, y_axis2,
                                   weights2,
                                   x_translation2, y_translation2,
                                   x_scale2, y_scale2,
                                   x_viewport_size, y_viewport_size,
                                   pixel_type, interpolation_flag,
                                   cmode_colour_map, rgb_colour_map,
                                   empty_colour, n_pixels_alloced, pixels );

    FREE2D( positions1 );
    FREE( weights1 );

    if( volume2 != (Volume) NULL )
    {
        FREE2D( positions2 );
        FREE( weights2 );
    }
}
