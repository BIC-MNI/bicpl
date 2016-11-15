#include "bicpl_internal.h"

/**
 * \file trackvis_io.c
 * \brief Input TrackVis tractography files.
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

#include <stdint.h>

/**
 * \brief Structure of a TrackVis .trk file.
 *
 * This structure defines the header. However, we don't rely 
 */
struct trackvis_header
{                               /* SIZE | OFFSET */
  char     id_string[6];        /*    6 |      0 */
  short    dim[3];              /*    6 |      6 */
  float    voxel_size[3];       /*   12 |     12 */
  float    origin[3];           /*   12 |     24 */
  uint16_t n_scalars;           /*    2 |     36 */
  char     scalar_name[10][20]; /*  200 |     38 */
  uint16_t n_properties;        /*    2 |    238 */
  char     property_name[10][20]; /* 200|    240 */
  float    vox_to_ras[4][4];    /*   64 |    440 */
  char     reserved[444];       /*  444 |    504 */
  char     voxel_order[4];      /*    4 |    948 */
  char     pad2[4];             /*    4 |    952 */
  float    image_orientation_patient[6]; /* 24 | 956 */
  char     pad1[2];             /*    2 |    980 */
  uint8_t  invert_x;            /*    1 |    982 */
  uint8_t  invert_y;            /*    1 |    983 */
  uint8_t  invert_z;            /*    1 |    984 */
  uint8_t  swap_xy;             /*    1 |    985 */
  uint8_t  swap_yz;             /*    1 |    986 */
  uint8_t  swap_zx;             /*    1 |    987 */
  uint32_t n_count;             /*    4 |    988 */
  uint32_t version;             /*    4 |    992 */
  uint32_t hdr_size;            /*    4 |    996 */
};

#define TRACKVIS_OFF_N_SCALARS 36
#define TRACKVIS_OFF_N_PROPERTIES 238
#define TRACKVIS_OFF_N_COUNT 988
#define TRACKVIS_OFF_HDR_SIZE 996

extern void swap_bytes( void *ptr, int n_total, int n_per_item );

/**
 * Function to read TrackVis geometric files into the internal
 * object representation.
 * \param filename The path to the file.
 * \param n_objects A pointer to the object count field of the model.
 * \param object_list A pointer to the object list field of the model.
 * \returns TRUE if successful.
 */
BICAPI VIO_BOOL
input_trackvis_graphics_file(const char *filename,
                             int *n_objects,
                             object_struct **object_list[])
{
  FILE          *fp;
  object_struct *object_ptr;
  lines_struct  *lines_ptr;
  VIO_BOOL      need_swap;
  char          id_string[6];
  uint16_t      n_scalars;
  uint16_t      n_properties;
  uint32_t      hdr_size;
  uint32_t      n_count;
  uint32_t      n_points;
  
  if ( ( fp = fopen( filename, "rb" )) == NULL )
  {
    return FALSE;
  }
  
  fread( id_string, sizeof(id_string), 1, fp );

  if ( strncmp( id_string, "TRACK", 5 ) != 0 )
  {
    fclose( fp );
    return FALSE;
  }

  fseek( fp, TRACKVIS_OFF_N_SCALARS, 0 );
  fread( &n_scalars, sizeof(n_scalars), 1, fp );

  fseek( fp, TRACKVIS_OFF_N_PROPERTIES, 0 );
  fread( &n_properties, sizeof(n_properties), 1, fp );

  fseek( fp, TRACKVIS_OFF_N_COUNT, 0 );
  fread( &n_count, sizeof(n_count), 1, fp );
    
  fseek( fp, TRACKVIS_OFF_HDR_SIZE, 0 );
  fread( &hdr_size, sizeof(hdr_size), 1, fp );

  if ( hdr_size == 0xe8030000 ) /* 1000 hex, byte-swapped */
  {
    need_swap = TRUE;
  }
  else if ( hdr_size == 0x000003e8 ) /* 1000 hex */
  {
    need_swap = FALSE;
  }
  else
  {
    fclose( fp );
    return FALSE;
  }

  if (need_swap)
  {
    /* Swap the fields we will need to use.
     */
    swap_bytes( &n_properties, sizeof(n_properties), sizeof(n_properties) );
    swap_bytes( &n_scalars, sizeof(n_scalars), sizeof(n_scalars) );
    swap_bytes( &n_count, sizeof(n_count), sizeof(n_count) );
  }

  print( "Reading TrackVis file with %d tracks, %d scalars, %d properties.\n",
         n_count, n_scalars, n_properties );

  object_ptr = create_object( LINES );
  lines_ptr = get_lines_ptr( object_ptr );

  initialize_lines( lines_ptr, WHITE );

  while ( fread( &n_points, sizeof(n_points), 1, fp ) != 0 )
  {
    float     coord[3];
    uint32_t  i, j;
    VIO_Point pt;
    float     tmp;

    if ( need_swap )
    {
      swap_bytes( &n_points, sizeof(int), sizeof(int) );
    }

    start_new_line( lines_ptr );
    for ( i = 0; i < n_points; i++ )
    {
      fread( coord, sizeof(float), 3, fp );
      if ( need_swap )
      {
        swap_bytes( coord, sizeof(float) * 3, sizeof(float) );
      }

      /* Read and discard scalars. */
      for ( j = 0; j < n_scalars; j++ )
      {
        fread( &tmp, sizeof(float), 1, fp );
      }
      fill_Point( pt, coord[0], coord[1], coord[2] );
      add_point_to_line( lines_ptr, &pt );
    }
    /* Read and discard properties. */
    for (j = 0; j < n_properties; j++)
    {
      fread( &tmp, sizeof(float), 1, fp );
    }
  }
  add_object_to_list( n_objects, object_list, object_ptr );
  fclose( fp );
  return TRUE;
}
