#include <internal_volume_io.h>
#include <images.h>

static  void  error_function(
    char  error[] );

#ifdef  __sgi
#include <image.h>

int getrow(IMAGE *image, unsigned short *buffer,
                unsigned int y, unsigned int z);

int iclose(IMAGE *image);

int putrow(IMAGE *image, unsigned short *buffer,
                unsigned int y, unsigned int z);
#endif

public  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
#ifdef  __sgi
    IMAGE                 *iimage;
    unsigned int          x_size, y_size, z_size;
    unsigned int          x, y;
    int                   r, g, b;
    unsigned short        rbuf[8192];
    unsigned short        gbuf[8192];
    unsigned short        bbuf[8192];

    i_seterror( error_function );

    if( (iimage = iopen(filename,"r")) == NULL )
    {
        return( ERROR );
    }

    x_size = iimage->xsize;
    y_size = iimage->ysize;
    z_size = iimage->zsize;

    initialize_pixels( pixels, 0, 0, x_size, y_size, 1.0, 1.0, RGB_PIXEL );

    if( z_size != 3 )
    {
        print_error( "Error: z_size (%d) != 3\n", z_size );
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
    STRING          filename,
    pixels_struct   *pixels )
{
#ifdef  __sgi
    IMAGE                 *oimage;
    unsigned int          x, y;
    Colour                col;
    unsigned short        rbuf[8192];
    unsigned short        gbuf[8192];
    unsigned short        bbuf[8192];

    i_seterror( error_function );

    if( !file_directory_exists( filename ) )
    {
        print_error( "Error: output file directory does not exist: %s\n",
                     filename );
        return( ERROR );
    }

    oimage = iopen( filename, "w", RLE(1), 3, pixels->x_size, pixels->y_size,3);
    if( oimage == NULL )
    {
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

static  void  error_function(
    char  error[] )
{
    print_error( error );
}
