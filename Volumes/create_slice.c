
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
    if( volume->flip_axis[x_axis_index] )
        x_axis_flip_flag = !x_axis_flip_flag;

    if( volume->flip_axis[y_axis_index] )
        y_axis_flip_flag = !y_axis_flip_flag;

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
    volume_struct   *volume1,
    volume_struct   *volume2,
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
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    int             *n_pixels_alloced,
    pixels_struct   *pixels )
{
    Volume_type  *slice_start1, *slice_start2;
    Boolean      within_viewport;
    int          strides1[N_DIMENSIONS], strides2[N_DIMENSIONS];
    int          indices1[N_DIMENSIONS], indices2[N_DIMENSIONS];
    int          axis, x_size, y_size;
    int          x_position, y_position;
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;
    Real         trans[N_DIMENSIONS], scale[N_DIMENSIONS];

    get_mapping( volume1,
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

    modify_pixels_size( n_pixels_alloced, pixels, x_size, y_size, pixel_type );
    pixels->x_position = x_position;
    pixels->y_position = y_position;

    if( within_viewport )
    {
        indices1[X] = ROUND( slice_position );
        indices1[Y] = ROUND( slice_position );
        indices1[Z] = ROUND( slice_position );
        indices1[x_axis_index] = 0;
        indices1[y_axis_index] = 0;

        for_less( axis, 0, N_DIMENSIONS )
        {
            if( indices1[axis] == volume1->sizes[axis] )
                indices1[axis] = volume1->sizes[axis] - 1;
        }

        slice_start1 = &volume1->data[indices1[X]][indices1[Y]][indices1[Z]];

        strides1[X] = volume1->sizes[Y] * volume1->sizes[Z];
        strides1[Y] = volume1->sizes[Z];
        strides1[Z] = 1;

        if( volume2 != (volume_struct *) NULL )
        {
            scale[X] = (Real) volume2->sizes[X] / (Real) volume1->sizes[X];
            scale[Y] = (Real) volume2->sizes[Y] / (Real) volume1->sizes[Y];
            scale[Z] = (Real) volume2->sizes[Z] / (Real) volume1->sizes[Z];
            trans[X] = scale[X] * 0.5 - 0.5;
            trans[Y] = scale[Y] * 0.5 - 0.5;
            trans[Z] = scale[Z] * 0.5 - 0.5;
            indices2[X] = ROUND( slice_position * scale[X] + trans[X] - 0.5 );
            indices2[Y] = ROUND( slice_position * scale[Y] + trans[Y] - 0.5 );
            indices2[Z] = ROUND( slice_position * scale[Z] + trans[Z] - 0.5 );
            indices2[x_axis_index] = 0;
            indices2[y_axis_index] = 0;

            for_less( axis, 0, N_DIMENSIONS )
            {
                if( indices2[axis] == volume2->sizes[axis] )
                    indices2[axis] = volume2->sizes[axis] - 1;
            }

            slice_start2 = &volume2->data[indices2[X]][indices2[Y]]
                                         [indices2[Z]];

            strides2[X] = volume2->sizes[Y] * volume2->sizes[Z];
            strides2[Y] = volume2->sizes[Z];
            strides2[Z] = 1;
        }
        else
            slice_start2 = (Volume_type *) NULL;

        render_volume_to_slice( slice_start1,
                                strides1[x_axis_index], strides1[y_axis_index],
                                x_start, y_start, x_delta, y_delta,
                                slice_start2,
                                strides2[x_axis_index], strides2[y_axis_index],
                                trans[x_axis_index], trans[y_axis_index],
                                scale[x_axis_index], scale[y_axis_index],
                                interpolation_flag, cmode_colour_map,
                                rgb_colour_map, pixels );
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

public  void  convert_voxel_to_slice_pixel(
    volume_struct   *volume,
    Real            voxel_position[N_DIMENSIONS],
    int             x_axis_index,
    Boolean         x_axis_flip_flag,
    int             y_axis_index,
    Boolean         y_axis_flip_flag,
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    int             *x_pixel,
    int             *y_pixel )
{
    Real         x_delta, x_start, x_end, y_delta, y_start, y_end;
    Real         x_offset, y_offset;

    get_mapping( volume,
                 x_translation, x_scale, x_axis_index, x_axis_flip_flag,
                 y_translation, y_scale, y_axis_index, y_axis_flip_flag,
                 &x_start, &x_end, &x_delta, &x_offset,
                 &y_start, &y_end, &y_delta, &y_offset );

    *x_pixel = ROUND( map_slice_to_viewport_1d( voxel_position[x_axis_index],
                                                x_offset, x_delta ) );
    *y_pixel = ROUND( map_slice_to_viewport_1d( voxel_position[y_axis_index],
                                                y_offset, y_delta ) );
}
