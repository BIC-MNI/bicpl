#include <volume_io/internal_volume_io.h>
#include <bicpl/images.h>


public  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    print_error( "input_rgb_file(): cannot read image files.\n" );
    print_error( "Recompile bicpl with an image library, then relink this application.\n" );
}



public  Status  output_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    print_error( "output_rgb_file(): cannot write image files.\n" );
    print_error( "Recompile bicpl with an image library, then relink this application.\n" );
}
