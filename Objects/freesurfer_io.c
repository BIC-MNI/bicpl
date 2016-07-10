/**
 * \file freesurfer_io.c
 * \brief Input FreeSurfer surface files.
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

#include "bicpl_internal.h"

#include <arpa/inet.h>

static VIO_BOOL
input_freesurfer_binary_triangle_file(FILE *fp, polygons_struct *polygons)
{
  char creator_string[VIO_EXTREMELY_LARGE_STRING_SIZE];
  int i, j;
  int n_vertices;
  int n_faces;
  int n_indices;
  const int n_vertices_per_face = 3; /* Only triangles for now. */
  VIO_BOOL format_error = FALSE;

  for (i = 0; i < VIO_EXTREMELY_LARGE_STRING_SIZE; i++) {
    fread(&creator_string[i], 1, 1, fp);
    /* Creator string is terminated by two newlines. We replace
     * the second one with a null character.
     */
    if (creator_string[i] == '\n' && i > 0 && creator_string[i - 1] == '\n') {
      creator_string[i] = '\0';
      break;
    }
  }
  fread(&n_vertices, sizeof(int), 1, fp);
  fread(&n_faces, sizeof(int), 1, fp);
  n_vertices = ntohl(n_vertices);
  n_faces = ntohl(n_faces);

  polygons->n_points = n_vertices;
  ALLOC( polygons->points, n_vertices );

  for (i = 0; i < n_vertices && !format_error; i++) {
    float xyz[3];

    if (fread(xyz, sizeof(float), 3, fp) != 3) {
      format_error = TRUE;
    }
    else {
      int temp;
      VIO_Point pt;

      temp = ntohl(*(int *)&xyz[0]);
      Point_x(pt) = *(float *)&temp;
      temp = ntohl(*(int *)&xyz[1]);
      Point_y(pt) = *(float *)&temp;
      temp = ntohl(*(int *)&xyz[2]);
      Point_z(pt) = *(float *)&temp;
      polygons->points[i] = pt;
    }
  }

  n_indices = n_faces * n_vertices_per_face;
  polygons->n_items = n_faces;
  ALLOC(polygons->indices, n_indices);
  ALLOC(polygons->end_indices, polygons->n_items);
  ALLOC(polygons->normals, n_vertices);

  for (i = 0; i < n_faces && !format_error; i++) {
    int v[3];

    if (fread(v, sizeof(int), n_vertices_per_face, fp) != (size_t) n_vertices_per_face) {
      format_error = TRUE;
    }

    for (j = 0; j < n_vertices_per_face; j++) {
      v[j] = ntohl(v[j]);
      if (v[j] < 0 || v[j] > n_vertices) {
        format_error = TRUE;
        break;
      }
      polygons->indices[i * n_vertices_per_face + j] = v[j];
    }
    polygons->end_indices[i] = (i + 1) * n_vertices_per_face;
  }
  return !format_error;
}

static VIO_BOOL
input_freesurfer_ascii_triangle_file(FILE *fp, polygons_struct *polygons)
{
  char first_line[VIO_EXTREMELY_LARGE_STRING_SIZE];
  int i, j;
  int n_vertices, n_faces, n_indices;
  int n_vertices_per_face;
  VIO_BOOL format_error = FALSE;

  n_vertices_per_face = 3;
  for (i = 0; i < VIO_EXTREMELY_LARGE_STRING_SIZE; i++) {
    fread(&first_line[i], 1, 1, fp);
    /* Creator string is terminated by two newlines. We replace
     * the second one with a null character.
     */
    if (first_line[i] == '\n') {
      first_line[i] = '\0';
      break;
    }
  }
  fscanf(fp, "%d %d\n", &n_vertices, &n_faces);

  polygons->n_points = n_vertices;
  ALLOC( polygons->points, n_vertices );

  for (i = 0; i < n_vertices && !format_error; i++) {
    float x, y, z;
    int in_patch;

    if (fscanf(fp, "%f %f %f %d\n", &x, &y, &z, &in_patch) != 4) {
      format_error = TRUE;
    }
    else if (in_patch < 0 || in_patch > 1) {
      format_error = TRUE;
    }
    else {
      VIO_Point pt;

      Point_x(pt) = x;
      Point_y(pt) = y;
      Point_z(pt) = z;
      polygons->points[i] = pt;
    }
  }

  n_indices = n_faces * n_vertices_per_face;
  polygons->n_items = n_faces;
  ALLOC(polygons->indices, n_indices);
  ALLOC(polygons->end_indices, polygons->n_items);
  ALLOC(polygons->normals, n_vertices);

  for (i = 0; i < n_faces && !format_error; i++) {
    int v;
    int in_patch;

    for (j = 0; j < n_vertices_per_face; j++) {
      fscanf(fp, "%d", &v);
      if (v < 0 || v > n_vertices) {
        format_error = TRUE;
        break;
      }
      polygons->indices[i * n_vertices_per_face + j] = v;
    }

    if (fscanf(fp, "%d", &in_patch) != 1 || in_patch < 0 || in_patch > 1) {
      format_error = TRUE;
    }

    polygons->end_indices[i] = (i + 1) * n_vertices_per_face;
  }
  return !format_error;
}

#define FS_MAGIC_N_BYTES 3

/**
 * Function to read FreeSurfer surface files into the internal 
 * surface representation.
 */
BICAPI VIO_BOOL
input_freesurfer_graphics_file(const char *filename,
                               int *n_objects,
                               object_struct **object_list[])
{
  FILE *fp;
  unsigned char magic[FS_MAGIC_N_BYTES];
  VIO_BOOL result = FALSE;
  int file_type = -1;
  object_struct *object;
  polygons_struct *polygons;

  if ((fp = fopen(filename, "r")) == NULL) {
    return FALSE;
  }

  fread(magic, FS_MAGIC_N_BYTES, 1, fp);
  if (magic[0] == 0xff || magic[1] == 0xff || magic[2] == 0xfe) {
    file_type = 0;
  }
  else if (magic[0] == '#' && magic[1] == '!' && magic[2] == 'a') {
    file_type = 1;
  }
  else {
    fclose(fp);
    return FALSE;
  }

  object = create_object( POLYGONS );
  polygons = get_polygons_ptr( object );
  initialize_polygons( polygons, WHITE, NULL);

  /* Set surfprop to reasonable default values. */
  polygons->surfprop.a = 0.3;
  polygons->surfprop.d = 0.3;
  polygons->surfprop.s = 0.4;
  polygons->surfprop.se = 10.0;
  polygons->surfprop.t = 1.0;

  if (file_type == 0) {
    result = input_freesurfer_binary_triangle_file(fp, polygons);
  }
  else {
    result = input_freesurfer_ascii_triangle_file(fp, polygons);
  }
  if (result) {
    compute_polygon_normals( polygons );
    add_object_to_list( n_objects, object_list, object );
    result = TRUE;
  }
  fclose(fp);
  return result;
}

