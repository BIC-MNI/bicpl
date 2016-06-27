#include "bicpl_internal.h"
#include <ctype.h>

/**
 * \file ply_io.c
 * \brief Input Stanford .ply format surface files.
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

static void
swap_bytes(char *ptr, int n_total, int n_per_item)
{
  int d;
  for (d = 0; d < n_total; d += n_per_item)
  {
    int hi_offset = n_per_item - 1;
    int lo_offset = 0;
    while (hi_offset > lo_offset)
    {
      int tmp = ptr[d + hi_offset];
      ptr[d + hi_offset] = ptr[d + lo_offset];
      ptr[d + lo_offset] = tmp;
      hi_offset--;
      lo_offset++;
    }
  }
}

static void
trim(char *str)
{
  char *p = str;
  while (*p != '\0')
  {
    p++;
  }
  while (--p >= str)
  {
    if (*p == '\n' || *p == '\r' || *p == ' ')
    {
      *p = 0;
    }
    else
    {
      break;
    }
  }
}

typedef enum element_class
{
  NONE, VERTEX, FACE
} element_class_t;

typedef enum property_type
{
  CHAR, UCHAR, SHORT, USHORT,
  INT, UINT, FLOAT, DOUBLE
} property_type_t;

typedef enum property_id
{
  ID_X, ID_Y, ID_Z, ID_R, ID_G, ID_B
} property_id_t;


static int
read_ply_int(FILE *fp, int ply_type, int big_endian)
{
  char buf[8];

  switch (ply_type)
  {
  case CHAR:
    fread(&buf[0], 1, 1, fp);
    return (int) (char) buf[0];
  case UCHAR:
    fread(&buf[0], 1, 1, fp);
    return (int) (unsigned char) buf[0];
  case SHORT:
    fread(&buf[0], 2, 1, fp);
    if (big_endian)
      swap_bytes(&buf[0], 2, 2);
    return (int) *(short *)&buf[0];
  case USHORT:
    fread(&buf[0], 2, 1, fp);
    if (big_endian)
      swap_bytes(&buf[0], 2, 2);
    return (int) *(unsigned short *)&buf[0];
  case INT:
    fread(&buf[0], 4, 1, fp);
    if (big_endian)
      swap_bytes(&buf[0], 4, 4);
    return (int) *(int *)&buf[0];
  case UINT:
    fread(&buf[0], 4, 1, fp);
    if (big_endian)
      swap_bytes(&buf[0], 4, 4);
    return (int) *(unsigned int *)&buf[0];
  default:
    printf("???? %d\n", ply_type);
    return -1;
  }
}

static int
plysize(int type_code)
{
  if (type_code == DOUBLE)
    return 8;
  else if (type_code >= INT)
    return 4;
  else if (type_code >= SHORT)
    return 2;
  return 1;
}

static int
plytype(char *ptr, char **endptr)
{
  static struct typename
  {
    const char *name;
    property_type_t code;
  } typenames[] = {
    {"char", CHAR },
    {"uchar", UCHAR },
    {"short", SHORT },
    {"ushort", USHORT },
    {"int", INT },
    {"uint", UINT },
    {"float", FLOAT },
    {"double", DOUBLE },
    {"int8", CHAR },
    {"uint8", UCHAR },
    {"int16", SHORT },
    {"uint16", USHORT },
    {"int32", INT },
    {"uint32", UINT },
    {"float32", FLOAT },
    {"float64", DOUBLE },
    {NULL, 0}
  };
    
  property_type_t typecode = -1;
  int i;

  while (isspace(*ptr))
  {
    ptr++;
  }

  for (i = 0; typenames[i].name != NULL; i++)
  {
    int nlen = strlen( typenames[i].name );
    if (!strncmp( ptr, typenames[i].name, nlen ) && isspace( ptr[nlen] ))
    {
      ptr += nlen + 1;
      typecode = typenames[i].code;
      break;
    }
  }

  if (endptr != NULL)
  {
    *endptr = ptr;
  }
  return typecode;
}

static int
plyprop(char *ptr, char **endptr)
{
  static struct propname
  {
    const char *name;
    int code;
  } propnames[] = {
    {"x", ID_X },
    {"y", ID_Y },
    {"z", ID_Z },
    {"red", ID_R },
    {"green", ID_G },
    {"blue", ID_B },
    {NULL, 0}
  };
    
  property_id_t idcode = -1;
  int i;

  while (isspace(*ptr))
  {
    ptr++;
  }

  for (i = 0; propnames[i].name != NULL; i++)
  {
    int nlen = strlen( propnames[i].name );
    if (!strncmp( ptr, propnames[i].name, nlen ) && ptr[nlen] == '\0')
    {
      ptr += nlen + 1;
      idcode = propnames[i].code;
      break;
    }
  }

  if (idcode < 0)
  {
    while (!isspace(*ptr) && *ptr != '\0')
    {
      ptr++;
    }
  }

  if (endptr != NULL)
  {
    *endptr = ptr;
  }
  return idcode;
}

/**
 * Read a Stanford .ply format surface file, saving it as a
 * POLYGONS object in our internal format.
 * \param fp An open file pointer set to the beginning of the file.
 * \param object_ptr A POLYGONS object pointer that has already been created
 * but not necessarily initialized.
 * \returns TRUE if successful.
 */

