#include  <def_mni.h>

private  void  render_2_volumes_rgb_flat(
    int           x_size,
    int           y_size,
    Volume_type   *volume_data,
    int           x_offsets[],
    int           y_offsets[],
    Volume_type   *volume_data2,
    int           x_offsets2[],
    int           y_offsets2[],
    Colour        **rgb_colour_map,
    pixels_struct *pixels );

private  void  render_2_volumes_colour_map_flat(
    int            x_size,
    int            y_size,
    Volume_type    *volume_data,
    int            x_offsets[],
    int            y_offsets[],
    Volume_type    *volume_data2,
    int            x_offsets2[],
    int            y_offsets2[],
    unsigned short **cmode_colour_map,
    pixels_struct  *pixels );

private  void  render_rgb_flat(
    int           x_size,
    int           y_size,
    Volume_type   *volume_data,
    int           x_offsets[],
    int           y_offsets[],
    Colour        rgb_colour_map[],
    pixels_struct *pixels );

private  void  render_colour_map_flat(
    int           x_size,
    int           y_size,
    Volume_type   *volume_data,
    int           x_offsets[],
    int           y_offsets[],
    unsigned short cmode_colour_map[],
    pixels_struct *pixels );

public  void  render_volume_to_slice(
    Volume_type     *volume_data,
    int             x_stride,
    int             y_stride,
    Real            x_start,
    Real            y_start,
    Real            x_delta,
    Real            y_delta,
    Volume_type     *second_volume_data,
    int             x_stride2,
    int             y_stride2,
    Real            x_trans_to_second,
    Real            y_trans_to_second,
    Real            x_scale_to_second,
    Real            y_scale_to_second,
    Boolean         interpolation_flag,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
    int                  x_size, y_size, x, y;
    Real                 x_voxel, y_voxel;
    Boolean              colour_map_mode;
    static  int          *x_offsets;
    static  int          *y_offsets;
    static  int          x_size_alloced = 0;
    static  int          y_size_alloced = 0;
    static  int          *second_x_offsets;
    static  int          *second_y_offsets;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    if( x_size > x_size_alloced )
    {
        SET_ARRAY_SIZE( x_offsets, x_size_alloced, x_size, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( second_x_offsets, x_size_alloced, x_size,
                        DEFAULT_CHUNK_SIZE );
        x_size_alloced = x_size;
    }

    if( y_size > y_size_alloced )
    {
        SET_ARRAY_SIZE( y_offsets, y_size_alloced, y_size, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( second_y_offsets, y_size_alloced, y_size,
                        DEFAULT_CHUNK_SIZE );
        y_size_alloced = y_size;
    }

    colour_map_mode = (pixels->pixel_type != RGB_PIXEL);

    if( interpolation_flag )
    {
    }
    else
    {
        for_less( x, 0, x_size )
        {
            x_voxel = x_start + ((Real) x + 0.5) / x_delta;
            x_offsets[x] = x_stride * ROUND( x_voxel );

            if( second_volume_data != (Volume_type *) NULL )
            {
                x_voxel = x_scale_to_second * x_voxel + x_trans_to_second;
                second_x_offsets[x] = x_stride2 * ROUND( x_voxel );
            }
        }

        for_less( y, 0, y_size )
        {
            y_voxel = y_start + ((Real) y + 0.5) / y_delta;
            y_offsets[y] = y_stride * ROUND( y_voxel );

            if( second_volume_data != (Volume_type *) NULL )
            {
                y_voxel = y_scale_to_second * y_voxel + y_trans_to_second;
                second_y_offsets[y] = y_stride2 * ROUND( y_voxel );
            }
        }
    }

    if( second_volume_data != (Volume_type *) NULL )
    {
        if( colour_map_mode )
        {
            if( interpolation_flag )
            {
            }
            else
            {
                render_2_volumes_colour_map_flat( x_size, y_size, volume_data,
                                        x_offsets, y_offsets,
                                        second_volume_data,
                                        second_x_offsets, second_y_offsets,
                                        cmode_colour_map, pixels );
            }
        }
        else
        {
            if( interpolation_flag )
            {
            }
            else
            {
                render_2_volumes_rgb_flat( x_size, y_size, volume_data,
                                 x_offsets, y_offsets, second_volume_data,
                                 second_x_offsets, second_y_offsets,
                                 rgb_colour_map, pixels );
            }
        }
    }
    else
    {
        if( colour_map_mode )
        {
            if( interpolation_flag )
            {
            }
            else
            {
                render_colour_map_flat( x_size, y_size, volume_data,
                                        x_offsets, y_offsets, *cmode_colour_map,
                                        pixels );
            }
        }
        else
        {
            if( interpolation_flag )
            {
            }
            else
            {
                render_rgb_flat( x_size, y_size, volume_data,
                                 x_offsets, y_offsets,
                                 *rgb_colour_map, pixels );
            }
        }
    }
}

private  void  render_colour_map_interpolated(
    Volume_type   *volume_data,
    int           x_stride,
    int           y_stride,
    Real          thickness[],
    unsigned short  cmode_colour_map[],
    pixels_struct *pixels )    /* ARGSUSED */
{
}

private  void  render_rgb_interpolated(
    Volume_type   *volume_data,
    int           x_stride,
    int           y_stride,
    Real          thickness[],
    Colour        rgb_colour_map[],
    pixels_struct *pixels )   /* ARGSUSED */
{
}

private  void  render_rgb_flat(
    int           x_size,
    int           y_size,
    Volume_type   *volume_data,
    int           x_offsets[],
    int           y_offsets[],
    Colour        rgb_colour_map[],
    pixels_struct *pixels )
{
    int           x, y;
    int           prev_y_offset, prev_x_offset;
    int           y_offset, x_offset;
    Colour        *pixel_ptr, colour;
    Volume_type   *voxel_ptr;

    pixel_ptr = pixels->data.pixels_rgb;

    prev_y_offset = y_offsets[0] + 1;

    for_less( y, 0, y_size )
    {
        y_offset = y_offsets[y];
        if( y_offset == prev_y_offset )
        {
            for_less( x, 0, x_size )
            {
                *pixel_ptr = pixel_ptr[-x_size];
                ++pixel_ptr;
            }
        }
        else
        {
            prev_y_offset = y_offset;
            voxel_ptr = &volume_data[y_offset];

            prev_x_offset = x_offsets[0] + 1;
            for_less( x, 0, x_size )
            {
                x_offset = x_offsets[x];
                if( x_offset != prev_x_offset )
                {
                    prev_x_offset = x_offset;
                    colour = rgb_colour_map[voxel_ptr[x_offset]];
                }
                *pixel_ptr = colour;
                ++pixel_ptr;
            }
        }
    }
}

private  void  render_colour_map_flat(
    int             x_size,
    int             y_size,
    Volume_type     *volume_data,
    int             x_offsets[],
    int             y_offsets[],
    unsigned short  cmode_colour_map[],
    pixels_struct   *pixels )
{
    int              x, y;
    int              prev_y_offset, prev_x_offset;
    int              y_offset, x_offset;
    unsigned short   voxel_data, *pixel_ptr;
    Volume_type      *voxel_ptr;

    pixel_ptr = pixels->data.pixels_16bit_colour_index;

    prev_y_offset = y_offsets[0] + 1;

    for_less( y, 0, y_size )
    {
        y_offset = y_offsets[y];
        if( y_offset == prev_y_offset )
        {
            for_less( x, 0, x_size )
            {
                *pixel_ptr = pixel_ptr[-x_size];
                ++pixel_ptr;
            }
        }
        else
        {
            prev_y_offset = y_offset;
            voxel_ptr = &volume_data[y_offset];

            prev_x_offset = x_offsets[0] + 1;
            for_less( x, 0, x_size )
            {
                x_offset = x_offsets[x];
                if( x_offset != prev_x_offset )
                {
                    prev_x_offset = x_offset;
                    voxel_data = cmode_colour_map[voxel_ptr[x_offset]];
                }
                *pixel_ptr = voxel_data;
                ++pixel_ptr;
            }
        }
    }
}

private  void  render_2_volumes_rgb_flat(
    int           x_size,
    int           y_size,
    Volume_type   *volume_data1,
    int           x_offsets1[],
    int           y_offsets1[],
    Volume_type   *volume_data2,
    int           x_offsets2[],
    int           y_offsets2[],
    Colour        **rgb_colour_map,
    pixels_struct *pixels )
{
    int           x, y;
    int           prev_y_offset1, prev_x_offset1;
    int           prev_y_offset2, prev_x_offset2;
    int           y_offset1, x_offset1;
    int           y_offset2, x_offset2;
    Colour        *pixel_ptr, colour;
    Volume_type   *voxel_ptr1, *voxel_ptr2, voxel_data1, voxel_data2;

    pixel_ptr = pixels->data.pixels_rgb;

    prev_y_offset1 = y_offsets1[0] + 1;
    prev_y_offset2 = y_offsets2[0] + 1;

    for_less( y, 0, y_size )
    {
        y_offset1 = y_offsets1[y];
        y_offset2 = y_offsets2[y];
        if( y_offset1 == prev_y_offset1 && y_offset2 == prev_y_offset2 )
        {
            for_less( x, 0, x_size )
            {
                *pixel_ptr = pixel_ptr[-x_size];
                ++pixel_ptr;
            }
        }
        else
        {
            prev_y_offset1 = y_offset1;
            prev_y_offset2 = y_offset2;

            voxel_ptr1 = &volume_data1[y_offset1];
            voxel_ptr2 = &volume_data2[y_offset2];

            prev_x_offset1 = x_offsets1[0] + 1;
            prev_x_offset2 = x_offsets2[0] + 1;

            for_less( x, 0, x_size )
            {
                x_offset1 = x_offsets1[x];
                x_offset2 = x_offsets2[x];
                if( x_offset1 != prev_x_offset1 )
                {
                    prev_x_offset1 = x_offset1;
                    voxel_data1 = voxel_ptr1[x_offset1];

                    if( x_offset2 != prev_x_offset2 )
                    {
                        prev_x_offset2 = x_offset2;
                        voxel_data2 = voxel_ptr2[x_offset2];
                    }

                    colour = rgb_colour_map[voxel_data1][voxel_data2];
                }
                else if( x_offset2 != prev_x_offset2 )
                {
                    prev_x_offset2 = x_offset2;
                    voxel_data2 = voxel_ptr2[x_offset2];

                    colour = rgb_colour_map[voxel_data1][voxel_data2];
                }

                *pixel_ptr = colour;
                ++pixel_ptr;
            }
        }
    }
}

private  void  render_2_volumes_colour_map_flat(
    int             x_size,
    int             y_size,
    Volume_type     *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    Volume_type     *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    unsigned short  **cmode_colour_map,
    pixels_struct   *pixels )
{
    int             x, y;
    int             prev_y_offset1, prev_x_offset1;
    int             prev_y_offset2, prev_x_offset2;
    int             y_offset1, x_offset1;
    int             y_offset2, x_offset2;
    unsigned short  ind, *pixel_ptr;
    Volume_type     *voxel_ptr1, *voxel_ptr2, voxel_data1, voxel_data2;

    pixel_ptr = pixels->data.pixels_16bit_colour_index;

    prev_y_offset1 = y_offsets1[0] + 1;
    prev_y_offset2 = y_offsets2[0] + 1;

    for_less( y, 0, y_size )
    {
        y_offset1 = y_offsets1[y];
        y_offset2 = y_offsets2[y];
        if( y_offset1 == prev_y_offset1 && y_offset2 == prev_y_offset2 )
        {
            for_less( x, 0, x_size )
            {
                *pixel_ptr = pixel_ptr[-x_size];
                ++pixel_ptr;
            }
        }
        else
        {
            prev_y_offset1 = y_offset1;
            prev_y_offset2 = y_offset2;

            voxel_ptr1 = &volume_data1[y_offset1];
            voxel_ptr2 = &volume_data2[y_offset2];

            prev_x_offset1 = x_offsets1[0] + 1;
            prev_x_offset2 = x_offsets2[0] + 1;

            for_less( x, 0, x_size )
            {
                x_offset1 = x_offsets1[x];
                x_offset2 = x_offsets2[x];
                if( x_offset1 != prev_x_offset1 )
                {
                    prev_x_offset1 = x_offset1;
                    voxel_data1 = voxel_ptr1[x_offset1];

                    if( x_offset2 != prev_x_offset2 )
                    {
                        prev_x_offset2 = x_offset2;
                        voxel_data2 = voxel_ptr2[x_offset2];
                    }

                    ind = cmode_colour_map[voxel_data1][voxel_data2];
                }
                else if( x_offset2 != prev_x_offset2 )
                {
                    prev_x_offset2 = x_offset2;
                    voxel_data2 = voxel_ptr2[x_offset2];

                    ind = cmode_colour_map[voxel_data1][voxel_data2];
                }

                *pixel_ptr = ind;
                ++pixel_ptr;
            }
        }
    }
}
