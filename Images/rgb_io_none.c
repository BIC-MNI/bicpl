#include "bicpl_internal.h"

BICAPI  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    print_error( "input_rgb_file(): cannot read image files.\n" );
    print_error( "Recompile bicpl with an image library, then relink this application.\n" );
}



BICAPI  Status  output_rgb_file(
    STRING          filename,
    pixels_struct   *pixels )
{
    print_error( "output_rgb_file(): cannot write image files.\n" );
    print_error( "Recompile bicpl with an image library, then relink this application.\n" );
}
