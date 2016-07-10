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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include  "bicpl_internal.h"

#include <limits.h>
#include <float.h>

#include  "render_funcs_include.c"

/* ----------------------------- MNI Header -----------------------------------
@NAME       : render_one_row
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Renders a row of a slice, by including the generated source
              files which handle each of the cases separately, for speed.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  render_one_row (
    void            *volume_data1,
    VIO_Data_types      volume1_type,
    int             y,
    int             start_x,
    int             end_x,
    size_t          **y_offsets1,
    size_t          **row_offsets1,
    void            **start_slices1,
    int             n_slices1,
    VIO_Real            weights1[],
    void            *volume_data2,
    VIO_Data_types      volume2_type,
    size_t          **y_offsets2,
    size_t          **row_offsets2,
    void            **start_slices2,
    int             n_slices2,
    VIO_Real            weights2[],
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
    int             x_size;
    VIO_Colour          *rgb_pixel_ptr;
    unsigned short  *cmap_pixel_ptr;

    x_size = pixels->x_size;

    if( pixels->pixel_type == RGB_PIXEL )
        rgb_pixel_ptr = &pixels->data.pixels_rgb[VIO_IJ(y,start_x,x_size)];
    else
        cmap_pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                      [VIO_IJ(y,start_x,x_size)];

#include "call_rend_f_include.c"
}
