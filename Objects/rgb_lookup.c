
#include  <internal_volume_io.h>
#include  <objects.h>

#define  NUM_PIXELS8   256

public  Colour     *get_8bit_rgb_pixel_lookup( void )
{
    static  BOOLEAN       initialized = FALSE;
    static  Colour        lookup_table[NUM_PIXELS8];
    int                   r, g, b, index, r255, g255, b255;

    if( !initialized )
    {
        initialized = TRUE;

        for_less( r, 0, 8 )
        {
            for_less( g, 0, 8 )
            {
                for_less( b, 0, 4 )
                {
                    index = ( r | (g << 3) | (b << 6) );

                    r255 = (int) ((r + 0.5) * 256.0 / 8.0);
                    g255 = (int) ((g + 0.5) * 256.0 / 8.0);
                    b255 = (int) ((b + 0.5) * 256.0 / 4.0);

                    lookup_table[index] = make_Colour( r255, g255, b255 );
                }
            }
        }
    }

    return( lookup_table );
}

public  int  convert_rgb_pixel_to_8bit_lookup(
    Colour    colour )
{
    int   ind, r, g, b, r_ind, g_ind, b_ind;

    r = get_Colour_r( colour );
    g = get_Colour_g( colour );
    b = get_Colour_b( colour );

    r_ind = 8 * r / 256;
    g_ind = 8 * g / 256;
    b_ind = 4 * b / 256;
    ind = r_ind | (g_ind << 3) | (b_ind << 6);

    return( ind );
}
