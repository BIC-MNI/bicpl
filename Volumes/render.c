#include  <def_mni.h>

private  void  render_byte_volume_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  cmode_colour_map[],
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#define  BYTE_DATA
#include              "render_include.c"
#undef  COLOUR_MAP
#undef  BYTE_DATA
}

private  void  render_short_volume_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  cmode_colour_map[],
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#include              "render_include.c"
#undef  COLOUR_MAP
}

private  void  render_byte_volume_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    Colour          rgb_colour_map[],
    pixels_struct   *pixels )
{
#define  BYTE_DATA
#include              "render_include.c"
#undef  BYTE_DATA
}

private  void  render_short_volume_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    Colour          rgb_colour_map[],
    pixels_struct   *pixels )
{
#include              "render_include.c"
}

/* --------------------- merge 2 volumes ---------------------- */

private  void  render_byte_byte_volumes_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    unsigned short  **cmode_colour_map,
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#define  TWO_VOLUMES
#define  BYTE_DATA
#define  BYTE_DATA2
#include              "render_include.c"
#undef  COLOUR_MAP
#undef  TWO_VOLUMES
#undef  BYTE_DATA
#undef  BYTE_DATA2
}

private  void  render_short_short_volumes_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    unsigned short  **cmode_colour_map,
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#define  TWO_VOLUMES
#include              "render_include.c"
#undef  COLOUR_MAP
#undef  TWO_VOLUMES
}

private  void  render_short_byte_volumes_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    unsigned short  **cmode_colour_map,
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#define  TWO_VOLUMES
#define  BYTE_DATA2
#include              "render_include.c"
#undef  COLOUR_MAP
#undef  TWO_VOLUMES
#undef  BYTE_DATA2
}

private  void  render_byte_short_volumes_colour_map_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    unsigned short  **cmode_colour_map,
    pixels_struct   *pixels )
{
#define  COLOUR_MAP
#define  TWO_VOLUMES
#define  BYTE_DATA
#include              "render_include.c"
#undef  COLOUR_MAP
#undef  TWO_VOLUMES
#undef  BYTE_DATA
}

/* ------------- merge 2 volumes rgb mode ----------------- */

private  void  render_byte_byte_volumes_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
#define  TWO_VOLUMES
#define  BYTE_DATA
#define  BYTE_DATA2
#include              "render_include.c"
#undef  TWO_VOLUMES
#undef  BYTE_DATA
#undef  BYTE_DATA2
}

private  void  render_short_short_volumes_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
#define  TWO_VOLUMES
#include              "render_include.c"
#undef  TWO_VOLUMES
}

private  void  render_short_byte_volumes_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned short  *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
#define  TWO_VOLUMES
#define  BYTE_DATA2
#include              "render_include.c"
#undef  TWO_VOLUMES
#undef  BYTE_DATA2
}

private  void  render_byte_short_volumes_rgb_flat(
    int             x_size,
    int             y_size,
    unsigned char   *volume_data1,
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  *volume_data2,
    int             x_offsets2[],
    int             y_offsets2[],
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
#define  TWO_VOLUMES
#define  BYTE_DATA
#include              "render_include.c"
#undef  TWO_VOLUMES
#undef  BYTE_DATA
}

