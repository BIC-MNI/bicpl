#include <bicpl/images.h>


int main( int ac, char* av[] )
{
    pixels_struct image;

    if ( ac != 3 ) {
	fprintf( stderr, "usage: %s in out\n", av[0] );
	return 1;
    }

    if ( input_rgb_file( av[1], &image ) != OK ) {
	fprintf( stderr, "input_rgb_file() failed.\n" );
	return 2;
    }

    if ( output_rgb_file( av[2], &image ) != OK ) {
	fprintf( stderr, "output_rgb_file() failed.\n" );
	return 2;
    }

    return 0;
}
