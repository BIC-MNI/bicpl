#undef  TWO
#ifdef TWO_VOLUMES
#define  TWO( x )  x
#else
#define  TWO( x )
#endif

    /* TWO    if needed
       TYPE1, TYPE2 */
{
    int            s, x, y;
    TYPE1          **start_data1;
    int            voxel_data1, **row_offsets1;
#ifdef TWO_VOLUMES
    TYPE2          **start_data2;
    int            voxel_data2, **row_offsets2;
#endif
#ifdef ONE_SLICE
    TYPE1          *ptr1;
    int            *offset_ptr1;
    TWO( TYPE2     *ptr2; )
    TWO( int       *offset_ptr2; )
#else
    Real           real_voxel_data1;
    TWO( Real      real_voxel_data2; )
#endif

#ifndef  TWO_VOLUMES
#ifdef  COLOUR_MAP
    unsigned short     *single_cmode_map = cmode_colour_map[0];
#else
    Colour             *single_rgb_map = rgb_colour_map[0];
#endif
#endif

#ifdef  COLOUR_MAP
    unsigned short  *pixel_ptr, colour;

    pixel_ptr = pixels->data.pixels_16bit_colour_index;
#else
    Colour          *pixel_ptr, colour;

    pixel_ptr = pixels->data.pixels_rgb;
#endif

    ALLOC( start_data1, n_slices1 );
    ALLOC( row_offsets1, n_slices1 );
    TWO( ALLOC( start_data2, n_slices2 ); )
    TWO( ALLOC( row_offsets2, n_slices2 ); )

    for_less( y, 0, y_size )
    {
        for_less( s, 0, n_slices1 )
        {
            start_data1[s] = ((TYPE1 *) volume_data1) + y_offsets1[s][y];
            row_offsets1[s] = which_x_offsets1[s][y];
        }
#ifdef  TWO_VOLUMES
        for_less( s, 0, n_slices2 )
        {
            start_data2[s] = ((TYPE2 *) volume_data2) + y_offsets2[s][y];
            row_offsets2[s] = which_x_offsets2[s][y];
        }
#endif

        for_less( x, 0, start_x[y] )
        {
            *pixel_ptr = empty_colour;
            ++pixel_ptr;
        }

#ifdef ONE_SLICE
        ptr1 = start_data1[0];
        offset_ptr1 = &row_offsets1[0][start_x[y]];
        TWO( ptr2 = start_data2[0]; )
        TWO( offset_ptr2 = &row_offsets2[0][start_x[y]]; )
#endif

        for_inclusive( x, start_x[y], end_x[y] )
        {
#ifdef ONE_SLICE
            voxel_data1 = ptr1[*offset_ptr1];
            ++offset_ptr1;
            TWO( voxel_data2 = ptr2[*offset_ptr2]; )
            TWO( ++offset_ptr2; )
#else
            real_voxel_data1 = 0.5;
            for_less( s, 0, n_slices1 )
            {
                real_voxel_data1 += weights1[s] *
                                    start_data1[s][row_offsets1[s][x]];
            }

            voxel_data1 = (int) real_voxel_data1;
#ifdef  TWO_VOLUMES
            real_voxel_data2 = 0.5;
            for_less( s, 0, n_slices2 )
            {
                real_voxel_data2 += weights2[s] *
                                    start_data2[s][row_offsets2[s][x]];
            }

            voxel_data2 = (int) real_voxel_data2;
#endif

#endif

#ifdef  TWO_VOLUMES
#ifdef COLOUR_MAP
            colour = cmode_colour_map[voxel_data1][voxel_data2];
#else
            colour = rgb_colour_map[voxel_data1][voxel_data2];
#endif
#else
#ifdef COLOUR_MAP
            colour = single_cmode_map[voxel_data1];
#else
            colour = single_rgb_map[voxel_data1];
#endif
#endif
            *pixel_ptr = colour;
            ++pixel_ptr;
        }

        for_less( x, end_x[y]+1, x_size )
        {
            *pixel_ptr = empty_colour;
            ++pixel_ptr;
        }
    }

    FREE( start_data1 );
    FREE( row_offsets1 );
    TWO( FREE( start_data1 ); )
    TWO( FREE( row_offsets1 ); )
}
