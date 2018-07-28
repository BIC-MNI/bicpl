/* Expands MNI object points from a surface, along the surface normal, by a
   input scaling factor.

   Author: Gabriel A. Devenyi <gdevenyi@gmail.com>
*/
#define HAVE_MINC2 1

#include <bicpl.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  VIO_File_formats    format;
  object_struct   **objects;
  int             n_objects, n_points, i;
  polygons_struct *polygons;
  VIO_STR          obj_filename, output_filename;
  VIO_Point           *points;
  VIO_Real          step;

  /* get input arguments */
  initialize_argument_processing(argc, argv);
  if (!get_string_argument(NULL, &obj_filename) ||
      !get_real_argument( 0.0, &step ) ||
      !get_string_argument(NULL, &output_filename)) {
    fprintf(stderr, "Usage: expand_object_along_surface_normal object.obj normal_multiple output.obj\n");
    return(1);
  }

  /* open the obj file */
  if (input_graphics_file(obj_filename,
			  &format, &n_objects, &objects ) != VIO_OK ) {
        return( 1 );
  }

  /* make sure data is polygons */
  if( n_objects != 1 || get_object_type(objects[0]) != POLYGONS ) {
        fprintf(stderr, "File must contain exactly 1 polygons struct.\n" );
        return( 1 );
  }

  polygons = get_polygons_ptr(objects[0]);
  n_points = get_object_points(objects[0], &points);


  for (i = 0; i < n_points; i++) {

    Point_x(polygons->points[i]) +=  Vector_x(polygons->normals[i])*step;
    Point_y(polygons->points[i]) +=  Vector_y(polygons->normals[i])*step;
    Point_z(polygons->points[i]) +=  Vector_z(polygons->normals[i])*step;
  }
  output_graphics_file( output_filename, format, 1, objects );
  return(0);
}
