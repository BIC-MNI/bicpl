
#include  <def_mni.h>

public  Status  initialize_pixels(
    pixels_struct  *pixels,
    int            x_position,
    int            y_position,
    int            x_size,
    int            y_size,
    Pixel_types    pixel_type )
{
    int            n_alloced;
    Status         status;

    n_alloced = 0;

    pixels->pixel_type = pixel_type;
    pixels->x_position = x_position;
    pixels->y_position = y_position;

    status = modify_pixels_size( &n_alloced, pixels, x_size, y_size,
                                 pixel_type );

    return( status );
}

public  Status  delete_pixels( pixels_struct *pixels )
{
    Status   status;

    status = OK;

    if( pixels->x_size > 0 && pixels->y_size > 0 )
    {
        switch( pixels->pixel_type )
        {
        case COLOUR_INDEX_8BIT_PIXEL:
            FREE( status, pixels->data.pixels_8bit_colour_index );
            break;

        case COLOUR_INDEX_16BIT_PIXEL:
            FREE( status, pixels->data.pixels_16bit_colour_index );
            break;

        case RGB_PIXEL:
            FREE( status, pixels->data.pixels_rgb );
            break;
        }
    }

    return( status );
}

public  Status  modify_pixels_size(
    int            *n_pixels_alloced,
    pixels_struct  *pixels,
    int            x_size,
    int            y_size,
    Pixel_types    pixel_type )
{
    int                new_n_pixels;
    unsigned long      *tmp_int_ptr;
    Status             status;

    status = OK;

    if( pixel_type != pixels->pixel_type )
    {
        if( *n_pixels_alloced > 0 )
            status = delete_pixels( pixels );
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

        SET_ARRAY_SIZE( status, tmp_int_ptr, (*n_pixels_alloced + 3) / 4,
                        (new_n_pixels + 3) / 4, DEFAULT_CHUNK_SIZE );
        pixels->data.pixels_8bit_colour_index = (unsigned char *)
                         ((void *) tmp_int_ptr );
        break;

    case COLOUR_INDEX_16BIT_PIXEL:    /* make sure it's word aligned */
        tmp_int_ptr = (unsigned long *) (void *)
                          pixels->data.pixels_16bit_colour_index;
        SET_ARRAY_SIZE( status, tmp_int_ptr, (*n_pixels_alloced + 1) / 2,
                        (new_n_pixels + 1) / 2, DEFAULT_CHUNK_SIZE );
        pixels->data.pixels_16bit_colour_index = (unsigned short *)
                         ((void *) tmp_int_ptr );
        break;

    case RGB_PIXEL:
        SET_ARRAY_SIZE( status, pixels->data.pixels_rgb, *n_pixels_alloced,
                        new_n_pixels, DEFAULT_CHUNK_SIZE );
        break;
    }

    *n_pixels_alloced = new_n_pixels;

    return( status );
}
