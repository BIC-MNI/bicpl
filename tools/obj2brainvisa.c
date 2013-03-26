
/*
   OBJ2BRAINVISA

   Convert a CLASP .obj to the BrainVisa ascii format.

   obj2brainvisa in.obj out.mesh

   Values: in.obj = CLASP input object file
           out.mesh = BrainVisa output object file

   By: Claude Lepage, April 2008.

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

#define BV_MODE_ASCII "ascii"
#define BV_MODE_BIGBINARY "binarABCD"
#define BV_MODE_LITTLEBINARY "binarDCBA"
#define BV_TEXTURETYPE "VOID"

// Prototypes of functions in this file.

static void usage( char * );

// Main program.

int main( int argc, char * argv[] ) {

  int      i, j;

  int n_objects;
  object_struct ** object;

  polygons_struct * surface;
  VIO_File_formats      format;
  VIO_STR            expanded;

  // Parse the command line arguments for the file names.

  if( argc != 3 ) {
    usage( argv[0] );
    return( 1 );
  }

  char * in_obj = argv[1];
  char * out_obj = argv[2];
  if( in_obj == NULL || out_obj == NULL ) {
    usage( argv[0] );
    return( 1 );
  }

  // Vectors to hold objects.

  // Read the surface file.
  expanded = expand_filename( in_obj );
  int err = input_graphics_file( expanded, &format, &n_objects, &object );

  if( err != OK ) {
    print_error( "Error reading file %s\n", expanded );
    return( 1 );
  }

  if( n_objects != 1 || 
      ( n_objects == 1 && get_object_type(object[0]) != POLYGONS ) ) {
    print_error( "Error in contents of file %s\n", expanded );
    return( 1 );
  }
  delete_string( expanded );

  surface = get_polygons_ptr( object[0] );

  int bv_polygon_dim = 3;   // triangles
  int bv_n_timesteps = 1;     // number of time frames
  int bv_timesteps = 0;     // number of time frames

  // Output surface in BrainVisa format

  FILE * fp = fopen( out_obj, "w+t" );

  fprintf( fp, "%s\n", BV_MODE_ASCII );
  fprintf( fp, "%s\n", BV_TEXTURETYPE );
  fprintf( fp, "%d\n", bv_polygon_dim );
  fprintf( fp, "%d\n", bv_n_timesteps );
  fprintf( fp, "%d\n", bv_timesteps );

  fprintf( fp, "%d\n", surface->n_points );
  for( i = 0; i < surface->n_points; i++ ) {
    fprintf( fp, "(%g,%g,%g)\n", surface->points[i].coords[0],
             surface->points[i].coords[1], surface->points[i].coords[2] );
  }

  fprintf( fp, "%d\n", surface->n_points );
  for( i = 0; i < surface->n_points; i++ ) {
    fprintf( fp, "(%g,%g,%g)\n", surface->normals[i].coords[0],
             surface->normals[i].coords[1], surface->normals[i].coords[2] );
  }

  fprintf( fp, "%d\n", 0 );  // no textures

  fprintf( fp, "%d\n", surface->n_items );
  j = 0;
  for( i = 0; i < surface->n_items; i++ ) {
    if( surface->end_indices[i] - j != 3 ) {
      fprintf( stderr, "Error: CLASP object must contain only triangles.\n" );
      return( 1 );
    }
    fprintf( fp, "(%d,%d,%d)\n", surface->indices[j], surface->indices[j+1],
             surface->indices[j+2] );
    j = surface->end_indices[i];
  }

  fclose( fp );

  delete_object_list( n_objects, object );

  return 0;
}


// -------------------------------------------------------------------
// Help message on how to use this module.
//
static void usage( char * executable_name ) {

  VIO_STR  usage_format = "\
Usage: %s in.obj out.mesh \n\
Values: in.obj = CLASP input object file\n\
        out.mesh = BrainVisa output surface file\n";

  print_error( usage_format, executable_name );
}

