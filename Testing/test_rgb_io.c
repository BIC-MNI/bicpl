#include <bicpl.h>

#define TEST_IMAGE_CX 300
#define TEST_IMAGE_CY 300

#define TEST_IMAGE_RED 'L'
#define TEST_IMAGE_BLUE 'L'
#define TEST_IMAGE_GREEN 'L'

int main( int ac, char* av[] )
{
    pixels_struct image;
    int i, j;

    if ( ac != 3 ) {
	fprintf( stderr, "usage: %s in out\n", av[0] );
	return 1;
    }

    if ( input_rgb_file( av[1], &image ) != VIO_OK ) {
	fprintf( stderr, "input_rgb_file() failed.\n" );
	return 2;
    }

    if (image.x_size != TEST_IMAGE_CX || image.y_size != TEST_IMAGE_CY) {
        fprintf(stderr, "test_rgb_io: incorrect image dimensions.\n");
        return 2;
    }

    if (image.pixel_type != RGB_PIXEL) {
        fprintf(stderr, "test_rgb_io: incorrect pixel type.\n");
        return 2;
    }

    for (i = 0; i < TEST_IMAGE_CX; i++) {
      for (j = 0; j < TEST_IMAGE_CY; j++) {
        VIO_Colour c = PIXEL_RGB_COLOUR(image, i, j);
        if (get_Colour_r(c) != TEST_IMAGE_RED ||
            get_Colour_g(c) != TEST_IMAGE_GREEN ||
            get_Colour_b(c) != TEST_IMAGE_BLUE) {
          fprintf(stderr, "test_rgb_io: incorrect pixel value (%d,%d).\n", i, j);
          return 2;
        }
      }
    }

    if ( output_rgb_file( av[2], &image ) != VIO_OK ) {
	fprintf( stderr, "output_rgb_file() failed.\n" );
	return 2;
    }

    return 0;
}
