
#include  <def_mni.h>

private  void  get_mapping(
    volume_struct   *volume,
    Real            x_translation,
    Real            x_scale,
    int             x_axis_index,
    Boolean         x_axis_flip_flag,
    Real            y_translation,
    Real            y_scale,
    int             y_axis_index,
    Boolean         y_axis_flip_flag,
    Real            *x_start,
    Real            *x_end,
    Real            *x_delta,
    Real            *x_offset,
    Real            *y_start,
    Real            *y_end,
    Real            *y_delta,
    Real            *y_offset )
{
    *x_delta = x_scale * volume->thickness[x_axis_index];
    *x_start = -0.5;
    *x_end = (Real) volume->sizes[x_axis_index] - 0.5;
    *x_offset = x_translation - 0.5 + (*x_delta) * 0.5;

    if( x_axis_flip_flag )
    {
        *x_delta = -*x_delta;
        *x_start = *x_end;
        *x_end = -0.5;
        *x_offset = x_translation + 0.5 -
                    (*x_delta) * (Real) (volume->sizes[x_axis_index]-1);
    }

    *y_delta = y_scale * volume->thickness[y_axis_index];
    *y_start = -0.5;
    *y_end = (Real) volume->sizes[y_axis_index] - 0.5;
    *y_offset = y_translation - 0.5 + (*y_delta) * 0.5;

    if( y_axis_flip_flag )
    {
        *y_delta = -*y_delta;
        *y_start = *y_end;
        *y_end = -0.5;
        *y_offset = y_translation + 0.5 -
                    (*y_delta) * (Real) (volume->sizes[y_axis_index]-1);
    }
}

public  void  create_volume_slice(
    volume_struct   *volume,
    Real            slice_position,
    int             x_axis_index,
    Boolean         x_axis_flip_flag,
    int             y_axis_index,
    Boolean         y_axis_flip_flag,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    int             x_viewport_size,
    int             y_viewport_size,
    Pixel_types     pixel_type,
    Boolean         interpolation_flag,
    Colour          rgb_colour_map[],
    int             colour_index_offset,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    Status       status;
    Volume_type  *slice_start;
    Boolean      within_viewport;
    int          axis, strides[3], indices[3], x_size, y_size;
    int          x_position, y_position;
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index, x_axis_flip_flag,
                 y_translation, y_scale, y_axis_index, y_axis_flip_flag,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    within_viewport = clip_slice_to_viewport( x_viewport_size, y_viewport_size,
                                              x_offset, y_offset,
                                              x_delta, y_delta,
                                              x_start, y_start,
                                              x_end, y_end,
                                              &x_start, &y_start,
                                              &x_size, &y_size,
                                              &x_position, &y_position );

    status = modify_pixels_size( n_pixels_alloced, pixels, x_size, y_size,
                                 pixel_type );
    pixels->x_position = x_position;
    pixels->y_position = y_position;

    if( status == OK && within_viewport )
    {
        indices[X] = ROUND( slice_position );
        indices[Y] = ROUND( slice_position );
        indices[Z] = ROUND( slice_position );
        indices[x_axis_index] = 0;
        indices[y_axis_index] = 0;

        for_less( axis, 0, N_DIMENSIONS )
        {
            if( indices[axis] == volume->sizes[axis] )
                indices[axis] = volume->sizes[axis] - 1;
        }

        slice_start = &volume->data[indices[X]][indices[Y]][indices[Z]];

        strides[X] = volume->sizes[Y] * volume->sizes[Z];
        strides[Y] = volume->sizes[Z];
        strides[Z] = 1;

        render_volume_to_slice( slice_start,
                                strides[x_axis_index], strides[y_axis_index],
                                x_start, y_start, x_delta, y_delta,
                                interpolation_flag, rgb_colour_map,
                                colour_index_offset, pixels );
    }
}

public  Boolean  convert_slice_pixel_to_voxel(
    volume_struct   *volume,
    int             x_pixel,
    int             y_pixel,
    Real            slice_position[N_DIMENSIONS],
    int             x_axis_index,
    Boolean         x_axis_flip_flag,
    int             y_axis_index,
    Boolean         y_axis_flip_flag,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            pixel_slice_position[N_DIMENSIONS] )
{
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index, x_axis_flip_flag,
                 y_translation, y_scale, y_axis_index, y_axis_flip_flag,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    pixel_slice_position[X] = slice_position[X];
    pixel_slice_position[Y] = slice_position[Y];
    pixel_slice_position[Z] = slice_position[Z];

    pixel_slice_position[x_axis_index] =
             map_viewport_to_slice_1d( (Real) x_pixel, x_offset, x_delta );
    pixel_slice_position[y_axis_index] =
             map_viewport_to_slice_1d( (Real) y_pixel, y_offset, y_delta );

    return( voxel_is_within_volume( volume, pixel_slice_position ) );
}

public  Boolean  voxel_is_within_volume(
    volume_struct   *volume,
    Real            position[N_DIMENSIONS] )
{
    return( position[X] >= -0.5 &&
            position[X] < (Real) volume->sizes[X] - 0.5 &&
            position[Y] >= -0.5 &&
            position[Y] < (Real) volume->sizes[Y] - 0.5 &&
            position[Z] >= -0.5 &&
            position[Z] < (Real) volume->sizes[Z] - 0.5 );
}
