#ifndef  DEF_image_prototypes
#define  DEF_image_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  void  crop_pixels(
    pixels_struct  *in_pixels,
    VIO_Colour         background_colour,
    int            border,
    pixels_struct  *out_pixels );

BICAPI  VIO_Status  input_rgb_file(
    VIO_STR          filename,
    pixels_struct   *pixels );

BICAPI  VIO_Status  output_rgb_file(
    VIO_STR          filename,
    pixels_struct   *pixels );

#ifdef __cplusplus
}
#endif

#endif
