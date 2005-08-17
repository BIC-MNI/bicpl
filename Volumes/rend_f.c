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

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/rend_f.c,v 1.9 2005-08-17 22:26:19 bert Exp $";
#endif

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
    Data_types      volume1_type,
    int             y,
    int             start_x,
    int             end_x,
    int             **y_offsets1,
    int             **row_offsets1,
    void            **start_slices1,
    int             n_slices1,
    Real            weights1[],
    void            *volume_data2,
    Data_types      volume2_type,
    int             **y_offsets2,
    int             **row_offsets2,
    void            **start_slices2,
    int             n_slices2,
    Real            weights2[],
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
    int             x_size;
    Colour          *rgb_pixel_ptr;
    unsigned short  *cmap_pixel_ptr;

    x_size = pixels->x_size;

    if( pixels->pixel_type == RGB_PIXEL )
        rgb_pixel_ptr = &pixels->data.pixels_rgb[IJ(y,start_x,x_size)];
    else
        cmap_pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                      [IJ(y,start_x,x_size)];

#include "call_rend_f_include.c"
}
