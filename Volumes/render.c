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

#include  <volume_io/internal_volume_io.h>
#include  <vols.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/render.c,v 1.40 2000-02-05 21:27:27 stever Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : clip
@INPUT      : left_edge
              right_edge
              origin
              delta
@OUTPUT     : t_min
              t_max
@RETURNS    : 
@DESCRIPTION: Clips one row of a slice.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

typedef  struct
{
    int     **x_offsets1;
    int     **y_offsets1;
    int     ***which_x_offsets1;
    void    **start_slices1;
    int     **row_offsets1;

    int     **x_offsets2;
    int     **y_offsets2;
    int     ***which_x_offsets2;
    void    **start_slices2;
    int     **row_offsets2;

    int     *start_x;
    int     *end_x;

    int     x_size1_alloced;
    int     y_size1_alloced;
    int     total_cases1_alloced;
    int     n_slices1_alloced;

    int     x_size2_alloced;
    int     y_size2_alloced;
    int     total_cases2_alloced;
    int     n_slices2_alloced;
}
render_storage_struct;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_render_storage
@INPUT      : 
@OUTPUT     : 
@RETURNS    : pointer to storage
@DESCRIPTION: Creates a structure containing rendering arrays, so that it
              can be used for many slice renders, without allocating many times.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   *initialize_render_storage( void )
{
    render_storage_struct  *store;
    void                   *void_ptr;

    ALLOC( store, 1 );

    store->x_size1_alloced = 0;
    store->y_size1_alloced = 0;
    store->total_cases1_alloced = 0;
    store->n_slices1_alloced = 0;

    store->x_size2_alloced = 0;
    store->y_size2_alloced = 0;
    store->total_cases2_alloced = 0;
    store->n_slices2_alloced = 0;

    void_ptr = (void *) store;

    return( void_ptr );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_render_storage
@INPUT      : ptr
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Deletes the render storage.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void   delete_render_storage(
    void  *ptr )
{
    render_storage_struct  *store;

    store = (render_storage_struct *) ptr;

    if( store->total_cases1_alloced > 0 && store->x_size1_alloced > 0 )
        FREE2D( store->x_offsets1 );

    if( store->n_slices1_alloced > 0 && store->y_size1_alloced > 0 )
    {
        FREE2D( store->y_offsets1 );
        FREE2D( store->which_x_offsets1 );
    }

    if( store->n_slices1_alloced > 0 )
    {
        FREE( store->start_slices1 );
        FREE( store->row_offsets1 );
    }

    if( store->total_cases2_alloced > 0 && store->x_size2_alloced > 0 )
        FREE2D( store->x_offsets2 );

    if( store->n_slices2_alloced > 0 && store->y_size2_alloced > 0 )
    {
        FREE2D( store->y_offsets2 );
        FREE2D( store->which_x_offsets2 );
    }

    if( store->n_slices2_alloced > 0 )
    {
        FREE( store->start_slices2 );
        FREE( store->row_offsets2 );
    }

    if( store->y_size1_alloced > 0 )
    {
        FREE( store->start_x );
        FREE( store->end_x );
    }

    FREE( store );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : render_volume_to_slice
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Renders a slice.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    void            *render_storage,
    pixels_struct   *pixels )
{
    int     i, c, p, total_cases1, total_cases2, case_index, case_multiplier;
    int     offset, int_start;
    int     s, x, y, n_cases1[MAX_DIMENSIONS], n_cases2[MAX_DIMENSIONS];
    int     **x_offsets1, **y_offsets1, *start_x, *end_x;
    int     **x_offsets2, **y_offsets2;
    int     ***which_x_offsets1, ***which_x_offsets2;
    int     remainder_case, x_size, y_size;
    int     **row_offsets1, **row_offsets2;
    int     x_left, x_right, n_non_zero;
    void    **start_slices1, **start_slices2;
    Real    start_c, x_start, x_end, remainder, tmp_origin[MAX_DIMENSIONS];
    Real    remainder_offset, left_edge, right_edge, delta;
    render_storage_struct  *store;
    static  int   max_cases[MAX_DIMENSIONS] = { 10, 10, 4, 3, 3 };
    int     new_total_cases1, new_x_size1, new_y_size1, new_n_slices1;
    int     new_total_cases2, new_x_size2, new_y_size2, new_n_slices2;

    if( render_storage == NULL )
        store = (render_storage_struct *) initialize_render_storage();
    else
        store = (render_storage_struct *) render_storage;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    n_non_zero = 0;
    for_less( c, 0, n_dims1 )
    {
        if( y_axis1[c] != 0.0 )
            ++n_non_zero;
    }

    total_cases1 = 1;
    for_less( c, 0, n_dims1 )
    {
        delta = FABS( y_axis1[c] );
        if( delta == 0.0 || n_non_zero == 1 )
            n_cases1[c] = 1;
        else if( delta <= 1.0 / (Real) max_cases[n_dims1-1] )
            n_cases1[c] = max_cases[n_dims1-1];
        else
        {
            n_cases1[c] = (int) (1.0 / delta);
            if( n_cases1[c] < 1 )
                n_cases1[c] = 1;
            else if( n_cases1[c] > max_cases[n_dims1-1] )
                n_cases1[c] = max_cases[n_dims1-1];
        }

        total_cases1 *= n_cases1[c];
    }

    new_total_cases1 = MAX( total_cases1, store->total_cases1_alloced );
    new_x_size1 = MAX( x_size, store->x_size1_alloced );
    new_y_size1 = MAX( y_size, store->y_size1_alloced );
    new_n_slices1 = MAX( n_slices1, store->n_slices1_alloced );

    if( new_total_cases1 > store->total_cases1_alloced ||
        new_x_size1 > store->x_size1_alloced )
    {
        if( store->total_cases1_alloced > 0 && store->x_size1_alloced > 0 )
            FREE2D( store->x_offsets1 );
        ALLOC2D( store->x_offsets1, new_total_cases1, new_x_size1 );
    }

    if( new_n_slices1 > store->n_slices1_alloced ||
        new_y_size1 > store->y_size1_alloced )
    {
        if( store->n_slices1_alloced > 0 && store->y_size1_alloced > 0 )
        {
            FREE2D( store->y_offsets1 );
            FREE2D( store->which_x_offsets1 );
        }
        ALLOC2D( store->y_offsets1, new_n_slices1, new_y_size1 );
        ALLOC2D( store->which_x_offsets1, new_n_slices1, new_y_size1 );
    }

    
    if( new_n_slices1 > store->n_slices1_alloced )
    {
        SET_ARRAY_SIZE( store->start_slices1, store->n_slices1_alloced,
                        new_n_slices1, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( store->row_offsets1, store->n_slices1_alloced,
                        new_n_slices1, DEFAULT_CHUNK_SIZE );
    }

    if( new_y_size1 > store->y_size1_alloced )
    {
        SET_ARRAY_SIZE( store->start_x, store->y_size1_alloced,
                        new_y_size1, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( store->end_x, store->y_size1_alloced,
                        new_y_size1, DEFAULT_CHUNK_SIZE );
    }

    store->total_cases1_alloced = new_total_cases1;
    store->n_slices1_alloced = new_n_slices1;
    store->x_size1_alloced = new_x_size1;
    store->y_size1_alloced = new_y_size1;

    x_offsets1 = store->x_offsets1;
    y_offsets1 = store->y_offsets1;
    which_x_offsets1 = store->which_x_offsets1;
    start_slices1 = store->start_slices1;

    start_x = store->start_x;
    end_x = store->end_x;
    row_offsets1 = store->row_offsets1;

    if( volume_data2 != (void *) NULL )
    {
        n_non_zero = 0;
        for_less( c, 0, n_dims1 )
        {
            if( y_axis1[c] != 0.0 )
                ++n_non_zero;
        }

        total_cases2 = 1;
        for_less( c, 0, n_dims2 )
        {
            delta = FABS( y_axis2[c] );
            if( delta == 0.0 || n_non_zero == 1 )
                n_cases2[c] = 1;
            else if( delta <= 1.0 / (Real) max_cases[n_dims2-1] )
                n_cases2[c] = max_cases[n_dims2-1];
            else
            {
                n_cases2[c] = (int) (1.0 / delta);
                if( n_cases2[c] < 1 )
                    n_cases2[c] = 1;
                else if( n_cases2[c] > max_cases[n_dims2-1] )
                    n_cases2[c] = max_cases[n_dims2-1];
            }

            total_cases2 *= n_cases2[c];
        }

        new_total_cases2 = MAX( total_cases2, store->total_cases2_alloced );
        new_x_size2 = MAX( x_size, store->x_size2_alloced );
        new_y_size2 = MAX( y_size, store->y_size2_alloced );
        new_n_slices2 = MAX( n_slices2, store->n_slices2_alloced );

        if( new_total_cases2 > store->total_cases2_alloced ||
            new_x_size2 > store->x_size2_alloced )
        {
            if( store->total_cases2_alloced > 0 && store->x_size2_alloced > 0 )
                FREE2D( store->x_offsets2 );
            ALLOC2D( store->x_offsets2, new_total_cases2, new_x_size2 );
        }

        if( new_n_slices2 > store->n_slices2_alloced ||
            new_y_size2 > store->y_size2_alloced )
        {
            if( store->n_slices2_alloced > 0 && store->y_size2_alloced > 0 )
            {
                FREE2D( store->y_offsets2 );
                FREE2D( store->which_x_offsets2 );
            }
            ALLOC2D( store->y_offsets2, new_n_slices2, new_y_size2 );
            ALLOC2D( store->which_x_offsets2, new_n_slices2, new_y_size2 );
        }
    
        if( new_n_slices2 > store->n_slices2_alloced )
        {
            SET_ARRAY_SIZE( store->start_slices2, store->n_slices2_alloced,
                            new_n_slices2, DEFAULT_CHUNK_SIZE );
            SET_ARRAY_SIZE( store->row_offsets2, store->n_slices2_alloced,
                            new_n_slices2, DEFAULT_CHUNK_SIZE );
        }

        store->total_cases2_alloced = new_total_cases2;
        store->n_slices2_alloced = new_n_slices2;
        store->x_size2_alloced = new_x_size2;
        store->y_size2_alloced = new_y_size2;

        x_offsets2 = store->x_offsets2;
        y_offsets2 = store->y_offsets2;
        which_x_offsets2 = store->which_x_offsets2;
        start_slices2 = store->start_slices2;
        row_offsets2 = store->row_offsets2;
    }

    for_less( i, 0, total_cases1 )
    {
        p = i;
        for_less( c, 0, n_dims1 )
        {
            case_index = p % n_cases1[c];
            if( y_axis1[c] == 0.0 && n_slices1 == n_cases1[c] )
                tmp_origin[c] = FRACTION( origins1[case_index][c] + 0.5 );
            else
                tmp_origin[c] = ((Real) case_index + 0.5) / (Real) n_cases1[c];
            p /= n_cases1[c];
        }

        for_inclusive( x, x_pixel_start, x_pixel_end )
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

    if( volume_data2 != (void *) NULL )
    {
        for_less( i, 0, total_cases2 )
        {
            p = i;
            for_less( c, 0, n_dims2 )
            {
                case_index = p % n_cases2[c];
                if( y_axis2[c] == 0.0 && n_slices2 == n_cases2[c] )
                    tmp_origin[c] = FRACTION( origins2[case_index][c] + 0.5 );
                else
                    tmp_origin[c] = ((Real) case_index + 0.5) /
                                    (Real) n_cases2[c];
                p /= n_cases2[c];
            }

            for_inclusive( x, x_pixel_start, x_pixel_end )
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
    }

    for_inclusive( y, y_pixel_start, y_pixel_end )
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

                if( y_axis1[c] == 0.0 && n_slices1 == n_cases1[c] )
                {
                    remainder_case = s;
                    remainder_offset = 0.0;
                }
                else
                {
                    remainder = start_c - (Real) int_start;
                    remainder_case = (int) (remainder * (Real) n_cases1[c]);
                    remainder_offset = remainder -
                           ((Real) remainder_case + 0.5)/ (Real) n_cases1[c];
                }

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

                    if( y_axis2[c] == 0.0 && n_slices2 == n_cases2[c] )
                    {
                        remainder_case = s;
                        remainder_offset = 0.0;
                    }
                    else
                    {
                        remainder = start_c - (Real) int_start;
                        remainder_case = (int) (remainder * (Real) n_cases2[c]);
                        remainder_offset = remainder -
                             ((Real) remainder_case + 0.5)/ (Real) n_cases2[c];
                    }

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

    for_inclusive( y, y_pixel_start, y_pixel_end )
    {
        for_less( s, 0, n_slices1 )
            row_offsets1[s] = which_x_offsets1[s][y];

        if( volume_data2 != (void *) NULL )
        {
            for_less( s, 0, n_slices2 )
                row_offsets2[s] = which_x_offsets2[s][y];
        }

        x_left = MAX( x_pixel_start, start_x[y] );
        x_right = MIN( x_pixel_end, end_x[y] );

        /* there is a core-dumping bug somewhere */
        /* this is a temporary fix for register */

        if( volume_data2 != NULL && total_cases2 != 1 )
        {
            ++x_left;
            --x_right;
        }

        if( x_left <= x_right )
        {
            render_one_row( volume_data1, volume1_type,
                            y, x_left, x_right,
                            y_offsets1, row_offsets1, start_slices1,
                            n_slices1, weights1,
                            volume_data2, volume2_type,
                            y_offsets2, row_offsets2, start_slices2,
                            n_slices2, weights2,
                            cmode_colour_map,
                            rgb_colour_map,
                            pixels );
        }

        if( pixels->pixel_type == RGB_PIXEL )
        {
            Colour          *pixel_ptr;

            pixel_ptr = &pixels->data.pixels_rgb[IJ(y,x_pixel_start,x_size)];

            for_less( x, x_pixel_start, start_x[y] )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }

            pixel_ptr = &pixels->data.pixels_rgb[IJ(y,end_x[y]+1,x_size)];
            for_less( x, end_x[y]+1, x_pixel_end+1 )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }
        }
        else
        {
            unsigned short          *pixel_ptr;

            pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                              [IJ(y,x_pixel_start,x_size)];
            for_less( x, x_pixel_start, start_x[y] )
            {
                *pixel_ptr = (unsigned short) empty_colour;
                ++pixel_ptr;
            }

            pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                              [IJ(y,end_x[y]+1,x_size)];
            for_less( x, end_x[y]+1, x_pixel_end+1 )
            {
                *pixel_ptr = (unsigned short) empty_colour;
                ++pixel_ptr;
            }
        }
    }

    if( render_storage == NULL )
        delete_render_storage( (void *) store );
}
