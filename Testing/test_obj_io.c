#include  <internal_volume_io.h>
#include  <bicpl.h>

/* perform object I/O tests.
 */

#define ERROR fprintf(stderr, "%s %s %d\n", __FILE__, __func__, __LINE__ )

const char *
obj_type_to_string(Object_types object_type)
{
  switch (object_type)
  {
  case LINES: return "LINES";
  case MARKER: return "MARKER";
  case MODEL: return "MODEL";
  case PIXELS: return "PIXELS";
  case POLYGONS: return "POLYGONS";
  case QUADMESH: return "QUADMESH";
  case TEXT: return "TEXT";
  default: return "???????";
  }
}


void check_points( object_struct *object )
{
  VIO_Point * points;
  VIO_Real sum[3] = {0, 0, 0};
  int i;
  int n_points = get_object_points( object, &points );

  for (i = 0; i < n_points; i++)
  {
    sum[0] += Point_x( points[i] );
    sum[1] += Point_y( points[i] );
    sum[2] += Point_z( points[i] );
  }
  printf("%g %g %g ", sum[0]/n_points, sum[1]/n_points, sum[2]/n_points);
}


int print_objects(int depth, int n_objects, object_struct **objects)
{
  int n_errors = 0;
  int i, j, n;
  VIO_Colour *colours;
  Colour_flags *colour_flag;

  printf("n_objects: %d\n", n_objects);
  
  for (i = 0; i < n_objects; i++)
  {
    int type = get_object_type( objects[i] );
    printf( " %d: %s ", i, obj_type_to_string( type ) );
    if (type == POLYGONS)
    {
      polygons_struct *ptr = get_polygons_ptr( objects[i] );
      printf("n_items %d n_points %d ", ptr->n_items, ptr->n_points );
      check_points( objects[i] );
      colour_flag = get_object_colours( objects[i], &colours );
      if (*colour_flag == ONE_COLOUR)
      {
        printf("%x ", (unsigned int)colours[0] );
      }
      else if (*colour_flag == PER_VERTEX_COLOURS)
      {
        printf("%x ", (unsigned int) colours[ptr->n_points - 1]);
      }
      else if (*colour_flag == PER_ITEM_COLOURS)
      {
        printf("%x ", (unsigned int) colours[ptr->n_items - 1]);
      }
      else
      {
        ERROR;
        n_errors++;
      }
      /* Check indices */
      n = NUMBER_INDICES( *ptr );
      for (j = 0; j < n; j++)
      {
        if (ptr->indices[j] < 0 || ptr->indices[j] >= ptr->n_points)
        {
          ERROR;
          n_errors++;
        }
      }

      for (j = 1; j < ptr->n_items; j++)
      {
        if (ptr->end_indices[j] < 0 || 
            ptr->end_indices[j-1] >= ptr->end_indices[j])
        {
          ERROR;
          n_errors++;
        }
      }
    }
    else if (type == LINES)
    {
      lines_struct *ptr = get_lines_ptr( objects[i] );
      printf("n_items %d n_points %d ", ptr->n_items, ptr->n_points );
      check_points( objects[i] );
      colour_flag = get_object_colours( objects[i], &colours );
      if (*colour_flag == ONE_COLOUR)
      {
        printf("%x ", (unsigned int)colours[0] );
      }
      else if (*colour_flag == PER_VERTEX_COLOURS)
      {
        printf("%x ", (unsigned int) colours[ptr->n_points - 1]);
      }
      else if (*colour_flag == PER_ITEM_COLOURS)
      {
        printf("%x ", (unsigned int) colours[ptr->n_items - 1]);
      }
      else
      {
        ERROR;
        n_errors++;
      }
      /* Check indices */
      n = NUMBER_INDICES( *ptr );
      for (j = 0; j < n; j++)
      {
        if (ptr->indices[j] < 0 || ptr->indices[j] >= ptr->n_points)
        {
          ERROR;
          n_errors++;
        }
      }

      for (j = 1; j < ptr->n_items; j++)
      {
        if (ptr->end_indices[j] < 0 || 
            ptr->end_indices[j-1] >= ptr->end_indices[j])
        {
          ERROR;
          n_errors++;
        }
      }
    }
    else if (type == MARKER)
    {
      marker_struct *ptr = get_marker_ptr( objects[i] );
      printf("type %d size %g position %g %g %g", ptr->type, ptr->size,
             Point_x(ptr->position),
             Point_y(ptr->position),
             Point_z(ptr->position));
    }
    else if (type == TEXT)
    {
      text_struct *ptr = get_text_ptr( objects[i] );
      printf("font %d size %g origin %g %g %g '%s'", ptr->font, ptr->size,
             Point_x(ptr->origin),
             Point_y(ptr->origin),
             Point_z(ptr->origin),
             ptr->string);
      
    }
    else if (type == QUADMESH)
    {
      quadmesh_struct *ptr = get_quadmesh_ptr( objects[i] );
      printf("m %d n %d m_closed %d n_closed %d", 
             ptr->m, ptr->n, ptr->m_closed, ptr->n_closed);
      check_points( objects[i] );
    }
    else if (type == PIXELS)
    {
      pixels_struct *ptr = get_pixels_ptr( objects[i] );
      long psum = 0;
      printf("type %d position %d %d size %d %d zoom %g %g ", 
             ptr->pixel_type,
             ptr->x_position, ptr->y_position,
             ptr->x_size, ptr->y_size,
             ptr->x_zoom, ptr->y_zoom);
      n = ptr->x_size * ptr->y_size;
      for (i = 0; i < n; i++)
      {
        switch (ptr->pixel_type)
        {
        case RGB_PIXEL:
          psum += (unsigned int) ptr->data.pixels_rgb[i];
          break;
        case COLOUR_INDEX_8BIT_PIXEL:
          psum += ptr->data.pixels_8bit_colour_index[i];
          break;
        case COLOUR_INDEX_16BIT_PIXEL:
          psum += ptr->data.pixels_16bit_colour_index[i];
          break;
        default:
          ERROR;
          n_errors++;
          break;
        }
      }
      printf("psum %ld ", psum);
    }
    else if (type == MODEL)
    {
      model_struct *ptr = get_model_ptr( objects[i] );
      print_objects( depth + 1, ptr->n_objects, ptr->objects );
    }
    else
    {
      ERROR;
      n_errors++;
    }
    printf("\n");
  }
  return n_errors;
}

int
main(int argc, char **argv)
{
  object_struct **objects;
  int n_objects = 0;
  VIO_Status status;
  int n_errors = 0;

  status = input_objects_any_format( NULL, argv[1],
                                     GREEN, 1, BOX_MARKER,
                                     &n_objects,
                                     &objects);

  if ( status != VIO_OK )
  {
    ERROR;
    n_errors++;
  }
  n_errors += print_objects(0, n_objects, objects);
  return n_errors;
}

