#include  <mni.h>

private  void  clip(
    Real  left_edge,
    Real  right_edge,
    Real  origin,
    Real  delta,
    Real  *t_min,
    Real  *t_max )
{
    Real  t;

    if( delta == 0.0 )
    {
        if( origin < left_edge || origin >= right_edge )
        {
            *t_min = 1.0;
            *t_max = 0.0;
        }
    }
    else if( delta > 0.0 )
    {
        t = (left_edge - origin) / delta;
        if( t > *t_min )
            *t_min = t;

        t = (right_edge - origin) / delta;
        if( t < *t_max )
            *t_max = t;
    }
    else
    {
        t = (left_edge - origin) / delta;
        if( t < *t_max )
            *t_max = t;

        t = (right_edge - origin) / delta;
        if( t > *t_min )
            *t_min = t;
    }
}

public  void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    Data_types      volume1_type,
    int             n_slices1,
    Real            weights1[],
    int             strides1[],
    Real            **origins1,
    Real            x_axis1[],
    Real            y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    Data_types      volume2_type,
    int             n_slices2,
    Real            weights2[],
    int             strides2[],
    Real            **origins2,
    Real            x_axis2[],
    Real            y_axis2[],
    BOOLEAN         interpolation_flag,    /* ARGSUSED */
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    pixels_struct   *pixels )
{
    int   i, c, p, total_cases, case_index, case_multiplier, offset, int_start;
    int   s, x, y, n_cases1[MAX_DIMENSIONS], n_cases2[MAX_DIMENSIONS];
    int   **x_offsets1, **y_offsets1, *start_x, *end_x;
    int   **x_offsets2, **y_offsets2;
    int   ***which_x_offsets1, ***which_x_offsets2;
    int   remainder_case, x_size, y_size;
    Real  start_c, x_start, x_end, remainder, tmp_origin[MAX_DIMENSIONS];
    Real  remainder_offset, left_edge, right_edge, delta;
    static  int   max_cases[MAX_DIMENSIONS] = { 10, 10, 4, 3, 3 };

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    total_cases = 1;
    for_less( c, 0, n_dims1 )
    {
        delta = ABS( x_axis1[c] );
        if( delta == 0.0 )
            n_cases1[c] = 1;
        else if( delta <= 1.0 / (Real) max_cases[n_dims1-1] )
            n_cases1[c] = max_cases[n_dims1-1];
        else
        {
            n_cases1[c] = (int) (1.0 / delta) + 1;
            if( n_cases1[c] > max_cases[n_dims1-1] )
                n_cases1[c] = max_cases[n_dims1-1];
        }

        total_cases *= n_cases1[c];
    }

    ALLOC2D( x_offsets1, total_cases, x_size );

    for_less( i, 0, total_cases )
    {
        p = i;
        for_less( c, 0, n_dims1 )
        {
            case_index = p % n_cases1[c];
            if( x_axis1[c] == 0.0 )
                tmp_origin[c] = 0.0;
            else
                tmp_origin[c] = ((Real) case_index + 0.5) / (Real) n_cases1[c];
            p /= n_cases1[c];
        }

        for_less( x, 0, x_size )
        {
            offset = 0;
            for_less( c, 0, n_dims1 )
            {
                start_c = tmp_origin[c] + (Real) x * x_axis1[c];
                offset += strides1[c] * FLOOR( start_c );
            }
            x_offsets1[i][x] = offset;
        }
    }

    ALLOC2D( y_offsets1, n_slices1, y_size );
    ALLOC2D( which_x_offsets1, n_slices1, y_size );

    if( volume_data2 != (void *) NULL )
    {
        total_cases = 1;
        for_less( c, 0, n_dims2 )
        {
            delta = ABS( x_axis2[c] );
            if( delta == 0.0 )
                n_cases2[c] = 1;
            else if( delta <= 1.0 / (Real) max_cases[n_dims2-1] )
                n_cases2[c] = max_cases[n_dims2-1];
            else
            {
                n_cases2[c] = (int) (1.0 / delta) + 1;
                if( n_cases2[c] > max_cases[n_dims2-1] )
                    n_cases2[c] = max_cases[n_dims2-1];
            }

            total_cases *= n_cases2[c];
        }

        ALLOC2D( x_offsets2, total_cases, x_size );

        for_less( i, 0, total_cases )
        {
            p = i;
            for_less( c, 0, n_dims2 )
            {
                case_index = p % n_cases2[c];
                if( x_axis2[c] == 0.0 )
                    tmp_origin[c] = 0.0;
                else
                    tmp_origin[c] = ((Real) case_index + 0.5) /
                                    (Real) n_cases2[c];
                p /= n_cases2[c];
            }

            for_less( x, 0, x_size )
            {
                offset = 0;
                for_less( c, 0, n_dims2 )
                {
                    start_c = tmp_origin[c] + (Real) x * x_axis2[c];
                    offset += strides2[c] * FLOOR( start_c );
                }
                x_offsets2[i][x] = offset;
            }
        }

        ALLOC2D( y_offsets2, n_slices2, y_size );
        ALLOC2D( which_x_offsets2, n_slices2, y_size );
    }

    ALLOC( start_x, y_size );
    ALLOC( end_x, y_size );

    for_less( y, 0, y_size )
    {
        x_start = 0.0;
        x_end = (Real) (x_size - 1);
        for_less( s, 0, n_slices1 )
        {
            offset = 0;
            case_index = 0;
            case_multiplier = 1;
            for_less( c, 0, n_dims1 )
            {
                start_c = origins1[s][c] + (Real) y * y_axis1[c] + 0.5;
                int_start = FLOOR( start_c );
                remainder = start_c - int_start;
                remainder_case = (int) (remainder * n_cases1[c]);
                remainder_offset = remainder -
                                   (remainder_case + 0.5)/ n_cases1[c];
                case_index += case_multiplier * remainder_case;
                case_multiplier *= n_cases1[c];
                offset += strides1[c] * int_start;

                left_edge = 0.0;
                right_edge = (Real) sizes1[c];

                if( remainder_offset < 0.0 )
                    right_edge += remainder_offset;
                else
                    left_edge += remainder_offset;

                clip( left_edge, right_edge, start_c, x_axis1[c],
                      &x_start, &x_end );
            }

            y_offsets1[s][y] = offset;
            which_x_offsets1[s][y] = x_offsets1[case_index];
        }

        if( volume_data2 != (void *) NULL )
        {
            for_less( s, 0, n_slices2 )
            {
                offset = 0;
                case_index = 0;
                case_multiplier = 1;
                for_less( c, 0, n_dims2 )
                {
                    start_c = origins2[s][c] + (Real) y * y_axis2[c] + 0.5;
                    int_start = FLOOR( start_c );
                    remainder = start_c - int_start;
                    remainder_case = (int) (remainder * n_cases2[c]);
                    remainder_offset = remainder -
                                       (remainder_case + 0.5)/ n_cases2[c];
                    case_index += case_multiplier * remainder_case;
                    case_multiplier *= n_cases2[c];
                    offset += strides2[c] * int_start;

                    left_edge = 0.0;
                    right_edge = (Real) sizes2[c];

                    if( remainder_offset < 0.0 )
                        right_edge += remainder_offset;
                    else
                        left_edge += remainder_offset;

                    clip( left_edge, right_edge, start_c, x_axis2[c],
                          &x_start, &x_end );
                }

                y_offsets2[s][y] = offset;
                which_x_offsets2[s][y] = x_offsets2[case_index];
            }
        }

        start_x[y] = CEILING( x_start );
        end_x[y] = FLOOR( x_end );

        if( start_x[y] > x_size )
            start_x[y] = x_size;
        if( end_x[y] < 0 )
            end_x[y] = 0;
        if( start_x[y] > end_x[y] + 1 )
            start_x[y] = end_x[y] + 1;
    }

{
    int              voxel_data1, **row_offsets1;
    int              voxel_data2, **row_offsets2;
    int              *offset_ptr1, *offset_ptr2;
    Real             real_voxel_data1, real_voxel_data2;
    unsigned short   *single_cmode_map;
    Colour           *single_rgb_map;
    Pixel_types      pixel_type;

    pixel_type = pixels->pixel_type;

    if( volume_data2 == (void *) NULL )
    {
        if( pixel_type == RGB_PIXEL )
            single_rgb_map = rgb_colour_map[0];
        else
            single_cmode_map = cmode_colour_map[0];
    }

    ALLOC( row_offsets1, n_slices1 );
    if( volume_data2 != (void *) NULL )
        ALLOC( row_offsets2, n_slices2 );

    for_less( y, 0, y_size )
    {
        for_less( s, 0, n_slices1 )
            row_offsets1[s] = which_x_offsets1[s][y];

        if( volume_data2 != (void *) NULL )
        {
            for_less( s, 0, n_slices2 )
                row_offsets2[s] = which_x_offsets2[s][y];
        }

        if( pixel_type == RGB_PIXEL )
        {
            Colour          *pixel_ptr, colour;

            pixel_ptr = &pixels->data.pixels_rgb[IJ(y,0,x_size)];

            for_less( x, 0, start_x[y] )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }

#include    "render_include1.c"

            for_less( x, end_x[y]+1, x_size )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }
        }
        else
        {
            unsigned short          *pixel_ptr, colour;

            pixel_ptr = &pixels->data.pixels_16bit_colour_index[IJ(y,0,x_size)];

            for_less( x, 0, start_x[y] )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }

#define     COLOUR_MAP
#include    "render_include1.c"

            for_less( x, end_x[y]+1, x_size )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }
        }
    }

    FREE( row_offsets1 );
    if( volume_data2 != (void *) NULL )
        FREE( row_offsets2 );
}

    FREE2D( x_offsets1 );
    FREE2D( y_offsets1 );
    FREE2D( which_x_offsets1 );

    if( volume_data2 != (void *) NULL )
    {
        FREE2D( x_offsets2 );
        FREE2D( y_offsets2 );
        FREE2D( which_x_offsets2 );
    }

    FREE( start_x );
    FREE( end_x );
}