public  void  render_volume_to_slice(
    void            *volume_data,
    Data_types      volume1_type,
    int             x_stride1,
    int             y_stride1,
    Real            x_start1,
    Real            y_start1,
    Real            x_delta1,
    Real            y_delta1,
    void            *second_volume_data,
    Data_types      volume2_type,
    int             x_stride2,
    int             y_stride2,
    Real            x_start2,
    Real            y_start2,
    Real            x_delta2,
    Real            y_delta2,
    Boolean         interpolation_flag,    /* ARGSUSED */
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    pixels_struct   *pixels )
{
    int                  x_size, y_size, x, y;
    Real                 x_voxel, y_voxel;
    Boolean              colour_map_mode;
    static  int          *x_offsets;
    static  int          *y_offsets;
    static  int          x_size_alloced = 0;
    static  int          y_size_alloced = 0;
    static  int          *second_x_offsets;
    static  int          *second_y_offsets;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    if( x_size > x_size_alloced )
    {
        SET_ARRAY_SIZE( x_offsets, x_size_alloced, x_size, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( second_x_offsets, x_size_alloced, x_size,
                        DEFAULT_CHUNK_SIZE );
        x_size_alloced = x_size;
    }

    if( y_size > y_size_alloced )
    {
        SET_ARRAY_SIZE( y_offsets, y_size_alloced, y_size, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( second_y_offsets, y_size_alloced, y_size,
                        DEFAULT_CHUNK_SIZE );
        y_size_alloced = y_size;
    }

    colour_map_mode = (pixels->pixel_type != RGB_PIXEL);

    for_less( x, 0, x_size )
    {
        x_voxel = x_start1 + ((Real) x + 0.5) / x_delta1;
        x_offsets[x] = x_stride1 * ROUND( x_voxel );

        if( second_volume_data != (void *) NULL )
        {
            x_voxel = x_start2 + ((Real) x + 0.5) / x_delta2;
            second_x_offsets[x] = x_stride2 * ROUND( x_voxel );
        }
    }

    for_less( y, 0, y_size )
    {
        y_voxel = y_start1 + ((Real) y + 0.5) / y_delta1;
        y_offsets[y] = y_stride1 * ROUND( y_voxel );

        if( second_volume_data != (void *) NULL )
        {
            y_voxel = y_start2 + ((Real) y + 0.5) / y_delta2;
            second_y_offsets[y] = y_stride2 * ROUND( y_voxel );
        }
    }

    if( second_volume_data != (void *) NULL )
    {
        if( colour_map_mode )
        {
            if( volume1_type == UNSIGNED_BYTE && volume2_type == UNSIGNED_BYTE )
            {
                render_byte_byte_volumes_colour_map_flat(
                          x_size, y_size, (unsigned char *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned char *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_SHORT &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_short_short_volumes_colour_map_flat(
                          x_size, y_size, (unsigned short *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned short *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_BYTE &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_byte_short_volumes_colour_map_flat(
                          x_size, y_size, (unsigned char *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned short *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else
            {
                render_short_byte_volumes_colour_map_flat(
                          x_size, y_size, (unsigned short *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned char *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
        }
        else
        {
            if( volume1_type == UNSIGNED_BYTE && volume2_type == UNSIGNED_BYTE )
            {
                render_byte_byte_volumes_rgb_flat(
                          x_size, y_size, (unsigned char *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned char *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_SHORT &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_short_short_volumes_rgb_flat(
                          x_size, y_size, (unsigned short *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned short *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_BYTE &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_byte_short_volumes_rgb_flat(
                          x_size, y_size, (unsigned char *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned short *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else
            {
                render_short_byte_volumes_rgb_flat(
                          x_size, y_size, (unsigned short *) volume_data,
                          x_offsets, y_offsets,
                          (unsigned char *) second_volume_data,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
        }
    }
    else
    {
        if( colour_map_mode )
        {
            if( volume1_type == UNSIGNED_BYTE )
                render_byte_volume_colour_map_flat( x_size, y_size,
                                        (unsigned char *) volume_data,
                                        x_offsets, y_offsets, *cmode_colour_map,
                                        pixels );
            else
                render_short_volume_colour_map_flat( x_size, y_size,
                                        (unsigned short *) volume_data,
                                        x_offsets, y_offsets, *cmode_colour_map,
                                        pixels );
        }
        else
        {
            if( volume1_type == UNSIGNED_BYTE )
                render_byte_volume_rgb_flat( x_size, y_size,
                                        (unsigned char *) volume_data,
                                        x_offsets, y_offsets,
                                        *rgb_colour_map, pixels );
            else
                render_short_volume_rgb_flat( x_size, y_size,
                                        (unsigned short *) volume_data,
                                        x_offsets, y_offsets,
                                        *rgb_colour_map, pixels );
        }
    }
}
