
#include  <def_mni.h>

private  void  get_mapping(
    Volume          volume,
    Real            x_translation,
    Real            x_scale,
    int             x_axis_index,
    Real            y_translation,
    Real            y_scale,
    int             y_axis_index,
    Real            *x_start,
    Real            *x_end,
    Real            *x_delta,
    Real            *x_offset,
    Real            *y_start,
    Real            *y_end,
    Real            *y_delta,
    Real            *y_offset )
{
    *x_delta = x_scale * volume->separation[x_axis_index];
    *x_offset = x_translation;
    if( *x_delta >= 0.0 )
    {
        *x_start = -0.5;
        *x_end = (Real) volume->sizes[x_axis_index] - 0.5;
    }
    else
    {
        *x_end = -0.5;
        *x_start = (Real) volume->sizes[x_axis_index] - 0.5;
    }

    *y_delta = y_scale * volume->separation[y_axis_index];
    *y_offset = y_translation;

    if( *y_delta >= 0.0 )
    {
        *y_start = -0.5;
        *y_end = (Real) volume->sizes[y_axis_index] - 0.5;
    }
    else
    {
        *y_end = -0.5;
        *y_start = (Real) volume->sizes[y_axis_index] - 0.5;
    }
}

public  void  clip_slice_to_viewport(
    Volume          volume,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    int             x_axis_index,
    int             y_axis_index,
    int             x_viewport_size,
    int             y_viewport_size,
    int             *x_pixel_start,
    int             *x_pixel_end,
    int             *y_pixel_start,
    int             *y_pixel_end )
{
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index,
                 y_translation, y_scale, y_axis_index,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    clip_slice_to_pixel_range( x_viewport_size, y_viewport_size,
                               x_offset, y_offset,
                               x_delta, y_delta,
                               x_start, y_start,
                               x_end, y_end,
                               x_pixel_start, y_pixel_start,
                               x_pixel_end, y_pixel_end );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_volume_slice
@INPUT      : volume1          - the volume to create a slice for
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

private  void  create_weighted_volume_slices(
    int             n_slices,
    Volume          volume1,
    Real            slice_positions1[],
    Real            weights1[],
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    Real            slice_positions2[],
    Real            weights2[],
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_axis_index,
    int             y_axis_index,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    Boolean         interpolation_flag,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    Data_types   volume2_data_type;
    void         **slice_start1, **slice_start2;
    int          strides1[N_DIMENSIONS], strides2[N_DIMENSIONS];
    int          indices[N_DIMENSIONS];
    int          axis, x_size, y_size, s;
    int          x_pixel_start, x_pixel_end, y_pixel_start, y_pixel_end;
    int          x_pixel_start2, x_pixel_end2, y_pixel_start2, y_pixel_end2;
    Real         x_delta1, x_start1, x_end1, y_delta1, y_start1, y_end1;
    Real         x_offset1, y_offset1;
    Real         x_delta2, x_start2, x_end2, y_delta2, y_start2, y_end2;
    Real         x_offset2, y_offset2;

    get_mapping( volume1,
                 x_translation1, x_scale1, x_axis_index,
                 y_translation1, y_scale1, y_axis_index,
                 &x_start1, &x_end1, &x_delta1, &x_offset1,
                 &y_start1, &y_end1, &y_delta1, &y_offset1 );

    clip_slice_to_pixel_range( x_viewport_size, y_viewport_size,
                               x_offset1, y_offset1,
                               x_delta1, y_delta1,
                               x_start1, y_start1,
                               x_end1, y_end1,
                               &x_pixel_start, &y_pixel_start,
                               &x_pixel_end, &y_pixel_end );

    if( volume2 != (Volume) NULL )
    {
        get_mapping( volume2,
                     x_translation2, x_scale2, x_axis_index,
                     y_translation2, y_scale2, y_axis_index,
                     &x_start2, &x_end2, &x_delta2, &x_offset2,
                     &y_start2, &y_end2, &y_delta2, &y_offset2 );

        clip_slice_to_pixel_range( x_viewport_size, y_viewport_size,
                                   x_offset2, y_offset2,
                                   x_delta2, y_delta2,
                                   x_start2, y_start2,
                                   x_end2, y_end2,
                                   &x_pixel_start2, &y_pixel_start2,
                                   &x_pixel_end2, &y_pixel_end2 );

        if( x_pixel_start2 > x_pixel_start )
            x_pixel_start = x_pixel_start2;
        if( x_pixel_end2 < x_pixel_end )
            x_pixel_end = x_pixel_end2;

        if( y_pixel_start2 > y_pixel_start )
            y_pixel_start = y_pixel_start2;
        if( y_pixel_end2 < y_pixel_end )
            y_pixel_end = y_pixel_end2;
    }

    x_size = x_pixel_end - x_pixel_start + 1;
    y_size = y_pixel_end - y_pixel_start + 1;

    --x_size;    /* because last pixel may be on boundary of volume */
    --y_size;

    modify_pixels_size( n_pixels_alloced, pixels, x_size, y_size, pixel_type );

    if( x_size > 0 && y_size > 0 )
    {
        convert_pixel_start_to_voxel( x_pixel_start, y_pixel_start,
                                      x_offset1, y_offset1,
                                      x_delta1, y_delta1,
                                      &x_start1, &y_start1 );

        if( volume2 != (Volume) NULL )
        {
            convert_pixel_start_to_voxel( x_pixel_start, y_pixel_start,
                                          x_offset2, y_offset2,
                                          x_delta2, y_delta2,
                                          &x_start2, &y_start2 );
        }

        pixels->x_position = x_pixel_start;
        pixels->y_position = y_pixel_start;

        ALLOC( slice_start1, n_slices );

        for_less( s, 0, n_slices )
        {
            indices[X] = ROUND( slice_positions1[s] );
            indices[Y] = ROUND( slice_positions1[s] );
            indices[Z] = ROUND( slice_positions1[s] );
            indices[x_axis_index] = 0;
            indices[y_axis_index] = 0;

            for_less( axis, 0, N_DIMENSIONS )
            {
                if( indices[axis] == volume1->sizes[axis] )
                    indices[axis] = volume1->sizes[axis] - 1;
            }

            GET_VOXEL_PTR_3D( slice_start1[s], volume1,
                              indices[X], indices[Y], indices[Z] );
        }

        strides1[X] = volume1->sizes[Y] * volume1->sizes[Z];
        strides1[Y] = volume1->sizes[Z];
        strides1[Z] = 1;

        if( volume2 != (Volume) NULL )
        {
            volume2_data_type = volume2->data_type;

            ALLOC( slice_start2, n_slices );

            for_less( s, 0, n_slices )
            {
                indices[X] = ROUND( slice_positions2[s] );
                indices[Y] = ROUND( slice_positions2[s] );
                indices[Z] = ROUND( slice_positions2[s] );
                indices[x_axis_index] = 0;
                indices[y_axis_index] = 0;

                for_less( axis, 0, N_DIMENSIONS )
                {
                    if( indices[axis] == volume2->sizes[axis] )
                        indices[axis] = volume2->sizes[axis] - 1;
                }

                GET_VOXEL_PTR_3D( slice_start2[s], volume2,
                                  indices[X], indices[Y], indices[Z] );
            }

            strides2[X] = volume2->sizes[Y] * volume2->sizes[Z];
            strides2[Y] = volume2->sizes[Z];
            strides2[Z] = 1;
        }
        else
            slice_start2 = (void **) NULL;

        render_volume_to_slice( n_slices,
                                slice_start1, volume1->data_type, weights1,
                                strides1[x_axis_index], strides1[y_axis_index],
                                x_start1, y_start1, x_delta1, y_delta1,
                                slice_start2, volume2_data_type, weights2,
                                strides2[x_axis_index], strides2[y_axis_index],
                                x_start2, y_start2, x_delta2, y_delta2,
                                interpolation_flag, cmode_colour_map,
                                rgb_colour_map, pixels );

        FREE( slice_start1 );

        if( volume2 != (Volume) NULL )
            FREE( slice_start2 );
    }
}

public  void  create_volume_slice(
    Filter_types    filter_type,
    Real            filter_width,
    Volume          volume1,
    Real            slice_position1,
    Real            x_translation1,
    Real            y_translation1,
    Real            x_scale1,
    Real            y_scale1,
    Volume          volume2,
    Real            slice_position2,
    Real            x_translation2,
    Real            y_translation2,
    Real            x_scale2,
    Real            y_scale2,
    int             x_axis_index,
    int             y_axis_index,
    int             axis_index,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    Boolean         interpolation_flag,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    int          i, n_slices;
    Real         *positions1, *positions2, *weights1, *weights2;

    n_slices = get_slice_weights_for_filter( volume1, axis_index,
                                             slice_position1,
                                             filter_type, filter_width,
                                             &positions1, &weights1 );

#ifdef DEBUG
#define DEBUG
if( n_slices > 1 )
{
    (void) printf( "%3d slices: ", n_slices );
    for_less( i, 0, n_slices )
        (void) printf( "%10g", positions1[i] );
    (void) printf( "\n" );

    (void) printf( "          : " );
    for_less( i, 0, n_slices )
        (void) printf( "%10g", weights1[i] );
    (void) printf( "\n" );
}
#endif

    if( volume2 != (Volume) NULL )
    {
        ALLOC( positions2, n_slices );
        ALLOC( weights2, n_slices );

        for_less( i, 0, n_slices )
        {
            weights2[i] = weights1[i];
            positions2[i] = slice_position2 + positions1[i] - slice_position1;
        }
    }

    create_weighted_volume_slices( n_slices,
                                   volume1, positions1, weights1,
                                   x_translation1, y_translation1,
                                   x_scale1, y_scale1,
                                   volume2, positions2, weights2,
                                   x_translation2, y_translation2,
                                   x_scale2, y_scale2,
                                   x_axis_index, y_axis_index,
                                   x_viewport_size, y_viewport_size,
                                   pixel_type, interpolation_flag,
                                   cmode_colour_map, rgb_colour_map,
                                   n_pixels_alloced, pixels );

    FREE( positions1 );
    FREE( weights1 );

    if( volume2 != (Volume) NULL )
    {
        FREE( positions2 );
        FREE( weights2 );
    }
}

public  Boolean  convert_slice_pixel_to_voxel(
    Volume          volume,
    Real            x_pixel,
    Real            y_pixel,
    Real            slice_position[N_DIMENSIONS],
    int             x_axis_index,
    int             y_axis_index,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            pixel_slice_position[N_DIMENSIONS] )
{
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index,
                 y_translation, y_scale, y_axis_index,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    pixel_slice_position[X] = slice_position[X];
    pixel_slice_position[Y] = slice_position[Y];
    pixel_slice_position[Z] = slice_position[Z];

    pixel_slice_position[x_axis_index] =
             map_viewport_to_slice_1d( x_pixel, x_offset, x_delta );
    pixel_slice_position[y_axis_index] =
             map_viewport_to_slice_1d( y_pixel, y_offset, y_delta );

    return( voxel_is_within_volume( volume, pixel_slice_position ) );
}

public  void  convert_voxel_to_slice_pixel(
    Volume          volume,
    Real            voxel_position[N_DIMENSIONS],
    int             x_axis_index,
    int             y_axis_index,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            *x_pixel,
    Real            *y_pixel )
{
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index,
                 y_translation, y_scale, y_axis_index,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    *x_pixel = map_slice_to_viewport_1d( voxel_position[x_axis_index],
                                         x_offset, x_delta );
    *y_pixel = map_slice_to_viewport_1d( voxel_position[y_axis_index],
                                         y_offset, y_delta );
}
