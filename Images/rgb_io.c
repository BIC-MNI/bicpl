#include <internal_volume_io.h>
#include <objects.h>
#include <images.h>

static  void  error_function(
    char  error[] );

#ifdef  __sgi
#if _MIPS_ISA == 1
#define  HAVE_IMAGE_LIBRARY
#endif
#endif

#ifdef  HAVE_IMAGE_LIBRARY
#include <image.h>

extern  unsigned short *ibufalloc(IMAGE *image);

extern IMAGE *iopen( char *file, char *mode, unsigned int type,
                     unsigned int dim, unsigned int xsize,
                     unsigned int ysize, unsigned int zsize );

extern int getrow(IMAGE *image, unsigned short *buffer,
                unsigned int y, unsigned int z);

extern int iclose(IMAGE *image);

extern int putrow(IMAGE *image, unsigned short *buffer,
                unsigned int y, unsigned int z);

extern int i_seterror( void (*func)() );

#endif

public  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
#ifdef  HAVE_IMAGE_LIBRARY
    IMAGE                 *iimage;
    int                   x_size, y_size, z_size;
    int                   x, y;
    int                   r, g, b, a;
    unsigned short        rbuf[8192];
    unsigned short        gbuf[8192];
    unsigned short        bbuf[8192];
    unsigned short        abuf[8192];

    i_seterror( error_function );

    if( (iimage = iopen(filename,"r",0,0,0,0,0)) == NULL )
    {
        return( ERROR );
    }

    x_size = (int) iimage->xsize;
    y_size = (int) iimage->ysize;
    z_size = (int) iimage->zsize;

    initialize_pixels( pixels, 0, 0, x_size, y_size, 1.0, 1.0, RGB_PIXEL );

    if( z_size != 3 && z_size != 4 )
    {
        print_error( "Error: z_size (%d) != 3 or 4\n", z_size );
        return( ERROR );
    }

    for_less( y, 0, y_size )
    {
        getrow( iimage, rbuf, (unsigned int) y, 0 );
        getrow( iimage, gbuf, (unsigned int) y, 1 );
        getrow( iimage, bbuf, (unsigned int) y, 2 );
        if( z_size == 4 )
            getrow( iimage, abuf, (unsigned int) y, 3 );

        for_less( x, 0, x_size )
        {
            r = (int) rbuf[x];
            g = (int) gbuf[x];
            b = (int) bbuf[x];
            if( z_size == 4 )
                a = (int) abuf[x];
            else
                a = 255;

            PIXEL_RGB_COLOUR( *pixels, x, y ) = make_rgba_Colour( r, g, b, a );
        }
    }

    iclose(iimage);
#else
    print_error( "input_rgb_file(), missing image library.\n" );
#endif
    return( OK );
}

public  Status  output_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
#ifdef  HAVE_IMAGE_LIBRARY
    IMAGE                 *oimage;
    int                   x, y, n_components;
    Colour                col;
    unsigned short        rbuf[8192];
    unsigned short        gbuf[8192];
    unsigned short        bbuf[8192];
    unsigned short        abuf[8192];

    i_seterror( error_function );

    if( !file_directory_exists( filename ) )
    {
        print_error( "Error: output file directory does not exist: %s\n",
                     filename );
        return( ERROR );
    }

    n_components = 3;
    for_less( y, 0, pixels->y_size )
    {
        for_less( x, 0, pixels->x_size )
        {
            col = PIXEL_RGB_COLOUR( *pixels, x, y );
            if( get_Colour_a( col ) != 255 )
            {
                n_components = 4;
                break;
            }
        }

        if( n_components == 4 )
            break;
    }

    oimage = iopen( filename, "w", RLE(1), 3, (unsigned int) pixels->x_size,
                    (unsigned int) pixels->y_size,
                    (unsigned int) n_components );

    if( oimage == NULL )
    {
        return( ERROR );
    }

    for_less( y, 0, pixels->y_size )
    {
        for_less( x, 0, pixels->x_size )
        {
            col = PIXEL_RGB_COLOUR( *pixels, x, y );
            rbuf[x] = (unsigned short) get_Colour_r( col );
            gbuf[x] = (unsigned short) get_Colour_g( col );
            bbuf[x] = (unsigned short) get_Colour_b( col );
            if( n_components == 4 )
                abuf[x] = (unsigned short) get_Colour_a( col );
        }

	putrow( oimage, rbuf, (unsigned int) y, 0 );
	putrow( oimage, gbuf, (unsigned int) y, 1 );
	putrow( oimage, bbuf, (unsigned int) y, 2 );
        if( n_components == 4 )
	    putrow( oimage, abuf, (unsigned int) y, 3 );
    }

    iclose(oimage);
#else
    print_error( "output_rgb_file(), missing image library.\n" );
#endif

    return( OK );
}

static  void  error_function(
    char  error[] )
{
    print_error( error );
}
