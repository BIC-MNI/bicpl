#include "bicpl_internal.h"
#include <ctype.h>

/**
 * \file ply_io.c
 * \brief Input Stanford .ply format surface files.
 *
 * This is an incomplete but functional implementation. It has not been 
 * thoroughly tested because I have access to only a fairly small database 
* of files, and the format is both vague and complex.
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

void
swap_bytes( void *_ptr, int n_total, int n_per_item )
{
  int d;
  char *ptr = (char *) _ptr;
  for ( d = 0; d < n_total; d += n_per_item )
  {
    int hi_offset = d + n_per_item - 1;
    int lo_offset = d + 0;
    while ( hi_offset > lo_offset )
    {
      int tmp = ptr[hi_offset];
      ptr[hi_offset] = ptr[lo_offset];
      ptr[lo_offset] = tmp;
      hi_offset--;
      lo_offset++;
    }
  }
}

static void
trim( char *str )
{
  char *p = str;
  while ( *p != '\0' )
  {
    p++;
  }
  while ( --p >= str )
  {
    if ( *p == '\n' || *p == '\r' || *p == ' ' )
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
  INT, UINT, FLOAT, DOUBLE, LIST
} property_type_t;

/** 
 * This defines the maximum number of bytes that a property can contain.
 */
#define MAX_PROPERTY_BYTES 2048

/** 
 * This defines the maximum number of properties that an element can
 * contain.
 */
#define MAX_PROPERTY_COUNT 24

struct property_record
{
  property_type_t prop_type;
  char *prop_name;
  property_type_t length_type;
  property_type_t element_type;
  int length;
  union {
    char   bval[MAX_PROPERTY_BYTES];
    short  sval[MAX_PROPERTY_BYTES / sizeof(short)];
    int    ival[MAX_PROPERTY_BYTES / sizeof(int)];
    float  fval[MAX_PROPERTY_BYTES / sizeof(float)];
    double dval[MAX_PROPERTY_BYTES / sizeof(double)];
  } u;
};

