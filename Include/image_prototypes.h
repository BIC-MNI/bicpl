#ifndef  DEF_image_prototypes
#define  DEF_image_prototypes

public  void  crop_pixels(
    pixels_struct  *in_pixels,
    Colour         background_colour,
    int            border,
    pixels_struct  *out_pixels );

public  Status  input_rgb_file(
    STRING          filename,
    pixels_struct   *pixels );

public  Status  output_rgb_file(
    STRING          filename,
    pixels_struct   *pixels );
#endif
