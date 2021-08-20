#include <bicpl.h>
#include <arpa/inet.h>

#define TEST_IMAGE_CX 512
#define TEST_IMAGE_CY 512


int main( int ac, char* av[] )
{
    pixels_struct image;
    int i, j;

    VIO_Status       status;
    VIO_File_formats format;
    Object_types     object_type;
    VIO_BOOL         eof;
    FILE             *file;
    pixels_struct    *pixels = &image;

    if ( ac != 3 ) {
    	fprintf( stderr, "usage: %s in out\n", av[0] );
	    return 1;
    }



    status = open_file( av[1], READ_FILE, BINARY_FORMAT, &file );

    if( status == VIO_OK )
        status = input_object_type( file, &object_type, &format, &eof );
    
    if( status == VIO_OK && !eof && object_type == PIXELS )
        status = io_pixels( file, READ_FILE, format, pixels );
    else
        status = VIO_ERROR;

    if( status == VIO_OK && pixels->pixel_type != COLOUR_INDEX_8BIT_PIXEL )
        status = VIO_ERROR;

    if( status == VIO_OK )
        status = close_file( file );

    /*printf("Image: %d,%d\n",(int)ntohl(image.x_size),(int)ntohl(image.y_size));*/
    printf("Image: %d,%d\n",image.x_size,image.y_size);

    if (image.x_size != TEST_IMAGE_CX || image.y_size != TEST_IMAGE_CY) {
        fprintf(stderr, "test_pixel_io: incorrect image dimensions :%d,%d.\n",image.x_size,image.y_size);
        return 2;
    }

    if (image.pixel_type != COLOUR_INDEX_8BIT_PIXEL ) {
        fprintf(stderr, "test_pixel_io: incorrect pixel type.\n");
        return 2;
    }

    // if ( output_rgb_file( av[2], &image ) != VIO_OK ) {
    // 	fprintf( stderr, "output_rgb_file() failed.\n" );
	//     return 2;
    // }

    delete_pixels( &image );

    return 0;
}
