
/*
   OBJ_CONCAT

   Concatenate .obj and .txt files.

   objconcat in1.obj in2.obj [inX.obj] out.obj

   Values: in1.obj = first input object file
           in2.obj = second input object file
           inX.obj = more input object files
           in1.txt = "none" or data field for first input object file
           in2.txt = "none" or data field for second input object file
           inX.txt = "none" or data field for more input object files
           out.obj = output object file
           out.txt = "none" or output data field file

   By: Claude Lepage, December 2007.

   COPYRIGHT: McConnell Brain Imaging Center, 
              Montreal Neurological Institute,
              Department of Psychology,
              McGill University, Montreal, Quebec, Canada. 
  
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
*/

#include <stdio.h>
#include <volume_io.h>
#include <bicpl.h>

// Prototypes of functions in this file.

static void usage( char * );

// Main program.

int main( int argc, char * argv[] ) {

  int      i, j;
  int      n_input_objs;       // number of input objects

  int    * n_objects;
  object_struct  *** object_list;

  polygons_struct * surface;
  VIO_File_formats      format;
  VIO_STR            expanded;

  VIO_Point  * total_coords;       // concatenated coords
  VIO_Vector * total_normals;      // concatenated normals
  int    * total_indices;      // concatenated index pointers
  int    * total_connec;       // concatenated connectivity

  int      total_n_points;     // total number of points
  int      total_n_indices;    // total number of index pointers
  int      total_n_elems;      // total number of polygons

  FILE   * fp = NULL;

  // Parse the command line arguments for the file names.

  n_input_objs = argc/2 - 1;
  if( argc <= 1 || 2*n_input_objs+3 != argc ) {
    usage( argv[0] );
    return( 1 );
  }

  char * out_obj = argv[2*n_input_objs+1];
  char * out_txt = argv[2*n_input_objs+2];
  if( out_obj == NULL || out_txt == NULL ) {
    usage( argv[0] );
    return( 1 );
  }

  // Output file for data field.

  if( strcmp( out_txt, "none" ) != 0 ) {
    fp = fopen( out_txt, "w+t" );
  }

  // Vectors to hold objects.

  total_n_points = 0;
  total_n_indices = 0;
  total_n_elems = 0;
  n_objects = (int*)malloc( n_input_objs * sizeof( int ) );
  object_list = (object_struct***)malloc( n_input_objs * sizeof( object_struct **) );

  // Read the surfaces into memory.

  for( i = 0; i < n_input_objs; i++ ) {

    // Read the surface file.
    expanded = expand_filename( argv[i+1] );
    int err = input_graphics_file( expanded, &format, &n_objects[i],
                                   &object_list[i] );

    if( err != VIO_OK ) {
      fprintf(stderr, "Error reading file %s\n", expanded );
      return( 1 );
    }

    if( n_objects[i] != 1 || 
        ( n_objects[i] == 1 && get_object_type(object_list[i][0]) != POLYGONS ) ) {
      fprintf(stderr, "Error in contents of file %s\n", expanded );
      return( 1 );
    }
    delete_string( expanded );

    surface = get_polygons_ptr( object_list[i][0] );

    total_n_points += surface->n_points;
    total_n_elems += surface->n_items;
    total_n_indices += surface->end_indices[surface->n_items-1];
  }

  // Merge the surfaces

  total_coords = (VIO_Point*)malloc( total_n_points * sizeof( VIO_Point ) );
  total_normals = (VIO_Vector*)malloc( total_n_points * sizeof( VIO_Vector ) );
  total_indices = (int*)malloc( total_n_elems * sizeof( int ) );  // like this, yes!
  total_connec = (int*)malloc( total_n_indices * sizeof( int ) );

  int shift_indices = 0;
  int shift_connec = 0;
  int count_points = 0;
  int count_indices = 0;
  int count_connec = 0;
  for( i = 0; i < n_input_objs; i++ ) {
    surface = get_polygons_ptr( object_list[i][0] );
    for( j = 0; j < surface->n_points; j++ ) {
      total_coords[count_points].coords[0] = surface->points[j].coords[0];
      total_coords[count_points].coords[1] = surface->points[j].coords[1];
      total_coords[count_points].coords[2] = surface->points[j].coords[2];
      total_normals[count_points].coords[0] = surface->normals[j].coords[0];
      total_normals[count_points].coords[1] = surface->normals[j].coords[1];
      total_normals[count_points].coords[2] = surface->normals[j].coords[2];
      count_points++;
    }

    for( j = 0; j < surface->n_items; j++ ) {
      total_indices[count_indices] += surface->end_indices[j] + shift_indices;
      count_indices++;
    }
    for( j = 0; j < surface->end_indices[surface->n_items-1]; j++ ) {
      total_connec[count_connec] += surface->indices[j] + shift_connec;
      count_connec++;
    }

    shift_indices += surface->end_indices[surface->n_items-1];
    shift_connec += surface->n_points;

    // Concatenate the texture files for this object.

    if( fp ) {
      if( strcmp( argv[n_input_objs+i+1], "none" ) == 0 ) {
        for( j = 0; j < surface->n_points; j++ ) {
          fprintf( fp, "%g\n", 0.0 );
        }
      } else {
        char buf[80];
        FILE * fp2 = fopen( argv[n_input_objs+i+1], "rt" );
        if( fp2 != NULL ) {
          for( j = 0; j < surface->n_points; j++ ) {
            if (fscanf( fp2, "%s", buf ) == 1) {
              fprintf( fp, "%s\n", buf );
            }
          }
          fclose( fp2 );
        } else {
          for( j = 0; j < surface->n_points; j++ ) {
            fprintf( fp, "%g\n", 0.0 );
          }
        }
      }
    }
  }

  // Use the first object for the header information.

  surface = get_polygons_ptr( object_list[0][0] );

  free( surface->points );
  free( surface->normals );
  free( surface->end_indices );
  free( surface->indices );

  // Replace previous structures by concatenated ones.

  surface->n_points = total_n_points;
  surface->n_items = total_n_elems;
  surface->points = total_coords;
  surface->normals = total_normals;
  surface->end_indices = total_indices;
  surface->indices = total_connec;
 
  // ok, this could be done more efficiently if we don't want the merged surface. 
  if( strcmp( out_obj, "none" ) != 0 ) {
    expanded = expand_filename( out_obj );

    if( output_graphics_file( expanded, format, n_objects[0], object_list[0] ) != VIO_OK) {
      return( 1 );
    }

    delete_string( expanded );
  }

  if( fp ) {
    fclose( fp );
  }

  // Free the memory after usage. Note that total_coords and similar vectors
  // will be deleted in object_list[0].

  for( i = 0; i < n_input_objs; i++ ) {
    delete_object_list( n_objects[i], object_list[i] );
  }
  free( n_objects );
  free( object_list );

  return 0;
}


// -------------------------------------------------------------------
// Help message on how to use this module.
//
static void usage( char * executable_name ) {

  VIO_STR  usage_format = "\
Usage: %s in1.obj in2.obj [inX.obj] in1.txt in2.txt [inX.txt] out.obj out.txt\n\
Values: in1.obj = first input object file\n\
        in2.obj = second input object file\n\
        [inX.obj] = more input object files\n\
        in1.txt = \"none\" or data field for first input object file\n\
        in2.txt = \"none\" or data field for second input object file\n\
        [inX.txt] = \"none\" or data field for more input object files\n\
        out.obj = output object file\n\
        out.txt = \"none\" or output data field file\n\n";

  fprintf(stderr, usage_format, executable_name );
}