static int
read_ply_int( FILE *fp, int ply_type, int swap_needed )
{
  char buf[8];

  switch (ply_type)
  {
  case CHAR:
    fread( &buf[0], 1, 1, fp );
    return (int) (char) buf[0];
  case UCHAR:
    fread( &buf[0], 1, 1, fp );
    return (int) (unsigned char) buf[0];
  case SHORT:
    fread( &buf[0], 2, 1, fp );
    if ( swap_needed )
    {
      swap_bytes( &buf[0], 2, 2 );
    }
    return (int) *(short *)&buf[0];
  case USHORT:
    fread( &buf[0], 2, 1, fp );
    if ( swap_needed )
    {
      swap_bytes( &buf[0], 2, 2 );
    }
    return (int) *(unsigned short *)&buf[0];
  case INT:
    fread( &buf[0], 4, 1, fp );
    if ( swap_needed )
      swap_bytes( &buf[0], 4, 4 );
    return (int) *(int *)&buf[0];
  case UINT:
    fread( &buf[0], 4, 1, fp );
    if ( swap_needed )
      swap_bytes( &buf[0], 4, 4 );
    return (int) *(unsigned int *)&buf[0];
  default:
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
  else
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

static void
scan_property_definition(char *p, struct property_record *prop_ptr)
{
  if (!strncmp( p, "list ", 5 ))
  {
    p += 5;
    prop_ptr->prop_type = LIST;
    prop_ptr->length_type = plytype( p, &p );
    prop_ptr->element_type = plytype( p, &p );
  }
  else
  {
    prop_ptr->prop_type = plytype( p, &p );
    prop_ptr->element_type = prop_ptr->prop_type;
  }
  while ( isspace( *p ) )
  {
    p++;
  }

  prop_ptr->prop_name = strdup( p );
}

static VIO_BOOL
load_property_binary( FILE *fp, struct property_record *prop_ptr,
                      int swap_needed )
{
  int n_per_item = plysize( prop_ptr->element_type );
  int n_total;

  if ( prop_ptr->prop_type == LIST )
  {
    prop_ptr->length = read_ply_int( fp, prop_ptr->length_type, swap_needed );
  }
  else
  {
    prop_ptr->length = 1;
  }
  n_total = n_per_item * prop_ptr->length;
  if ( n_total > MAX_PROPERTY_BYTES )
  {
    print_error( "Property is too large!\n" );
    return FALSE;
  }
  fread( prop_ptr->u.bval, n_per_item, prop_ptr->length, fp );
  if ( swap_needed && n_per_item > 1 )
  {
    swap_bytes( prop_ptr->u.bval, n_total, n_per_item );
  }
  return TRUE;
}

static int
load_property_text( char *ptr, char **endptr, struct property_record *prop_ptr )
{
  int i;

  if ( prop_ptr->prop_type == LIST )
  {
    prop_ptr->length = strtol( ptr, &ptr, 10 );
  }
  else
  {
    prop_ptr->length = 1;
  }

  for ( i = 0; i < prop_ptr->length; i++ )
  {
    switch ( prop_ptr->element_type )
    {
    case DOUBLE:
      prop_ptr->u.dval[i] = strtod( ptr, &ptr );
      break;
    case FLOAT:
      prop_ptr->u.fval[i] = strtod( ptr, &ptr );
      break;
    case UINT:
    case INT:
      prop_ptr->u.ival[i] = strtol( ptr, &ptr, 10 );
      break;
    case USHORT:
    case SHORT:
      prop_ptr->u.sval[i] = strtol( ptr, &ptr, 10 );
      break;
    case UCHAR:
    case CHAR:
      prop_ptr->u.bval[i] = strtol( ptr, &ptr, 10 );
      break;
    default:
      print_error( "SNH %s:%d\n", __func__, __LINE__ );
      exit( -1 );
    }
  }
  *endptr = ptr;
  return 0;
}

static struct property_record *
find_property( struct property_record *prop_arr, int n, char *name )
{
  int i;

  for (i = 0; i < n; i++)
  {
    if (!strcmp( name, prop_arr[i].prop_name ))
    {
      return &prop_arr[i];
    }
  }
  return NULL;
}

static int
get_property_length( struct property_record *prop_ptr )
{
  return prop_ptr->length;
}

static int
get_property_element( struct property_record *prop_ptr, int k, void *value_ptr )
{
  if ( k < 0 || k >= prop_ptr->length )
  {
    print_error( "SNH %s:%d\n", __func__, __LINE__ );
    exit( -1 );
  }

  switch ( prop_ptr->element_type )
  {
  case CHAR:
    *((int *)value_ptr) = (int) prop_ptr->u.bval[k];
    break;
  case UCHAR:
    *((int *)value_ptr) = (int) (unsigned char) prop_ptr->u.bval[k];
    break;
  case SHORT:
    *((int *)value_ptr) = (int) prop_ptr->u.sval[k];
    break;
  case USHORT:
    *((int *)value_ptr) = (int) (unsigned short) prop_ptr->u.sval[k];
    break;
  case INT:
  case UINT:
    *((int *)value_ptr) = (int) prop_ptr->u.ival[k];
    break;
  case FLOAT:
    *((double *)value_ptr) = prop_ptr->u.fval[k];
    break;
  case DOUBLE:
    *((double *)value_ptr) = prop_ptr->u.dval[k];
    break;
  default:
    print_error( "SNH %s:%d\n", __func__, __LINE__ );
    exit(-1);
  }
  return 1;
}

/** 
 * Return non-zero if the processor is big-endian.
 */
static int
is_big_endian(void)
{
  union {
    int v;
    char c[sizeof(int)];
  } tmp = { 1 };
  return tmp.c[0] == 0;
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
  int swap_needed;
  polygons_struct *poly_ptr = get_polygons_ptr( object_ptr );
  int element_class = NONE;
  int i, j;
  struct property_record face_props[MAX_PROPERTY_COUNT];
  int face_count = 0;
  struct property_record vertex_props[MAX_PROPERTY_COUNT];
  int vertex_count = 0;
  struct property_record *prec_x, *prec_y, *prec_z;
  struct property_record *prec_r, *prec_g, *prec_b;
  struct property_record *prec_vertex_indices;
  double x, y, z;

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
    swap_needed = is_big_endian();
  }
  else if (!strcmp( line, "format binary_big_endian 1.0" ))
  {
    is_binary = 1;
    swap_needed = !is_big_endian();
  }
  else
  {
    print( "format line error '%s'.\n", line );
    return FALSE;
  }

  while ( fgets( line, sizeof(line), fp ) )
  {
    trim( line );
    if (!strncmp( line, "comment ", 8 ))
    {
      /* We just ignore comment lines. */
      continue;
    }
    else if (!strncmp( line, "element ", 8 ))
    {
      if (!strncmp( &line[8], "vertex ", 7 ))
      {
        element_class = VERTEX;
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
        print_error( "unknown element '%s'\n", line );
        element_class = NONE;
        continue;
      }
    }
    else if ( !strncmp( line, "property ", 9 ) )
    {
      if ( element_class == VERTEX )
      {
        scan_property_definition( &line[9], &vertex_props[vertex_count] );
        if ( ++vertex_count >= MAX_PROPERTY_COUNT )
        {
          print_error( "Too many vertex properties.\n" );
          return FALSE;
        }
      }
      else if ( element_class == FACE )
      {
        scan_property_definition( &line[9], &face_props[face_count] );
        if ( ++face_count >= MAX_PROPERTY_COUNT )
        {
          print_error( "Too many face properties.\n" );
          return FALSE;
        }
      }
      else
      {
        continue;
      }
    }
    else if (!strcmp( line, "end_header"))
    {
      break;
    }
    else
    {
      print_error( "unknown header directive '%s'\n", line );
      continue;
    }
  }

  prec_x = find_property( vertex_props, vertex_count, "x" );
  prec_y = find_property( vertex_props, vertex_count, "y" );
  prec_z = find_property( vertex_props, vertex_count, "z" );

  if ( prec_x == NULL || prec_y == NULL || prec_z == NULL )
  {
    print_error( "Can't read this .ply file, missing a coordinate.\n" );
    return FALSE;
  }

  prec_r = find_property( vertex_props, vertex_count, "red" );
  prec_g = find_property( vertex_props, vertex_count, "green" );
  prec_b = find_property( vertex_props, vertex_count, "blue" );

  if ( prec_r == NULL || prec_g == NULL || prec_b == NULL )
  {
    prec_r = find_property( vertex_props, vertex_count, "diffuse_red" );
    prec_g = find_property( vertex_props, vertex_count, "diffuse_green" );
    prec_b = find_property( vertex_props, vertex_count, "diffuse_blue" );
  }

  if ( prec_r != NULL && prec_g != NULL && prec_b != NULL )
  {
    poly_ptr->colour_flag = PER_VERTEX_COLOURS;
    FREE( poly_ptr->colours );
    ALLOC( poly_ptr->colours, poly_ptr->n_points );
  }

  prec_vertex_indices = find_property( face_props, face_count,
                                       "vertex_indices" );

  if ( prec_vertex_indices == NULL )
  {
    print_error( "Can't read this .ply file, missing the vertex indices.\n" );
    return FALSE;
  }

  for ( i = 0; i < poly_ptr->n_points; i++ )
  {
    if (!is_binary)
    {
      char *p = &line[0];
      if (!fgets( line, sizeof(line), fp ))
      {
        print_error( "premature end of file parsing vertices.\n" );
        return FALSE;
      }
      for (j = 0; j < vertex_count; j++)
      {
        load_property_text( p, &p, &vertex_props[j] );
      }
    }
    else
    {
      for (j = 0; j < vertex_count; j++)
      {
        load_property_binary( fp, &vertex_props[j], swap_needed );
      }
    }

    get_property_element( prec_x, 0, &x );
    get_property_element( prec_y, 0, &y );
    get_property_element( prec_z, 0, &z );
    Point_x( poly_ptr->points[i] ) = x;
    Point_y( poly_ptr->points[i] ) = y;
    Point_z( poly_ptr->points[i] ) = z;

    if ( poly_ptr->colour_flag == PER_VERTEX_COLOURS )
    {
      int r, g, b;
      get_property_element( prec_r, 0, &r );
      get_property_element( prec_g, 0, &g );
      get_property_element( prec_b, 0, &b );

      poly_ptr->colours[i] = make_Colour(r, g, b);
    }
  }

  for (i = 0; i < poly_ptr->n_items; i++)
  {
    int n_elements;

    if (!is_binary)
    {
      char *p = &line[0];
      if (!fgets( line, sizeof(line), fp ))
      {
        print_error( "premature end of file parsing vertices.\n" );
        return FALSE;
      }

      for (j = 0; j < face_count; j++)
      {
        load_property_text( p, &p, &face_props[j] );
      }
    }
    else
    {
      for (j = 0; j < face_count; j++)
      {
        load_property_binary( fp, &face_props[j], swap_needed );
      }
    }

    n_elements = get_property_length( prec_vertex_indices );

    poly_ptr->end_indices[i] = n_indices + n_elements;

    for (j = 0; j < n_elements; j++)
    {
      int i_pt;
      get_property_element( prec_vertex_indices, j, &i_pt );
      if ( i_pt < 0 || i_pt >= poly_ptr->n_points )
      {
        print_error( "illegal vertex index in face.\n" );
      }
      ADD_ELEMENT_TO_ARRAY( poly_ptr->indices,
                            n_indices,
                            i_pt,
                            poly_ptr->n_items );

    }
  }

  for (i = 0; i < face_count; i++)
  {
    FREE( face_props[i].prop_name );
  }
  for (i = 0; i < vertex_count; i++)
  {
    FREE( vertex_props[i].prop_name );
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
input_ply_graphics_file(char *filename,
                        int *n_objects,
                        object_struct **object_list[])
{
  VIO_BOOL result = FALSE;
  FILE *fp;

  if ( open_file( filename, READ_FILE, BINARY_FORMAT, &fp ) == VIO_OK )
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
    close_file( fp );
  }
  return result;
}
