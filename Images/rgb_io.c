#include <internal_volume_io.h>
#include <images.h>

#ifdef  sgi
#include <image.h>
#endif

public  Status  input_rgb_file(
    char            filename[],
    pixels_struct   *pixels )
{
#ifdef  sgi
    IMAGE        *iimage;
    unsigned int x_size, y_size, z_size;
    unsigned int x, y;
    int          r, g, b;
    short        rbuf[8192];
    short        gbuf[8192];
    short        bbuf[8192];

    if( (iimage = iopen(filename,"r")) == NULL )
    {
        print( "Error: can't open input file %s\n", filename );
        return( ERROR );
    }

    x_size = iimage->xsize;
    y_size = iimage->ysize;
    z_size = iimage->zsize;

    initialize_pixels( pixels, 0, 0, x_size, y_size, 1.0, 1.0, RGB_PIXEL );

    if( z_size != 3 )
    {
        print( "Error: z_size (%d) != 3\n", z_size );
        return( ERROR );
    }

    for_less( y, 0, y_size )
    {
        getrow( iimage, rbuf, y, 0 );
        getrow( iimage, gbuf, y, 1 );
        getrow( iimage, bbuf, y, 2 );

        for_less( x, 0, x_size )
        {
            r = rbuf[x];
            g = gbuf[x];
            b = bbuf[x];
            PIXEL_RGB_COLOUR( *pixels, x, y ) = make_Colour( r, g, b );
        }
    }

    iclose(iimage);
#endif
    return( OK );
}

public  Status  output_rgb_file(
    char            filename[],
    pixels_struct   *pixels )
{
#ifdef  sgi
    IMAGE        *oimage;
    unsigned int x, y;
    Colour       col;
    short        rbuf[8192];
    short        gbuf[8192];
    short        bbuf[8192];

    oimage = iopen( filename, "w", RLE(1), 3, pixels->x_size, pixels->y_size,3);
    if( oimage == NULL )
    {
        print( "Error: can't open output file %s\n", filename );
        return( ERROR );
    }

    for_less( y, 0, pixels->y_size )
    {
        for_less( x, 0, pixels->x_size )
        {
            col = PIXEL_RGB_COLOUR( *pixels, x, y );
            rbuf[x] = get_Colour_r( col );
            gbuf[x] = get_Colour_g( col );
            bbuf[x] = get_Colour_b( col );
        }

	putrow( oimage, rbuf, y, 0 );
	putrow( oimage, gbuf, y, 1 );
	putrow( oimage, bbuf, y, 2 );
    }

    iclose(oimage);
#endif

    return( OK );
}
