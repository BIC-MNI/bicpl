/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Volumes/render.c,v 1.46 2009-06-29 18:10:48 claude Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : clip
@INPUT      : left_edge
              right_edge
              origin
              delta
@OUTPUT     : t_min
              t_max
@RETURNS    :
@DESCRIPTION: Clips one row of a slice.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    :         1993    David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

static void  clip(
    VIO_Real  left_edge,
    VIO_Real  right_edge,
    VIO_Real  origin,
    VIO_Real  delta,
    VIO_Real  *t_min,
    VIO_Real  *t_max )
{
    VIO_Real  t;

    if( delta == 0.0 )
    {
        if( origin < left_edge || origin >= right_edge )
        {
            *t_min = 1.0;
            *t_max = 0.0;
        }
    }
    else if( delta > 0.0 )
    {
        t = (left_edge - origin) / delta;
        if( t > *t_min )
            *t_min = t;

        t = (right_edge - origin) / delta;
        if( t < *t_max )
            *t_max = t;
    }
    else
    {
        t = (left_edge - origin) / delta;
        if( t < *t_max )
            *t_max = t;

        t = (right_edge - origin) / delta;
        if( t > *t_min )
            *t_min = t;
    }
}

typedef  struct
{
    size_t  **x_offsets1;
    size_t  **y_offsets1;
    size_t  ***which_x_offsets1;
    void    **start_slices1;
    size_t  **row_offsets1;

    size_t  **x_offsets2;
    size_t  **y_offsets2;
    size_t  ***which_x_offsets2;
    void    **start_slices2;
    size_t  **row_offsets2;

    int     *start_x;
    int     *end_x;

    int     x_size1_alloced;
    int     y_size1_alloced;
    int     total_cases1_alloced;
    int     n_slices1_alloced;

    int     x_size2_alloced;
    int     y_size2_alloced;
    int     total_cases2_alloced;
    int     n_slices2_alloced;
}
render_storage_struct;

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_render_storage
@INPUT      :
@OUTPUT     :
@RETURNS    : pointer to storage
@DESCRIPTION: Creates a structure containing rendering arrays, so that it
              can be used for many slice renders, without allocating many times.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    :         1993    David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

