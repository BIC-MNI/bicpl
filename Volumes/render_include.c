#undef  TWO
#ifdef TWO_VOLUMES
#define  TWO( x )  x
#else
#define  TWO( x )
#endif

/* assumes that we have the following variables:
    x_size
    y_size
    volume_data1
    x_offsets1
    y_offsets1
    [volume_data2
     x_offsets2
     y_offsets2]
    cmmode_colour_map or rgb_colour_map
    pixels */
{
    int           x, y, y_offset1, x_offset1, prev_y_offset1, prev_x_offset1;

#ifdef BYTE_DATA
    unsigned char   *voxel_ptr1, voxel_data1;
#else
    unsigned short  *voxel_ptr1, voxel_data1;
#endif

#ifdef TWO_VOLUMES
    int             prev_y_offset2, prev_x_offset2;
    int             y_offset2, x_offset2;
#ifdef BYTE_DATA2
    unsigned char   *voxel_ptr2, voxel_data2;
#else
    unsigned short  *voxel_ptr2, voxel_data2;
#endif
#endif

#ifdef  COLOUR_MAP
    unsigned short  *pixel_ptr, colour;

    pixel_ptr = pixels->data.pixels_16bit_colour_index;
#else
    Colour          *pixel_ptr, colour;

    pixel_ptr = pixels->data.pixels_rgb;
#endif

    prev_y_offset1 = y_offsets1[0] + 1;
    TWO( prev_y_offset2 = y_offsets2[0] + 1 );

    for_less( y, 0, y_size )
    {
        y_offset1 = y_offsets1[y];
        TWO( y_offset2 = y_offsets2[y] );

        if( y_offset1 == prev_y_offset1 TWO(&& y_offset2 == prev_y_offset2) )
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
            TWO( prev_y_offset2 = y_offset2 );

            voxel_ptr1 = &volume_data1[y_offset1];
            TWO( voxel_ptr2 = &volume_data2[y_offset2] );

            prev_x_offset1 = x_offsets1[0] + 1;
            TWO( prev_x_offset2 = x_offsets2[0] + 1 );

            for_less( x, 0, x_size )
            {
                x_offset1 = x_offsets1[x];
                TWO( x_offset2 = x_offsets2[x] );
                if( x_offset1 != prev_x_offset1 )
                {
                    prev_x_offset1 = x_offset1;
                    voxel_data1 = voxel_ptr1[x_offset1];

#ifdef TWO_VOLUMES
                    if( x_offset2 != prev_x_offset2 )
                    {
                        prev_x_offset2 = x_offset2;
                        voxel_data2 = voxel_ptr2[x_offset2];
                    }

#ifdef COLOUR_MAP
                    colour = cmode_colour_map[voxel_data1][voxel_data2];
#else
                    colour = rgb_colour_map[voxel_data1][voxel_data2];
#endif
#else
#ifdef COLOUR_MAP
                    colour = cmode_colour_map[voxel_data1];
#else
                    colour = rgb_colour_map[voxel_data1];
#endif
#endif
                }
#ifdef TWO_VOLUMES
                else if( x_offset2 != prev_x_offset2 )
                {
                    prev_x_offset2 = x_offset2;
                    voxel_data2 = voxel_ptr2[x_offset2];

#ifdef COLOUR_MAP
                    colour = cmode_colour_map[voxel_data1][voxel_data2];
#else
                    colour = rgb_colour_map[voxel_data1][voxel_data2];
#endif
                }
#endif

                *pixel_ptr = colour;
                ++pixel_ptr;
            }
        }
    }
}
