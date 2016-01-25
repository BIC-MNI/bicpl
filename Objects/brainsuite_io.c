#include "bicpl_internal.h"

/**
 * \file brainsuite_io.c
 * \brief Input BrainSuite surface files.
 *
 * \copyright
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
 */

/** Simple interpretation of the brainsuite tractography file metadata - all
    we care about is the colours, so we just grab them as simply as possible.
*/
static VIO_BOOL
parse_brainsuite_metadata(const char *metadata, size_t n_meta, 
                          VIO_Colour *colours)
{
  const char *meta_p1, *meta_p2, *meta_end;
  int n_colours;

  meta_p1 = metadata;
  meta_end = metadata + n_meta;
  n_colours = 0;
  const char prefix[] = "color=\"";
  while (meta_p1 < meta_end) 
  {
    float r, g, b;

    meta_p2 = strstr(meta_p1, prefix);
    if (meta_p2 == NULL)
      break;
    if (sscanf(meta_p2 + sizeof(prefix) - 1, "%f %f %f", &r, &g, &b) != 3) {
      return FALSE;
    }
    colours[n_colours++] = make_Colour_0_1(r, g, b);
    meta_p1 = meta_p2 + sizeof(prefix);
  }
  return TRUE;
}

#define BS_MAGIC_N_BYTES 8

static VIO_BOOL
input_brainsuite_tractography_file( FILE *fp, object_struct *object_ptr )
{
  char temp[BS_MAGIC_N_BYTES];
  lines_struct *lines_ptr;
  int32_t header_size;
  int32_t data_offset;
  int32_t meta_offset;
  int32_t subject_offset;
  int32_t n_curves;
  char *metadata;
  size_t n_meta;
  int i;
  int n_indices;

  if (fread(temp, sizeof(char), BS_MAGIC_N_BYTES, fp) != BS_MAGIC_N_BYTES)
    return FALSE;

  /* check magic */
  if (strcmp("DFT_LE", temp) && strcmp("DFC_LE", temp))
    return FALSE;

  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( lines_ptr, WHITE );

  if (fread(temp, sizeof(char), 4, fp) != 4)
    return FALSE;

  /* Paranoia - require version 1.0.0.X */
  if (temp[0] != 1 || temp[1] != 0 || temp[2] != 0) {
    print_error("Unknown DFT/DFC version.\n");
    return FALSE;
  }
  
  if (fread(&header_size, sizeof(int32_t), 1, fp) != 1)
    return FALSE;

  if (fread(&data_offset, sizeof(int32_t), 1, fp) != 1)
    return FALSE;

  if (fread(&meta_offset, sizeof(int32_t), 1, fp) != 1)
    return FALSE;

  if (fread(&subject_offset, sizeof(int32_t), 1, fp) != 1)
    return FALSE;

  if (fread(&n_curves, sizeof(int32_t), 1, fp) != 1)
    return FALSE;

  lines_ptr->line_thickness = 1.0;
  lines_ptr->n_items = n_curves;
  lines_ptr->colour_flag = PER_ITEM_COLOURS;
  lines_ptr->n_points = 0;

  ALLOC( lines_ptr->colours, n_curves );
  ALLOC( lines_ptr->end_indices, n_curves );

  n_meta = data_offset - meta_offset;
  if ((metadata = malloc(n_meta)) == NULL)
    return FALSE;

  if (fseek(fp, meta_offset, SEEK_SET) != 0) {
    free(metadata);
    return FALSE;
  }

  if (fread(metadata, sizeof(char), n_meta, fp) != n_meta) {
    free(metadata);
    return FALSE;
  }

  if (!parse_brainsuite_metadata(metadata, n_meta, lines_ptr->colours)) {
    free(metadata);
    return FALSE;
  }

  free(metadata);

  n_indices = 0;

  /* Read in each of the curves. 
   */
  for (i = 0; i < n_curves; i++)
  {
    int32_t n_curve_points;
    int j;

    /* Each curve consists of a integer that gives the number of
     * points in this curve, followed by a list of point coordinates in
     * XYZ order.
     */

    if (fread(&n_curve_points, sizeof(int32_t), 1, fp) != 1)
      return FALSE;

    SET_ARRAY_SIZE(lines_ptr->indices, n_indices, n_indices + n_curve_points, 1);
    SET_ARRAY_SIZE(lines_ptr->points, n_indices, n_indices + n_curve_points, 1);

    fread(&lines_ptr->points[n_indices], sizeof(float) * 3, n_curve_points, fp);
          
    for (j = 0; j < n_curve_points; j++)
    {
      lines_ptr->indices[n_indices] = n_indices;
      n_indices++;
    }
    lines_ptr->end_indices[i] = n_indices;
  }

  lines_ptr->n_points = n_indices;

  return TRUE;
}


/**
 * Function to read BrainSuite tractography files into the internal 
 * lines representation.
 */
BICAPI VIO_BOOL
input_brainsuite_graphics_file(const char *filename,
                               int *n_objects,
                               object_struct **object_list[])
{
  FILE *fp;
  VIO_BOOL result = FALSE;

  if ((fp = fopen(filename, "r")) != NULL) {
    object_struct *object_ptr = create_object( LINES );
    if (!input_brainsuite_tractography_file(fp, object_ptr)) {
      delete_object( object_ptr );
    }
    else {
      add_object_to_list( n_objects, object_list, object_ptr );
      result = TRUE;
    }
    fclose(fp);
  }
  return result;
}
