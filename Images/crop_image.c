#include  <internal_volume_io.h>
#include  <images.h>

private  BOOLEAN  is_clear_slab(
    pixels_struct  *pixels,
    Colour         bg_colour,
    int            x_min,
    int            y_min,
    int            x_max,
    int            y_max )
{
    int  x, y;

    for_inclusive( x, x_min, x_max )
    {
        for_inclusive( y, y_min, y_max )
        {
            if( PIXEL_RGB_COLOUR(*pixels,x,y) != bg_colour )
                return( FALSE );
        }
    }

    return( TRUE );
}

public  void  crop_pixels(
    pixels_struct  *in_pixels,
    Colour         background_colour,
    int            border,
    pixels_struct  *out_pixels )
{
    int   x, y;
    int   x_min, x_max, y_min, y_max;

    if( in_pixels->pixel_type != RGB_PIXEL )
    {
        print( "cannot crop non-rgb pixels.\n" );
        return;
    }

    x_min = 0;
    while( x_min < in_pixels->x_size &&
           is_clear_slab( in_pixels, background_colour,
                          x_min, 0, x_min, in_pixels->y_size-1 ) )
        ++x_min;

    x_max = in_pixels->x_size-1;
    while( x_max >= 0 &&
           is_clear_slab( in_pixels, background_colour,
                          x_max, 0, x_max, in_pixels->y_size-1 ) )
        --x_max;

    y_min = 0;
    while( y_min < in_pixels->y_size &&
           is_clear_slab( in_pixels, background_colour,
                          0, y_min, in_pixels->x_size-1, y_min ) )
        ++y_min;

    y_max = in_pixels->y_size-1;
    while( y_max >= 0 &&
           is_clear_slab( in_pixels, background_colour,
                          0, y_max, in_pixels->x_size-1, y_max ) )
        --y_max;

    x_max = MIN( in_pixels->x_size-1, x_max + border );
    x_min = MAX( 0,                   x_min - border );
    y_max = MIN( in_pixels->y_size-1, y_max + border );
    y_min = MAX( 0,                   y_min - border );

    if( x_max < x_min )
        x_max = x_min - 1;
    if( y_max < y_min )
        y_max = y_min - 1;

    initialize_pixels( out_pixels, in_pixels->x_position + x_min,
                       in_pixels->y_position + y_min,
                       x_max - x_min + 1, y_max - y_min + 1,
                       in_pixels->x_zoom, in_pixels->y_zoom,
                       RGB_PIXEL );

    for_inclusive( x, x_min, x_max )
    {
        for_inclusive( y, y_min, y_max )
        {
            PIXEL_RGB_COLOUR(*out_pixels,x-x_min,y-y_min) =
                      PIXEL_RGB_COLOUR(*in_pixels,x,y);
        }
    }
}
