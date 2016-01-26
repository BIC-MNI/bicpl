#include "bicpl_internal.h"

/**
 * \file brainsuite_io.c
 * \brief Input BrainSuite surface and tractography files.
 *
 * This is a _very_ limited implementation. It assumes little-endian files
 * and little-endian architecture.
 *
 * \copyright
              Copyright 1993-2016 David MacDonald and Robert D. Vincent
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
    if (sscanf(meta_p2 + sizeof(prefix) - 1, "%f %f %f", &r, &g, &b) != 3)
    {
      return FALSE;
    }
    colours[n_colours++] = make_Colour_0_1(r, g, b);
    meta_p1 = meta_p2 + sizeof(prefix);
  }
  return TRUE;
}

#define DFT_MAGIC_N_BYTES 8

/**
 * Read a BrainSuite .dft/.dfc format tractography file, saving it as a
 * LINES object in our internal format.
 * \param fp An open file pointer set to the beginning of the file.
 * \param object_ptr A LINES object pointer that has already been created
 * but not necessarily initialized.
 * \returns TRUE if successful.
 */
static VIO_BOOL
input_brainsuite_tractography_file( FILE *fp, object_struct *object_ptr )
{
  char temp[DFT_MAGIC_N_BYTES];
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

  lines_ptr = get_lines_ptr( object_ptr );
  initialize_lines( lines_ptr, WHITE );

  if (fread(temp, sizeof(char), DFT_MAGIC_N_BYTES, fp) != DFT_MAGIC_N_BYTES)
    return FALSE;

  /* check magic */
  if (strcmp("DFT_LE", temp) && strcmp("DFC_LE", temp))
    return FALSE;

  print("Reading BrainSuite tractography.\n");

  if (fread(temp, sizeof(char), 4, fp) != 4)
    return FALSE;

  /* Paranoia - require version 1.0.0.X */
  if (temp[0] != 1 || temp[1] != 0 || temp[2] != 0)
  {
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

  if (fseek(fp, meta_offset, SEEK_SET) != 0)
  {
    free(metadata);
    return FALSE;
  }

  if (fread(metadata, sizeof(char), n_meta, fp) != n_meta)
  {
    free(metadata);
    return FALSE;
  }

  if (!parse_brainsuite_metadata(metadata, n_meta, lines_ptr->colours))
  {
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

#define DFS_MAGIC_N_BYTES 12
#define N_PER_TRIANGLE 3

#define SIZEOF_POINT ( sizeof( float ) * N_PER_TRIANGLE )
#define SIZEOF_TRIANGLE (sizeof( int32_t ) * N_PER_TRIANGLE)

/**
 * Read a BrainSuite .dfs format surface file, saving it as a
 * POLYGONS object in our internal format.
 * \param fp An open file pointer set to the beginning of the file.
 * \param object_ptr A POLYGONS object pointer that has already been created
 * but not necessarily initialized.
 * \returns TRUE if successful.
 */
static VIO_BOOL
input_brainsuite_surface_file( FILE *fp, object_struct *object_ptr )
{
  char temp[DFS_MAGIC_N_BYTES];
  polygons_struct *poly_ptr;
  int32_t header_size;
  int32_t meta_offset;
  int32_t subject_offset;
  uint32_t n_items;
  uint32_t n_points;
  int32_t n_strips;
  int32_t strip_size;
  int32_t normals_offset;
  int32_t vtx_uv_offset;
  int32_t vtx_col_offset;
  int32_t vtx_lbl_offset;
  uint32_t i;

  poly_ptr = get_polygons_ptr( object_ptr );
  initialize_polygons( poly_ptr, WHITE, NULL );

  if ( fread( temp, sizeof(char), DFS_MAGIC_N_BYTES, fp ) != DFS_MAGIC_N_BYTES )
    return FALSE;

  if ( strcmp(temp, "DFS_LE v2.0") )
    return FALSE;

  print("Reading BrainSuite surface.\n");

  if ( fread( &header_size, sizeof( header_size ), 1, fp ) != 1 )
    return FALSE;

  if ( fread( &meta_offset, sizeof( meta_offset ), 1, fp ) != 1)
    return FALSE;

  if (fread( &subject_offset, sizeof( subject_offset ), 1, fp ) != 1)
    return FALSE;

  if (fread( &n_items, sizeof( n_items ), 1, fp ) != 1)
    return FALSE;

  if (fread( &n_points, sizeof( n_points ), 1, fp ) != 1)
    return FALSE;

  if (fread( &n_strips, sizeof( n_strips ), 1, fp ) != 1)
    return FALSE;

  if (fread( &strip_size, sizeof( strip_size ), 1, fp ) != 1)
    return FALSE;

  if (fread( &normals_offset, sizeof( normals_offset ), 1, fp ) != 1)
    return FALSE;

  if (fread( &vtx_uv_offset, sizeof( vtx_uv_offset ), 1, fp ) != 1)
    return FALSE;

  if (fread( &vtx_col_offset, sizeof( vtx_col_offset ), 1, fp ) != 1)
    return FALSE;

  if (fread( &vtx_lbl_offset, sizeof( vtx_lbl_offset ), 1, fp ) != 1)
    return FALSE;

  poly_ptr->n_points = n_points;
  poly_ptr->n_items = n_items;

  ALLOC( poly_ptr->indices, n_items * N_PER_TRIANGLE );
  ALLOC( poly_ptr->points, n_points );
  ALLOC( poly_ptr->end_indices, n_items );

  if (fseek( fp, header_size, SEEK_SET ) != 0 )
    return FALSE;

  if (fread( poly_ptr->indices, SIZEOF_TRIANGLE, n_items, fp ) != n_items )
    return FALSE;

  if (fread( poly_ptr->points, SIZEOF_POINT, n_points, fp ) != n_points )
    return FALSE;

  for (i = 0; i < n_items; i++)
    poly_ptr->end_indices[i] = (i + 1) * N_PER_TRIANGLE;

  ALLOC( poly_ptr->normals, n_points );

  if (normals_offset == 0)
  {
    compute_polygon_normals( poly_ptr );
  }
  else
  {
    if (fseek( fp, normals_offset, SEEK_SET ) != 0)
      return FALSE;

    if (fread( poly_ptr->normals, SIZEOF_POINT, n_points, fp ) != n_points)
      return FALSE;
  }

  if (vtx_col_offset != 0)
  {
    float *temp_colours;

    FREE( poly_ptr->colours );
    poly_ptr->colour_flag = PER_VERTEX_COLOURS;
    ALLOC( poly_ptr->colours, n_points );
    ALLOC( temp_colours, n_points * 3 );
    if ( fseek( fp, vtx_col_offset, SEEK_SET) != 0 )
      return FALSE;
    if ( fread( temp_colours, sizeof(float) * 3, n_points, fp ) != n_points )
      return FALSE;
    for ( i = 0; i < n_points; i++ )
      poly_ptr->colours[i] = make_Colour_0_1( temp_colours[i * 3 + 0],
                                              temp_colours[i * 3 + 1],
                                              temp_colours[i * 3 + 2]);
    FREE( temp_colours );
  }
  if (vtx_lbl_offset != 0)
  {
    print("Label information is present.\n");
  }
  return TRUE;
}

/**
 * This is a simple function table we step through as we attempt
 * to interpret the files as various BrainSuite formats.
 */
static struct input_func_table
{
  VIO_BOOL (*input_fn)(FILE *, object_struct *);
  Object_types object_type;
} table[] =
{
  { input_brainsuite_tractography_file, LINES },
  { input_brainsuite_surface_file, POLYGONS },
  { NULL, 0 }
};

/**
 * Function to read BrainSuite geometric files into the internal
 * object representation.
 * \param filename The path to the file.
 * \param n_objects A pointer to the object count field of the model.
 * \param object_list A pointer to the object list field of the model.
 * \returns TRUE if successful.
 */
BICAPI VIO_BOOL
input_brainsuite_graphics_file(const char *filename,
                               int *n_objects,
                               object_struct **object_list[])
{
  VIO_BOOL result = FALSE;
  int i;

  for ( i = 0; !result && table[i].input_fn != NULL; i++ )
  {
    FILE *fp;

    if ( ( fp = fopen( filename, "r" )) != NULL )
    {
      object_struct *object_ptr = create_object( table[i].object_type );

      if ( !( *table[i].input_fn )( fp, object_ptr ) )
      {
        delete_object( object_ptr );
      }
      else
      {
        add_object_to_list( n_objects, object_list, object_ptr );
        result = TRUE;
      }
      fclose( fp );
    }
  }
  return result;
}
