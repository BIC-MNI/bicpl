
#include  <def_objects.h>
#include  <def_alloc.h>

public  Status  initialize_pixels( pixels, x_size, y_size, pixel_type )
    pixels_struct  *pixels;
    int            x_size;
    int            y_size;
    Pixel_types    pixel_type;
{
    Status   status;

    pixels->pixel_type = pixel_type;
    pixels->x_size = x_size;
    pixels->y_size = y_size;

    if( x_size > 0 && y_size > 0 )
    {
        switch( pixel_type )
        {
        case COLOUR_INDEX_PIXEL:
            ALLOC( status, pixels->data.pixels_colour_index, x_size * y_size );
            break;

        case RGB_PIXEL:
            ALLOC( status, pixels->data.pixels_rgb, x_size * y_size );
            break;

        case RGB_8BIT_PIXEL:
            ALLOC( status, pixels->data.pixels_8bit_rgb, x_size * y_size );
            break;
        }
    }

    return( status );
}

public  Status  delete_pixels( pixels )
    pixels_struct  *pixels;
{
    Status   status;

    status = OK;

    if( pixels->x_size > 0 && pixels->y_size > 0 )
    {
        switch( pixels->pixel_type )
        {
        case COLOUR_INDEX_PIXEL:
            FREE( status, pixels->data.pixels_colour_index );
            break;

        case RGB_PIXEL:
            FREE( status, pixels->data.pixels_rgb );
            break;

        case RGB_8BIT_PIXEL:
            FREE( status, pixels->data.pixels_8bit_rgb );
            break;
        }
    }

    return( status );
}
