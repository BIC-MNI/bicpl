#include  <def_mni.h>

private  void  render_byte_volume_colour_map_flat(
    int             x_size,
    int             y_size,
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
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
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
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
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned short  **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned char   **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    unsigned char   **volume_data1,
    Real            weights1[],
    int             x_offsets1[],
    int             y_offsets1[],
    unsigned short  **volume_data2,
    Real            weights2[],
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
    int             n_slices,
    void            **volume_data,
    Data_types      volume1_type,
    Real            weights1[],
    int             x_stride1,
    int             y_stride1,
    Real            x_offset1,
    Real            y_offset1,
    Real            x_delta1,
    Real            y_delta1,
    void            *second_volume_data,
    Data_types      volume2_type,
    Real            weights2[],
    int             x_stride2,
    int             y_stride2,
    Real            x_offset2,
    Real            y_offset2,
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
    int                  *x_offsets;
    int                  *y_offsets;
    int                  *second_x_offsets;
    int                  *second_y_offsets;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    ALLOC( x_offsets, x_size );
    ALLOC( y_offsets, y_size );

    if( second_volume_data != (void **) NULL )
    {
        ALLOC( second_x_offsets, x_size );
        ALLOC( second_y_offsets, y_size );
    }

    colour_map_mode = (pixels->pixel_type != RGB_PIXEL);

    for_less( x, 0, x_size )
    {
        x_voxel = map_viewport_to_slice_1d( (Real) x, x_offset1, x_delta1 );
        x_offsets[x] = x_stride1 * ROUND( x_voxel );

        if( second_volume_data != (void **) NULL )
        {
            x_voxel = map_viewport_to_slice_1d( (Real) x, x_offset2, x_delta2 );
            second_x_offsets[x] = x_stride2 * ROUND( x_voxel );
        }
    }

    for_less( y, 0, y_size )
    {
        y_voxel = map_viewport_to_slice_1d( (Real) y, y_offset1, y_delta1 );
        y_offsets[y] = y_stride1 * ROUND( y_voxel );

        if( second_volume_data != (void **) NULL )
        {
            y_voxel = map_viewport_to_slice_1d( (Real) y, y_offset2, y_delta2 );
            second_y_offsets[y] = y_stride2 * ROUND( y_voxel );
        }
    }

    if( second_volume_data != (void **) NULL )
    {
        if( colour_map_mode )
        {
            if( volume1_type == UNSIGNED_BYTE && volume2_type == UNSIGNED_BYTE )
            {
                render_byte_byte_volumes_colour_map_flat(
                          x_size, y_size, n_slices,
                          (unsigned char **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned char **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_SHORT &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_short_short_volumes_colour_map_flat(
                          x_size, y_size, n_slices,
                          (unsigned short **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned short **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_BYTE &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_byte_short_volumes_colour_map_flat(
                          x_size, y_size, n_slices,
                          (unsigned char **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned short **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
            else
            {
                render_short_byte_volumes_colour_map_flat(
                          x_size, y_size, n_slices,
                          (unsigned short **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned char **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          cmode_colour_map, pixels );
            }
        }
        else
        {
            if( volume1_type == UNSIGNED_BYTE && volume2_type == UNSIGNED_BYTE )
            {
                render_byte_byte_volumes_rgb_flat(
                          x_size, y_size, n_slices,
                          (unsigned char **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned char **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_SHORT &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_short_short_volumes_rgb_flat(
                          x_size, y_size, n_slices,
                          (unsigned short **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned short **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else if( volume1_type == UNSIGNED_BYTE &&
                     volume2_type == UNSIGNED_SHORT )
            {
                render_byte_short_volumes_rgb_flat(
                          x_size, y_size, n_slices,
                          (unsigned char **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned short **) second_volume_data, weights2,
                          second_x_offsets, second_y_offsets,
                          rgb_colour_map, pixels );
            }
            else
            {
                render_short_byte_volumes_rgb_flat(
                          x_size, y_size, n_slices,
                          (unsigned short **) volume_data, weights1,
                          x_offsets, y_offsets,
                          (unsigned char **) second_volume_data, weights2,
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
                render_byte_volume_colour_map_flat( x_size, y_size, n_slices,
                             (unsigned char **) volume_data, weights1,
                             x_offsets, y_offsets, *cmode_colour_map, pixels );
            else
                render_short_volume_colour_map_flat( x_size, y_size, n_slices,
                             (unsigned short **) volume_data, weights1,
                             x_offsets, y_offsets, *cmode_colour_map, pixels );
        }
        else
        {
            if( volume1_type == UNSIGNED_BYTE )
                render_byte_volume_rgb_flat( x_size, y_size, n_slices,
                             (unsigned char **) volume_data, weights1,
                             x_offsets, y_offsets, *rgb_colour_map, pixels );
            else
                render_short_volume_rgb_flat( x_size, y_size, n_slices,
                             (unsigned short **) volume_data, weights1,
                             x_offsets, y_offsets, *rgb_colour_map, pixels );
        }
    }

    FREE( x_offsets );
    FREE( y_offsets );

    if( second_volume_data != (void **) NULL )
    {
        FREE( second_x_offsets );
        FREE( second_y_offsets );
    }
}