BICAPI void   *initialize_render_storage( void )
{
    render_storage_struct  *store;
    void                   *void_ptr;

    ALLOC( store, 1 );

    store->x_size1_alloced = 0;
    store->y_size1_alloced = 0;
    store->total_cases1_alloced = 0;
    store->n_slices1_alloced = 0;

    store->x_size2_alloced = 0;
    store->y_size2_alloced = 0;
    store->total_cases2_alloced = 0;
    store->n_slices2_alloced = 0;

    void_ptr = (void *) store;

    return( void_ptr );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_render_storage
@INPUT      : ptr
@OUTPUT     :
@RETURNS    :
@DESCRIPTION: Deletes the render storage.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    :         1993    David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

BICAPI void   delete_render_storage(
    void  *ptr )
{
    render_storage_struct  *store;

    store = (render_storage_struct *) ptr;

    if( store->total_cases1_alloced > 0 && store->x_size1_alloced > 0 )
        VIO_FREE2D( store->x_offsets1 );

    if( store->n_slices1_alloced > 0 && store->y_size1_alloced > 0 )
    {
        VIO_FREE2D( store->y_offsets1 );
        VIO_FREE2D( store->which_x_offsets1 );
    }

    if( store->n_slices1_alloced > 0 )
    {
        FREE( store->start_slices1 );
        FREE( store->row_offsets1 );
    }

    if( store->total_cases2_alloced > 0 && store->x_size2_alloced > 0 )
        VIO_FREE2D( store->x_offsets2 );

    if( store->n_slices2_alloced > 0 && store->y_size2_alloced > 0 )
    {
        VIO_FREE2D( store->y_offsets2 );
        VIO_FREE2D( store->which_x_offsets2 );
    }

    if( store->n_slices2_alloced > 0 )
    {
        FREE( store->start_slices2 );
        FREE( store->row_offsets2 );
    }

    if( store->y_size1_alloced > 0 )
    {
        FREE( store->start_x );
        FREE( store->end_x );
    }

    FREE( store );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : render_volume_to_slice
@INPUT      :
@OUTPUT     :
@RETURNS    :
@DESCRIPTION: Renders a slice.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    :         1993    David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */
#define RENDER_FIXED 1
//#define RENDER_FLOAT 1

#if defined(RENDER_FIXED)

typedef int64_t fixed_t;

#define FRAC_BITS 32

#define FLOAT_TO_FIXED(x) rint((x) * (1L<<FRAC_BITS))
#define FIXED_TO_INT(x) (((x) + (1L<<(FRAC_BITS-1))) >> FRAC_BITS)

BICAPI void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    VIO_Data_types  volume_type1,
    int             n_slices1,
    VIO_Real        weights1[],
    int             strides1[],
    VIO_Real        **origins1,
    VIO_Real        x_axis1[],
    VIO_Real        y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    VIO_Data_types  volume_type2,
    int             n_slices2,
    VIO_Real        weights2[],
    int             strides2[],
    VIO_Real        **origins2,
    VIO_Real        x_axis2[],
    VIO_Real        y_axis2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    unsigned short  **cmode_colour_map,
    VIO_Colour      **rgb_colour_map,
    VIO_Colour      empty_colour,
    void            *render_storage,
    pixels_struct   *pixels )
{
  int x, y;
  int i;
  VIO_Colour *single_rgb_map = rgb_colour_map[0];
  fixed_t row_origin[VIO_MAX_DIMENSIONS];
  fixed_t cur_origin[VIO_MAX_DIMENSIONS];
  fixed_t x_axis_f[VIO_N_DIMENSIONS];
  fixed_t y_axis_f[VIO_N_DIMENSIONS];
  size_t fixed_offset = 0;
  VIO_Colour *pixel_ptr = &pixels->data.pixels_rgb[0];

  if (volume_data2 != NULL)
  {
    print_error("Can't handle two volumes yet.\n");
  }
  if (pixels->pixel_type != RGB_PIXEL)
  {
    print_error("Only supports RGB pixels.\n");
    return;
  }

  for (i = VIO_N_DIMENSIONS; i < n_dims1; i++)
  {
    fixed_offset += rint(origins1[0][i] * strides1[i]);
  }

  for (i = 0; i < VIO_N_DIMENSIONS; i++)
  {
    x_axis_f[i] = FLOAT_TO_FIXED(x_axis1[i]);
    y_axis_f[i] = FLOAT_TO_FIXED(y_axis1[i]);
  }

#if 0
  printf("x_axis_f %f %f %f %lx %lx %lx\n", x_axis1[0], x_axis1[1], x_axis1[2], x_axis_f[0], x_axis_f[1], x_axis_f[2]);
  printf("y_axis_f %f %f %f %lx %lx %lx\n", y_axis1[0], y_axis1[1], y_axis1[2], y_axis_f[0], y_axis_f[1], y_axis_f[2]);
#endif

  for (i = 0; i < VIO_N_DIMENSIONS; i++)
  {
    row_origin[i] = FLOAT_TO_FIXED(origins1[0][i] + (x_pixel_start * x_axis1[i] +
                                                     y_pixel_start * y_axis1[i]));
  }

  if (n_slices1 == 1)
  {
    for (y = y_pixel_start; y <= y_pixel_end; y++)
    {
      cur_origin[0] = row_origin[0];
      cur_origin[1] = row_origin[1];
      cur_origin[2] = row_origin[2];

      for (x = x_pixel_start; x <= x_pixel_end; x++)
      {
        VIO_Colour colour;
        int int_origin[VIO_MAX_DIMENSIONS];

        int_origin[0] = FIXED_TO_INT(cur_origin[0]);
        int_origin[1] = FIXED_TO_INT(cur_origin[1]);
        int_origin[2] = FIXED_TO_INT(cur_origin[2]);

        if (int_origin[0] < 0 || int_origin[0] >= sizes1[0] ||
            int_origin[1] < 0 || int_origin[1] >= sizes1[1] ||
            int_origin[2] < 0 || int_origin[2] >= sizes1[2])
        {
          colour = empty_colour;
        }
        else
        {
          double value;
          size_t voxel_offset = fixed_offset;

          voxel_offset += ((size_t)int_origin[0] * strides1[0] +
                           (size_t)int_origin[1] * strides1[1] +
                           (size_t)int_origin[2] * strides1[2]);

          switch (volume_type1)
          {
          case VIO_UNSIGNED_BYTE:
            value = ((unsigned char *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_BYTE:
            value = ((signed char *)volume_data1)[voxel_offset];
            break;
          case VIO_UNSIGNED_SHORT:
            value = ((unsigned short *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_SHORT:
            value = ((signed short *)volume_data1)[voxel_offset];
            break;
          case VIO_UNSIGNED_INT:
            value = ((unsigned int *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_INT:
            value = ((signed int *)volume_data1)[voxel_offset];
            break;
          case VIO_FLOAT:
            value = ((float *)volume_data1)[voxel_offset];
            break;
          case VIO_DOUBLE:
            value = ((double *)volume_data1)[voxel_offset];
            break;
          default:
            print_error("Unknown voxel type: %d", volume_type1);
            exit(-1);
          }
          colour = single_rgb_map[(int)rint(value)];
        }

        *pixel_ptr++ = colour;

        cur_origin[0] += x_axis_f[0];
        cur_origin[1] += x_axis_f[1];
        cur_origin[2] += x_axis_f[2];
      }
      row_origin[0] += y_axis_f[0];
      row_origin[1] += y_axis_f[1];
      row_origin[2] += y_axis_f[2];
    }
  }
  else
  {
    int s;
    fixed_t col_origin[VIO_MAX_DIMENSIONS];
    fixed_t slc_delta[VIO_MAX_DIMENSIONS];

    for (i = 0; i < VIO_N_DIMENSIONS; i++)
    {
      slc_delta[i] = FLOAT_TO_FIXED(origins1[1][i]) - FLOAT_TO_FIXED(origins1[0][i]);
    }

    for (y = y_pixel_start; y <= y_pixel_end; y++)
    {
      col_origin[0] = row_origin[0];
      col_origin[1] = row_origin[1];
      col_origin[2] = row_origin[2];

      for (x = x_pixel_start; x <= x_pixel_end; x++)
      {
        VIO_Colour colour;
        VIO_Real combined_value = 0.0;

        cur_origin[0] = col_origin[0];
        cur_origin[1] = col_origin[1];
        cur_origin[2] = col_origin[2];

        for (s = 0; s < n_slices1; s++)
        {
          int int_origin[VIO_MAX_DIMENSIONS];

          int_origin[0] = FIXED_TO_INT(cur_origin[0]);
          int_origin[1] = FIXED_TO_INT(cur_origin[1]);
          int_origin[2] = FIXED_TO_INT(cur_origin[2]);

          if (int_origin[0] < 0 || int_origin[0] >= sizes1[0] ||
              int_origin[1] < 0 || int_origin[1] >= sizes1[1] ||
              int_origin[2] < 0 || int_origin[2] >= sizes1[2])
          {
            break;
          }
          else
          {
            double value;
            size_t voxel_offset = fixed_offset;

            voxel_offset += ((size_t)int_origin[0] * strides1[0] +
                             (size_t)int_origin[1] * strides1[1] +
                             (size_t)int_origin[2] * strides1[2]);

            switch (volume_type1)
            {
            case VIO_UNSIGNED_BYTE:
              value = ((unsigned char *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_BYTE:
              value = ((signed char *)volume_data1)[voxel_offset];
              break;
            case VIO_UNSIGNED_SHORT:
              value = ((unsigned short *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_SHORT:
              value = ((signed short *)volume_data1)[voxel_offset];
              break;
            case VIO_UNSIGNED_INT:
              value = ((unsigned int *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_INT:
              value = ((signed int *)volume_data1)[voxel_offset];
              break;
            case VIO_FLOAT:
              value = ((float *)volume_data1)[voxel_offset];
              break;
            case VIO_DOUBLE:
              value = ((double *)volume_data1)[voxel_offset];
              break;
            default:
              print_error("Unknown voxel type: %d", volume_type1);
              exit(-1);
            }
            combined_value += weights1[s] * value;
          }
          cur_origin[0] += slc_delta[0];
          cur_origin[1] += slc_delta[1];
          cur_origin[2] += slc_delta[2];
        }

        if (s < n_slices1)
        {
          colour = empty_colour;
        }
        else
        {
          int ivalue = lrint(combined_value);
          colour = single_rgb_map[ivalue];
        }

        *pixel_ptr++ = colour;

        col_origin[0] += x_axis_f[0];
        col_origin[1] += x_axis_f[1];
        col_origin[2] += x_axis_f[2];
      }
      row_origin[0] += y_axis_f[0];
      row_origin[1] += y_axis_f[1];
      row_origin[2] += y_axis_f[2];
    }
  }
}
#elif defined(RENDER_FLOAT)
BICAPI void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    VIO_Data_types  volume_type1,
    int             n_slices1,
    VIO_Real        weights1[],
    int             strides1[],
    VIO_Real        **origins1,
    VIO_Real        x_axis1[],
    VIO_Real        y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    VIO_Data_types  volume_type2,
    int             n_slices2,
    VIO_Real        weights2[],
    int             strides2[],
    VIO_Real        **origins2,
    VIO_Real        x_axis2[],
    VIO_Real        y_axis2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    unsigned short  **cmode_colour_map,
    VIO_Colour      **rgb_colour_map,
    VIO_Colour      empty_colour,
    void            *render_storage,
    pixels_struct   *pixels )
{
  int x, y;
  int i;
  VIO_Colour *single_rgb_map = rgb_colour_map[0];
  double row_origin[VIO_N_DIMENSIONS];
  double cur_origin[VIO_N_DIMENSIONS];
  size_t fixed_offset = 0;
  VIO_Colour *pixel_ptr = &pixels->data.pixels_rgb[0];

  if (volume_data2 != NULL)
  {
    print_error("Can't handle two volumes yet.\n");
  }
  if (pixels->pixel_type != RGB_PIXEL)
  {
    print_error("Only supports RGB pixels.\n");
    return;
  }

  for (i = VIO_N_DIMENSIONS; i < n_dims1; i++)
  {
    fixed_offset += lrint(origins1[0][i] * strides1[i]);
  }

  for (i = 0; i < n_dims1; i++)
  {
    row_origin[i] = origins1[0][i] + (x_pixel_start * x_axis1[i] +
                                      y_pixel_start * y_axis1[i]);
  }

  if (n_slices1 == 1)
  {
    for (y = y_pixel_start; y <= y_pixel_end; y++)
    {
      cur_origin[0] = row_origin[0];
      cur_origin[1] = row_origin[1];
      cur_origin[2] = row_origin[2];

      for (x = x_pixel_start; x <= x_pixel_end; x++)
      {
        VIO_Colour colour;
        int int_origin[VIO_MAX_DIMENSIONS];

        int_origin[0] = lrint(cur_origin[0]);
        int_origin[1] = lrint(cur_origin[1]);
        int_origin[2] = lrint(cur_origin[2]);

        if (int_origin[0] < 0 || int_origin[0] >= sizes1[0] ||
            int_origin[1] < 0 || int_origin[1] >= sizes1[1] ||
            int_origin[2] < 0 || int_origin[2] >= sizes1[2])
        {
          colour = empty_colour;
        }
        else {
          double value;
          int ivalue;
          size_t voxel_offset = fixed_offset;

          voxel_offset = (int_origin[0] * strides1[0] +
                          int_origin[1] * strides1[1] +
                          int_origin[2] * strides1[2]);
          switch (volume_type1)
          {
          case VIO_UNSIGNED_BYTE:
            value = ((unsigned char *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_BYTE:
            value = ((signed char *)volume_data1)[voxel_offset];
            break;
          case VIO_UNSIGNED_SHORT:
            value = ((unsigned short *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_SHORT:
            value = ((signed short *)volume_data1)[voxel_offset];
            break;
          case VIO_UNSIGNED_INT:
            value = ((unsigned int *)volume_data1)[voxel_offset];
            break;
          case VIO_SIGNED_INT:
            value = ((signed int *)volume_data1)[voxel_offset];
            break;
          case VIO_FLOAT:
            value = ((float *)volume_data1)[voxel_offset];
            break;
          case VIO_DOUBLE:
            value = ((double *)volume_data1)[voxel_offset];
            break;
          default:
            print_error("Unknown voxel type: %d", volume_type1);
            exit(-1);
          }
          ivalue = lrint(value);
          colour = single_rgb_map[ivalue];
        }
        *pixel_ptr++ = colour;

        cur_origin[0] += x_axis1[0];
        cur_origin[1] += x_axis1[1];
        cur_origin[2] += x_axis1[2];
      }
      row_origin[0] += y_axis1[0];
      row_origin[1] += y_axis1[1];
      row_origin[2] += y_axis1[2];
    }
  }
  else
  {
    int s;
    double col_origin[VIO_MAX_DIMENSIONS];
    double slc_delta[VIO_MAX_DIMENSIONS];

    for (i = 0; i < n_dims1; i++)
    {
      slc_delta[i] = origins1[1][i] - origins1[0][i];
    }

    for (i = 0; i < n_dims1; i++)
    {
      for (s = 1; s < n_slices1; s++)
      {
        VIO_Real diff = slc_delta[i] - (origins1[s][i] - origins1[s-1][i]);
        if (fabs(diff) > 1e-06)
        {
          print_error("Can't handle these slice origins! %d %g %g %g\n",
                      i, slc_delta[i], origins1[s][i] - origins1[s-1][i], diff);
          return;
        }
      }
    }

    for (y = y_pixel_start; y <= y_pixel_end; y++)
    {
      col_origin[0] = row_origin[0];
      col_origin[1] = row_origin[1];
      col_origin[2] = row_origin[2];

      for (x = x_pixel_start; x <= x_pixel_end; x++)
      {
        VIO_Colour colour;
        double combined_value = 0.0;

        cur_origin[0] = col_origin[0];
        cur_origin[1] = col_origin[1];
        cur_origin[2] = col_origin[2];

        for (s = 0; s < n_slices1; s++)
        {
          int int_origin[VIO_MAX_DIMENSIONS];

          int_origin[0] = lrint(cur_origin[0]);
          int_origin[1] = lrint(cur_origin[1]);
          int_origin[2] = lrint(cur_origin[2]);

          if (int_origin[0] < 0 || int_origin[0] >= sizes1[0] ||
              int_origin[1] < 0 || int_origin[1] >= sizes1[1] ||
              int_origin[2] < 0 || int_origin[2] >= sizes1[2])
          {
            break;
          }
          else
          {
            double value;
            size_t voxel_offset = fixed_offset;

            voxel_offset += (int_origin[0] * strides1[0] +
                             int_origin[1] * strides1[1] +
                             int_origin[2] * strides1[2]);

            switch (volume_type1)
            {
            case VIO_UNSIGNED_BYTE:
              value = ((unsigned char *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_BYTE:
              value = ((signed char *)volume_data1)[voxel_offset];
              break;
            case VIO_UNSIGNED_SHORT:
              value = ((unsigned short *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_SHORT:
              value = ((signed short *)volume_data1)[voxel_offset];
              break;
            case VIO_UNSIGNED_INT:
              value = ((unsigned int *)volume_data1)[voxel_offset];
              break;
            case VIO_SIGNED_INT:
              value = ((signed int *)volume_data1)[voxel_offset];
              break;
            case VIO_FLOAT:
              value = ((float *)volume_data1)[voxel_offset];
              break;
            case VIO_DOUBLE:
              value = ((double *)volume_data1)[voxel_offset];
              break;
            default:
              print_error("Unknown voxel type: %d", volume_type1);
              exit(-1);
            }
            combined_value += weights1[s] * value;
          }

          cur_origin[0] += slc_delta[0];
          cur_origin[1] += slc_delta[1];
          cur_origin[2] += slc_delta[2];
        }

        if (s < n_slices1)
        {
          colour = empty_colour;
        }
        else
        {
          int ivalue = lrint(combined_value);
          colour = single_rgb_map[ivalue];
        }

        *pixel_ptr++ = colour;

        col_origin[0] += x_axis1[0];
        col_origin[1] += x_axis1[1];
        col_origin[2] += x_axis1[2];
      }
      row_origin[0] += y_axis1[0];
      row_origin[1] += y_axis1[1];
      row_origin[2] += y_axis1[2];
    }
  }
}
#else
BICAPI void  render_volume_to_slice(
    int             n_dims1,
    int             sizes1[],
    void            *volume_data1,
    VIO_Data_types      volume1_type,
    int             n_slices1,
    VIO_Real            weights1[],
    int             strides1[],
    VIO_Real            **origins1,
    VIO_Real            x_axis1[],
    VIO_Real            y_axis1[],
    int             n_dims2,
    int             sizes2[],
    void            *volume_data2,
    VIO_Data_types      volume2_type,
    int             n_slices2,
    VIO_Real            weights2[],
    int             strides2[],
    VIO_Real            **origins2,
    VIO_Real            x_axis2[],
    VIO_Real            y_axis2[],
    int             x_pixel_start,
    int             x_pixel_end,
    int             y_pixel_start,
    int             y_pixel_end,
    unsigned short  **cmode_colour_map,
    VIO_Colour          **rgb_colour_map,
    VIO_Colour          empty_colour,
    void            *render_storage,
    pixels_struct   *pixels )
{
    int     i, c, p, total_cases1, total_cases2, case_index, case_multiplier;
    int     int_start;
    int     *start_x, *end_x;
    int     s, x, y, n_cases1[VIO_MAX_DIMENSIONS], n_cases2[VIO_MAX_DIMENSIONS];
    size_t  offset;
    size_t  **x_offsets1, **y_offsets1;
    size_t  **x_offsets2, **y_offsets2;
    size_t  ***which_x_offsets1, ***which_x_offsets2;
    size_t  **row_offsets1, **row_offsets2;
    int     remainder_case, x_size, y_size;
    int     x_left, x_right, n_non_zero;
    void    **start_slices1, **start_slices2;
    VIO_Real    start_c, x_start, x_end, remainder, tmp_origin[VIO_MAX_DIMENSIONS];
    VIO_Real    remainder_offset, left_edge, right_edge, delta;
    render_storage_struct  *store;
    static  int   max_cases[VIO_MAX_DIMENSIONS] = { 10, 10, 4, 3, 3 };
    int     new_total_cases1, new_x_size1, new_y_size1, new_n_slices1;
    int     new_total_cases2, new_x_size2, new_y_size2, new_n_slices2;

    if( render_storage == NULL )
        store = (render_storage_struct *) initialize_render_storage();
    else
        store = (render_storage_struct *) render_storage;

    x_size = pixels->x_size;
    y_size = pixels->y_size;

    n_non_zero = 0;
    for_less( c, 0, n_dims1 )
    {
        if( y_axis1[c] != 0.0 )
            ++n_non_zero;
    }

    /* Set up the n_cases1 array. This appears to be more-or-less the
     * following:
     * n_cases1[c] = min(max(1, 1 / n_cases1[c]), max_cases[n_dims1-1]);
     */
    total_cases1 = 1;
    for_less( c, 0, n_dims1 )
    {
        delta = VIO_FABS( y_axis1[c] );
        if( delta == 0.0 || n_non_zero == 1 )
            n_cases1[c] = 1;
        else if( delta <= 1.0 / (VIO_Real) max_cases[n_dims1-1] )
            n_cases1[c] = max_cases[n_dims1-1];
        else
        {
            n_cases1[c] = (int) (1.0 / delta);
            if( n_cases1[c] < 1 )
                n_cases1[c] = 1;
            else if( n_cases1[c] > max_cases[n_dims1-1] )
                n_cases1[c] = max_cases[n_dims1-1];
        }

        total_cases1 *= n_cases1[c];
    }

    new_total_cases1 = MAX( total_cases1, store->total_cases1_alloced );
    new_x_size1 = MAX( x_size, store->x_size1_alloced );
    new_y_size1 = MAX( y_size, store->y_size1_alloced );
    new_n_slices1 = MAX( n_slices1, store->n_slices1_alloced );

    if( new_total_cases1 > store->total_cases1_alloced ||
        new_x_size1 > store->x_size1_alloced )
    {
        if( store->total_cases1_alloced > 0 && store->x_size1_alloced > 0 )
            VIO_FREE2D( store->x_offsets1 );
        VIO_ALLOC2D( store->x_offsets1, new_total_cases1, new_x_size1 );
    }

    if( new_n_slices1 > store->n_slices1_alloced ||
        new_y_size1 > store->y_size1_alloced )
    {
        if( store->n_slices1_alloced > 0 && store->y_size1_alloced > 0 )
        {
            VIO_FREE2D( store->y_offsets1 );
            VIO_FREE2D( store->which_x_offsets1 );
        }
        VIO_ALLOC2D( store->y_offsets1, new_n_slices1, new_y_size1 );
        VIO_ALLOC2D( store->which_x_offsets1, new_n_slices1, new_y_size1 );
    }


    if( new_n_slices1 > store->n_slices1_alloced )
    {
        SET_ARRAY_SIZE( store->start_slices1, store->n_slices1_alloced,
                        new_n_slices1, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( store->row_offsets1, store->n_slices1_alloced,
                        new_n_slices1, DEFAULT_CHUNK_SIZE );
    }

    if( new_y_size1 > store->y_size1_alloced )
    {
        SET_ARRAY_SIZE( store->start_x, store->y_size1_alloced,
                        new_y_size1, DEFAULT_CHUNK_SIZE );
        SET_ARRAY_SIZE( store->end_x, store->y_size1_alloced,
                        new_y_size1, DEFAULT_CHUNK_SIZE );
    }

    store->total_cases1_alloced = new_total_cases1;
    store->n_slices1_alloced = new_n_slices1;
    store->x_size1_alloced = new_x_size1;
    store->y_size1_alloced = new_y_size1;

    x_offsets1 = store->x_offsets1;
    y_offsets1 = store->y_offsets1;
    which_x_offsets1 = store->which_x_offsets1;
    start_slices1 = store->start_slices1;

    start_x = store->start_x;
    end_x = store->end_x;
    row_offsets1 = store->row_offsets1;

    if( volume_data2 != (void *) NULL )
    {
        n_non_zero = 0;
        for_less( c, 0, n_dims1 )
        {
            if( y_axis1[c] != 0.0 )
                ++n_non_zero;
        }

        total_cases2 = 1;
        for_less( c, 0, n_dims2 )
        {
            delta = VIO_FABS( y_axis2[c] );
            if( delta == 0.0 || n_non_zero == 1 )
                n_cases2[c] = 1;
            else if( delta <= 1.0 / (VIO_Real) max_cases[n_dims2-1] )
                n_cases2[c] = max_cases[n_dims2-1];
            else
            {
                n_cases2[c] = (int) (1.0 / delta);
                if( n_cases2[c] < 1 )
                    n_cases2[c] = 1;
                else if( n_cases2[c] > max_cases[n_dims2-1] )
                    n_cases2[c] = max_cases[n_dims2-1];
            }

            total_cases2 *= n_cases2[c];
        }

        new_total_cases2 = MAX( total_cases2, store->total_cases2_alloced );
        new_x_size2 = MAX( x_size, store->x_size2_alloced );
        new_y_size2 = MAX( y_size, store->y_size2_alloced );
        new_n_slices2 = MAX( n_slices2, store->n_slices2_alloced );

        if( new_total_cases2 > store->total_cases2_alloced ||
            new_x_size2 > store->x_size2_alloced )
        {
            if( store->total_cases2_alloced > 0 && store->x_size2_alloced > 0 )
                VIO_FREE2D( store->x_offsets2 );
            VIO_ALLOC2D( store->x_offsets2, new_total_cases2, new_x_size2 );
        }

        if( new_n_slices2 > store->n_slices2_alloced ||
            new_y_size2 > store->y_size2_alloced )
        {
            if( store->n_slices2_alloced > 0 && store->y_size2_alloced > 0 )
            {
                VIO_FREE2D( store->y_offsets2 );
                VIO_FREE2D( store->which_x_offsets2 );
            }
            VIO_ALLOC2D( store->y_offsets2, new_n_slices2, new_y_size2 );
            VIO_ALLOC2D( store->which_x_offsets2, new_n_slices2, new_y_size2 );
        }

        if( new_n_slices2 > store->n_slices2_alloced )
        {
            SET_ARRAY_SIZE( store->start_slices2, store->n_slices2_alloced,
                            new_n_slices2, DEFAULT_CHUNK_SIZE );
            SET_ARRAY_SIZE( store->row_offsets2, store->n_slices2_alloced,
                            new_n_slices2, DEFAULT_CHUNK_SIZE );
        }

        store->total_cases2_alloced = new_total_cases2;
        store->n_slices2_alloced = new_n_slices2;
        store->x_size2_alloced = new_x_size2;
        store->y_size2_alloced = new_y_size2;

        x_offsets2 = store->x_offsets2;
        y_offsets2 = store->y_offsets2;
        which_x_offsets2 = store->which_x_offsets2;
        start_slices2 = store->start_slices2;
        row_offsets2 = store->row_offsets2;
    }

    /* Now we go through all of the cases, figuring out what to do with
       them. We store "total_cases1" separate sets of values for the
       "x_offsets1" structure, which correspond to the different ways
       we might step through the voxel array.
    */
    for_less( i, 0, total_cases1 )
    {
        p = i;
        for_less( c, 0, n_dims1 )
        {
            case_index = p % n_cases1[c];
            if( y_axis1[c] == 0.0 && n_slices1 == n_cases1[c] )
                tmp_origin[c] = VIO_FRACTION( origins1[case_index][c] + 0.5 );
            else
                tmp_origin[c] = ((VIO_Real) case_index + 0.5) / (VIO_Real) n_cases1[c];
            p /= n_cases1[c];
        }


        for_inclusive( x, x_pixel_start, x_pixel_end )
        {
            offset = 0;
            for_less( c, 0, n_dims1 )
            {
                start_c = tmp_origin[c] + MIN( (VIO_Real) x * x_axis1[c], (VIO_Real)sizes1[c]-1 );
                offset += (size_t)strides1[c] * (size_t)VIO_FLOOR( start_c );
            }
            x_offsets1[i][x] = offset;
        }
    }

    if( volume_data2 != (void *) NULL )
    {
        for_less( i, 0, total_cases2 )
        {
            p = i;
            for_less( c, 0, n_dims2 )
            {
                case_index = p % n_cases2[c];
                if( y_axis2[c] == 0.0 && n_slices2 == n_cases2[c] )
                    tmp_origin[c] = VIO_FRACTION( origins2[case_index][c] + 0.5 );
                else
                    tmp_origin[c] = ((VIO_Real) case_index + 0.5) /
                                    (VIO_Real) n_cases2[c];
                p /= n_cases2[c];
            }

            for_inclusive( x, x_pixel_start, x_pixel_end )
            {
                offset = 0;
                for_less( c, 0, n_dims2 )
                {
                    start_c = tmp_origin[c] + MIN( (VIO_Real) x * x_axis2[c], (VIO_Real)sizes2[c]-1 );
                    offset += (size_t)strides2[c] * (size_t)VIO_FLOOR( start_c );
                }
                x_offsets2[i][x] = offset;
            }
        }
    }

    for_inclusive( y, y_pixel_start, y_pixel_end )
    {
        x_start = 0.0;
        x_end = (VIO_Real) (x_size - 1);

        /* For each of the slices we intend to draw, we need to figure out
         * which of the x_offsets1 arrays we will use, AND we need to
         * calculate the y_offsets1 values.
         */
        for_less( s, 0, n_slices1 )
        {
            offset = 0;
            case_index = 0;
            case_multiplier = 1;
            for_less( c, 0, n_dims1 )
            {
                start_c = origins1[s][c] + (VIO_Real) y * y_axis1[c] + 0.5;
                int_start = VIO_FLOOR( start_c );

                if( y_axis1[c] == 0.0 && n_slices1 == n_cases1[c] )
                {
                    remainder_case = s;
                    remainder_offset = 0.0;
                }
                else
                {
                    remainder = start_c - (VIO_Real) int_start;
                    remainder_case = (int) (remainder * (VIO_Real) n_cases1[c]);
                    remainder_offset = remainder -
                           ((VIO_Real) remainder_case + 0.5)/ (VIO_Real) n_cases1[c];
                }

                case_index += case_multiplier * remainder_case;
                case_multiplier *= n_cases1[c];
                offset += (size_t)strides1[c] * (size_t)int_start;

                left_edge = 0.0;
                right_edge = (VIO_Real) sizes1[c];

                if( remainder_offset < 0.0 )
                    right_edge += remainder_offset;
                else
                    left_edge += remainder_offset;

                clip( left_edge, right_edge, start_c, x_axis1[c],
                      &x_start, &x_end );
            }
            y_offsets1[s][y] = offset;
            which_x_offsets1[s][y] = x_offsets1[case_index];
        }

        if( volume_data2 != (void *) NULL )
        {
            for_less( s, 0, n_slices2 )
            {
                offset = 0;
                case_index = 0;
                case_multiplier = 1;
                for_less( c, 0, n_dims2 )
                {
                    start_c = origins2[s][c] + (VIO_Real) y * y_axis2[c] + 0.5;
                    int_start = VIO_FLOOR( start_c );

                    if( y_axis2[c] == 0.0 && n_slices2 == n_cases2[c] )
                    {
                        remainder_case = s;
                        remainder_offset = 0.0;
                    }
                    else
                    {
                        remainder = start_c - (VIO_Real) int_start;
                        remainder_case = (int) (remainder * (VIO_Real) n_cases2[c]);
                        remainder_offset = remainder -
                             ((VIO_Real) remainder_case + 0.5)/ (VIO_Real) n_cases2[c];
                    }

                    case_index += case_multiplier * remainder_case;
                    case_multiplier *= n_cases2[c];
                    offset += (size_t)strides2[c] * (size_t)int_start;

                    left_edge = 0.0;
                    right_edge = (VIO_Real) sizes2[c];

                    if( remainder_offset < 0.0 )
                        right_edge += remainder_offset;
                    else
                        left_edge += remainder_offset;

                    clip( left_edge, right_edge, start_c, x_axis2[c],
                          &x_start, &x_end );
                }

                y_offsets2[s][y] = offset;
                which_x_offsets2[s][y] = x_offsets2[case_index];
            }
        }

        start_x[y] = VIO_CEILING( x_start );
        end_x[y] = VIO_FLOOR( x_end );

        if( start_x[y] > x_size )
            start_x[y] = x_size;
        if( end_x[y] < 0 )
            end_x[y] = 0;
        if( start_x[y] > end_x[y] + 1 )
            start_x[y] = end_x[y] + 1;
    }

    for_inclusive( y, y_pixel_start, y_pixel_end )
    {
        for_less( s, 0, n_slices1 )
            row_offsets1[s] = which_x_offsets1[s][y];

        if( volume_data2 != (void *) NULL )
        {
            for_less( s, 0, n_slices2 )
                row_offsets2[s] = which_x_offsets2[s][y];
        }

        x_left = MAX( x_pixel_start, start_x[y] );
        x_right = MIN( x_pixel_end, end_x[y] );

#if 0
        /* THIS SHOULD HAVE BEEN FIXED BY NOW. (CLAUDE, June 2009) */
        /* there is a core-dumping bug somewhere */
        /* this is a temporary fix for register */
        /* The problem arises from bogus offsets, so test them (P.N.)*/

        size_t max_offset = 1;
        for_less( c, 0, n_dims1 ) {
           max_offset *= sizes1[c];
        }

        for_less( s, 0, n_slices1 ) {
           offset = y_offsets1[s][y] + row_offsets1[s][x_left];
           if ((offset < 0) || (offset >= max_offset)) {
              x_left++;
           }
           offset = y_offsets1[s][y] + row_offsets1[s][x_right];
           if ((offset < 0) || (offset >= max_offset)) {
              x_right--;
           }
        }
        if (volume_data2 != NULL) {
           max_offset = 1;
           for_less( c, 0, n_dims2 ) {
              max_offset *= sizes2[c];
           }
           for_less( s, 0, n_slices2 ) {
              offset = y_offsets2[s][y] + row_offsets2[s][x_left];
              if ((offset < 0) || (offset >= max_offset))
                 x_left++;
              offset = y_offsets2[s][y] + row_offsets2[s][x_right];
              if ((offset < 0) || (offset >= max_offset))
                 x_right--;
           }
        }
#endif

        if( x_left <= x_right )
        {
            render_one_row( volume_data1, volume1_type,
                            y, x_left, x_right,
                            y_offsets1, row_offsets1, start_slices1,
                            n_slices1, weights1,
                            volume_data2, volume2_type,
                            y_offsets2, row_offsets2, start_slices2,
                            n_slices2, weights2,
                            cmode_colour_map,
                            rgb_colour_map,
                            pixels );
        }

        if( pixels->pixel_type == RGB_PIXEL )
        {
            VIO_Colour          *pixel_ptr;

            pixel_ptr = &pixels->data.pixels_rgb[VIO_IJ(y,x_pixel_start,x_size)];

            for_less( x, x_pixel_start, start_x[y] )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }

            pixel_ptr = &pixels->data.pixels_rgb[VIO_IJ(y,end_x[y]+1,x_size)];
            for_less( x, end_x[y]+1, x_pixel_end+1 )
            {
                *pixel_ptr = empty_colour;
                ++pixel_ptr;
            }
        }
        else
        {
            unsigned short          *pixel_ptr;

            pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                              [VIO_IJ(y,x_pixel_start,x_size)];
            for_less( x, x_pixel_start, start_x[y] )
            {
                *pixel_ptr = (unsigned short) empty_colour;
                ++pixel_ptr;
            }

            pixel_ptr = &pixels->data.pixels_16bit_colour_index
                                              [VIO_IJ(y,end_x[y]+1,x_size)];
            for_less( x, end_x[y]+1, x_pixel_end+1 )
            {
                *pixel_ptr = (unsigned short) empty_colour;
                ++pixel_ptr;
            }
        }
    }

    if( render_storage == NULL )
        delete_render_storage( (void *) store );
}
#endif
