/*
 * Output (2D) image in PPM format.
 */

#include "config.h"

#include <volume_io/internal_volume_io.h>
#include <bicpl/objects.h>
#include <bicpl/images.h>

#include <ppm.h>



public  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    print_error( "input_rgb_file(): fix rgb_io_ppm.c.\n" );
}



public  Status  output_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    FILE* f;
    pixel* rowbuf;
    int x,y;

    if( !file_directory_exists( filename ) )
    {
        print_error( "Error: output file directory does not exist: %s\n",
                     filename );
        return( ERROR );
    }

    if ( (f = fopen(filename,"w")) == NULL ) {
        print_error( "Error: output file could not be opened for writing: %s\n",
                     filename );
        return( ERROR );
    }

    if ( pixels->pixel_type != RGB_PIXEL ) {
        print_error( "Error: only RGB_PIXEL images are handled\n" );
        return( ERROR );
    }

    /* I think some errors detected by the ppm_ routines
       are handled by PPM, which can be changed,
       and probably ought to be. */

    /* The last argument is set to 0 to allow "raw" mode,
       and the penultimate argument is the max. value for
       a red/green/blue value.  Note we are assuming RGB_PIXEL
       uses 8-bit values, which was true when I wrote this. */
    ppm_writeppminit( f, pixels->x_size, pixels->y_size, 255, 0 );


    if ( (rowbuf = ppm_allocrow( pixels->x_size ) ) == NULL ) {
        print_error( "Error: could not allocate memory for image\n" );
        return( ERROR );
    }

    /* The image appears to be scanned from left to right,
       and bottom to top, so we scan from the largest row index
       to the smallest. */

    for ( y = pixels->y_size - 1; y >= 0; --y ) {
	for( x = 0; x < pixels->x_size; ++x ) {
            Colour col = PIXEL_RGB_COLOUR( *pixels, x, y );
	    PPM_ASSIGN( rowbuf[x], 
			get_Colour_r( col ),
			get_Colour_g( col ),
			get_Colour_b( col ) );
	}

	ppm_writeppmrow( f, rowbuf, pixels->x_size, 255, 0 );
    }

    ppm_freerow( rowbuf );
    fclose( f );

    return( OK );
}

