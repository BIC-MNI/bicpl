#include  <internal_volume_io.h>
#include  <vols.h>

#include  "render_funcs_include.c"

public  void  render_the_slice (
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
