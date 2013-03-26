#include "bicpl_internal.h"

//using bundled libppm routine
#include "ppm/ppm.h"
#include "ppm/ppm.c"

BICAPI  VIO_Status  input_rgb_file(
    VIO_STR          filename,
    pixels_struct   *pixels )
{
  ppm_t ppm;
  ppm_err_t err;
  const ppm_pixel_t *px;
  int x,y;
  if ((err = ppm_load_file(&ppm, filename)) != PPM_ERR_OK)
  {
    if((ppm.bpp == PPM_BPP_24))
    {
      initialize_pixels( pixels, 0, 0,  ppm.width ,ppm.height, 1.0, 1.0, RGB_PIXEL );
      /* The image appears to be scanned from left to right,
        and bottom to top, so we scan from the largest row index
        to the smallest. */

      for ( y = ppm.height - 1; y >= 0; --y ) {
        //ppm_readppmrow( f, rowbuf, n_cols, max_pixval, format );
        for( x = 0; x < ppm.width; ++x ) {
          if ((err = ppm_pixel_const(&ppm, &px, x, y)) == PPM_ERR_OK)
            PIXEL_RGB_COLOUR( *pixels, x, y ) = make_rgba_Colour( px->b.rgb[0],
                  px->b.rgb[1],
                  px->b.rgb[2],
                  255 );
        }
      }
    } else {
      print_error( "Error: 48 bit images are not supported\n" );
      ppm_destroy(&ppm);
      return ERROR;
    }
    
    if ((err = ppm_destroy(&ppm)) == PPM_ERR_OK)
      return OK;
  }
  ppm_perror(err, filename);
  return( ERROR );
}



BICAPI  VIO_Status  output_rgb_file(
    VIO_STR          filename,
    pixels_struct   *pixels )
{
  int x,y;
    ppm_t ppm;
    ppm_err_t err;
    ppm_pixel_t *px;  
    
    if ( pixels->pixel_type != RGB_PIXEL ) {
        print_error( "Error: only RGB_PIXEL images are handled\n" );
        return( ERROR );
    }

    if( !file_directory_exists( filename ) )
    {
        print_error( "Error: output file directory does not exist: %s\n",
                     filename );
        return( ERROR );
    }
    
    if ((err = ppm_create(&ppm, pixels->x_size, pixels->y_size, PPM_BPP_24)) == PPM_ERR_OK)
    {
      
      /* The image appears to be scanned from left to right,
        and bottom to top, so we scan from the largest row index
        to the smallest. */

      for ( y = pixels->y_size - 1; y >= 0; --y ) {
          for( x = 0; x < pixels->x_size; ++x ) {
            if ((err = ppm_pixel(&ppm, &px, x, y)) == PPM_ERR_OK)
            {
              VIO_Colour col = PIXEL_RGB_COLOUR( *pixels, x, y );
              px->b.rgb[0] = get_Colour_r( col );
              px->b.rgb[1] = get_Colour_g( col );
              px->b.rgb[2] = get_Colour_b( col );
            }
        }
      }
      
      if((err = ppm_save_file(&ppm, filename))==PPM_ERR_OK)
      {
        ppm_destroy(&ppm);
        return OK;
      }
      ppm_destroy(&ppm);
    }
    ppm_perror(err, filename);
    return( ERROR );
}