static VIO_BOOL
input_ply_surface_file( FILE *fp, object_struct *object_ptr )
{
  char line[1024];
  int n_indices = 0;
  int n_normals = 0;
  int is_binary;
  int big_endian;
  polygons_struct *poly_ptr = get_polygons_ptr( object_ptr );
  int element_class = NONE;
  int i;
  char *p;
  int vertex_size = 0;
  int vertex_list_type = -1;
  int vertex_elem_type = -1;

  initialize_polygons( poly_ptr, WHITE, NULL );

  fgets( line, sizeof(line), fp );
  trim( line );
  if (strcmp( line, "ply" ))
  {
    return FALSE;
  }
  fgets( line, sizeof(line), fp );
  trim( line );
  if (!strcmp( line, "format ascii 1.0" ))
  {
    is_binary = 0;
  }
  else if (!strcmp( line, "format binary_little_endian 1.0" ))
  {
    is_binary = 1;
    big_endian = 0;
  }
  else if (!strcmp( line, "format binary_big_endian 1.0" ))
  {
    is_binary = 1;
    big_endian = 1;
  }
  else
  {
    printf("format line error '%s'.\n", line);
    return FALSE;
  }
  
  while ( fgets( line, sizeof(line), fp ) )
  {
    p = &line[0];
    trim( line );
    if (!strncmp( line, "comment ", 8 ))
    {
      continue;
    }
    else if (!strncmp( line, "element ", 8 ))
    {
      if (!strncmp( &line[8], "vertex ", 7 ))
      {
        element_class = VERTEX;
        vertex_size = 0;
        poly_ptr->n_points = strtol( &line[15], NULL, 10 );
        ALLOC( poly_ptr->points, poly_ptr->n_points );
      }
      else if (!strncmp( &line[8], "face ", 5))
      {
        element_class = FACE;
        poly_ptr->n_items = strtol( &line[13], NULL, 10 );
        ALLOC( poly_ptr->end_indices, poly_ptr->n_items );
      }
      else
      {
        printf( "unknown element '%s'\n", line );
        element_class = NONE;
        continue;
      }
    }
    else if (!strncmp( line, "property ", 9))
    {
      p += 9;
      if (element_class == VERTEX)
      {
        int id;
        int element_type;

        /* expect several lines of the form
           property <type> x
           property <type> y
           property <type> z
           [property <type> r]
           [property <type> g]
           [property <type> b]
        */
        if ( ( element_type = plytype( p, &p ) ) < 0 )
        {
          printf("not a type '%s'\n", line);
          return FALSE;
        }

        /* accumulate size for binary records. */
        vertex_size += plysize( element_type );

        if ( ( id = plyprop( p, &p ) ) < 0 )
        {
          printf("unknown property '%s'\n", line);
        }
        
        if ( id == ID_R )
        {
          poly_ptr->colour_flag = PER_VERTEX_COLOURS;
          ALLOC( poly_ptr->colours, poly_ptr->n_points );
        }
      }
      else if ( element_class == FACE )
      {
        int tmp_list_type;
        int tmp_elem_type;
        /* expect one line of the form:
           list <type1> <type2> vertex_indices
        */
        if (!strncmp( p, "list ", 5 ))
        {
          p += 5;
        }
        else
        {
          printf("list not found where expected\n");
          return FALSE;
        }
        if ( ( tmp_list_type = plytype( p, &p ) ) < 0 )
        {
          return FALSE;
        }
        if ( ( tmp_elem_type = plytype( p, &p ) ) < 0 )
        {
          return FALSE;
        }
        while ( isspace( *p ) )
        {
          p++;
        }
        if (!strncmp( p, "vertex_indices", 14 ))
        {
          vertex_list_type = tmp_list_type;
          vertex_elem_type = tmp_elem_type;
        }
        else
        {
          /* unknown alternative list? */
        }
      }
      else
      {
        printf("unknown element class '%s'.\n", line);
        continue;
      }
    }
    else if (!strcmp( line, "end_header"))
    {
      break;
    }
    else
    {
      printf("unknown header directive '%s'\n", line);
      continue;
    }
  }

  if (!is_binary)
  {
    for ( i = 0; i < poly_ptr->n_points; i++ )
    {
      if ((i % 10000) == 0) printf("vertex %d\n", i);
      if (!fgets( line, sizeof(line), fp ))
      {
        printf("premature end of file parsing vertices.\n");
        return FALSE;
      }
      p = &line[0];
      Point_x( poly_ptr->points[i] ) = strtod( p, &p );
      Point_y( poly_ptr->points[i] ) = strtod( p, &p );
      Point_z( poly_ptr->points[i] ) = strtod( p, &p );
      if ( poly_ptr->colour_flag == PER_VERTEX_COLOURS )
      {
        int r = strtol( p, &p, 10 );
        int g = strtol( p, &p, 10 );
        int b = strtol( p, &p, 10 );

        poly_ptr->colours[i] = make_Colour(r, g, b);
      }
    }

    for (i = 0; i < poly_ptr->n_items; i++)
    {
      int j;
      int n_el = 0;
      int i_pt;

      if ((i % 10000) == 0) printf("face %d\n", i);

      if (!fgets( line, sizeof(line), fp ))
      {
        printf("premature end of file parsing vertices.\n");
        return FALSE;
      }

      p = &line[0];
      n_el = strtol(p, &p, 10);

      poly_ptr->end_indices[i] = n_indices + n_el;

      for (j = 0; j < n_el; j++)
      {
        i_pt = strtol(p, &p, 10);
        if (i_pt < 0 || i_pt >= poly_ptr->n_points)
        {
          printf("illegal vertex index in face.\n");
        }
        ADD_ELEMENT_TO_ARRAY( poly_ptr->indices,
                              n_indices,
                              i_pt,
                              DEFAULT_CHUNK_SIZE);

      }
    }
  }
  else
  {
    char buf[128];

    for ( i = 0; i < poly_ptr->n_points; i++ )
    {
      if ((i % 10000) == 0) printf("vertex %d\n", i);

      fread(buf, vertex_size, 1, fp);
      if (big_endian)
      {
        swap_bytes( buf, sizeof(buf), 4 );
      }
      Point_x( poly_ptr->points[i] ) = *(float *)&buf[0];
      Point_y( poly_ptr->points[i] ) = *(float *)&buf[4];
      Point_z( poly_ptr->points[i] ) = *(float *)&buf[8];
    }

    for ( i = 0; i < poly_ptr->n_items; i++ )
    {
      int n_el;
      int j;

      if ((i % 10000) == 0) printf("face %d\n", i);

      n_el = read_ply_int( fp, vertex_list_type, big_endian );

      poly_ptr->end_indices[i] = n_indices + n_el;

      for ( j = 0; j < n_el; j++ )
      {
        int i_pt = read_ply_int( fp, vertex_elem_type, big_endian );

        /* Using n_items as the "chunk size" here helps improve perfomance,
         * and is probably generally efficient because we expect that the
         * total number of vertices will be a small multiple of the total
         * number of polygons.
         */
        ADD_ELEMENT_TO_ARRAY( poly_ptr->indices,
                              n_indices,
                              i_pt,
                              poly_ptr->n_items );
      }
    }
  }

  if (n_normals == 0)
  {
    ALLOC( poly_ptr->normals, poly_ptr->n_points );
    compute_polygon_normals( poly_ptr );
  }
  return TRUE;
}

/**
 * Function to read BrainSuite geometric files into the internal
 * object representation.
 * \param filename The path to the file.
 * \param n_objects A pointer to the object count field of the model.
 * \param object_list A pointer to the object list field of the model.
 * \returns TRUE if successful.
 */
BICAPI VIO_BOOL
input_ply_graphics_file(const char *filename,
                        int *n_objects,
                        object_struct **object_list[])
{
  VIO_BOOL result = FALSE;
  FILE *fp;

  if ( ( fp = fopen( filename, "rb" )) != NULL )
  {
    object_struct *object_ptr = create_object( POLYGONS );

    if ( !input_ply_surface_file( fp, object_ptr ) )
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
  return result;
}
