
#include  <mni.h>

public  void  initialize_pixels(
    pixels_struct  *pixels,
    int            x_position,
    int            y_position,
    int            x_size,
    int            y_size,
    Real           x_zoom,
    Real           y_zoom,
    Pixel_types    pixel_type )
{
    int            n_alloced;

    n_alloced = 0;

    pixels->pixel_type = pixel_type;
    pixels->x_size = -1;
    pixels->y_size = -1;
    pixels->x_position = x_position;
    pixels->y_position = y_position;
    pixels->x_zoom = x_zoom;
    pixels->y_zoom = y_zoom;

    modify_pixels_size( &n_alloced, pixels, x_size, y_size, pixel_type );
}

public  void  delete_pixels( pixels_struct *pixels )
{
    if( pixels->x_size > 0 && pixels->y_size > 0 )
    {
        switch( pixels->pixel_type )
        {
        case COLOUR_INDEX_8BIT_PIXEL:
            FREE( pixels->data.pixels_8bit_colour_index );
            break;

        case COLOUR_INDEX_16BIT_PIXEL:
            FREE( pixels->data.pixels_16bit_colour_index );
            break;

        case RGB_PIXEL:
            FREE( pixels->data.pixels_rgb );
            break;
        }
    }
}

public  void  modify_pixels_size(
    int            *n_pixels_alloced,
    pixels_struct  *pixels,
    int            x_size,
    int            y_size,
    Pixel_types    pixel_type )
{
    int                new_n_pixels;
    unsigned long      *tmp_int_ptr;

    if( pixel_type != pixels->pixel_type )
    {
        if( *n_pixels_alloced > 0 )
            delete_pixels( pixels );
        *n_pixels_alloced = 0;
        pixels->pixel_type = pixel_type;
    }

    if( x_size > 0 && y_size > 0 )
        new_n_pixels = x_size * y_size;
    else
        new_n_pixels = 0;

    pixels->x_size = x_size;
    pixels->y_size = y_size;

    switch( pixel_type )
    {
    case COLOUR_INDEX_8BIT_PIXEL:    /* make sure it's word aligned */
        tmp_int_ptr = (unsigned long *) (void *)
                          pixels->data.pixels_8bit_colour_index;

        SET_ARRAY_SIZE( tmp_int_ptr, (*n_pixels_alloced + 3) / 4,
                        (new_n_pixels + 3) / 4, DEFAULT_CHUNK_SIZE );
        pixels->data.pixels_8bit_colour_index = (unsigned char *)
                         ((void *) tmp_int_ptr );
        break;

    case COLOUR_INDEX_16BIT_PIXEL:    /* make sure it's word aligned */
        tmp_int_ptr = (unsigned long *) (void *)
                          pixels->data.pixels_16bit_colour_index;
        SET_ARRAY_SIZE( tmp_int_ptr, (*n_pixels_alloced + 1) / 2,
                        (new_n_pixels + 1) / 2, DEFAULT_CHUNK_SIZE );
        pixels->data.pixels_16bit_colour_index = (unsigned short *)
                         ((void *) tmp_int_ptr );
        break;

    case RGB_PIXEL:
        SET_ARRAY_SIZE( pixels->data.pixels_rgb, *n_pixels_alloced,
                        new_n_pixels, DEFAULT_CHUNK_SIZE );
        break;
    }

    *n_pixels_alloced = new_n_pixels;
}
