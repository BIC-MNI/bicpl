/**
 * Convert a David MacDonald .obj file to a PLY file.
 **/

#include <stdio.h>

#include <volume_io.h>
#include <bicpl.h>

#include "ply.h"



typedef struct Vertex {
  float x,y,z;            /* position */
  float nx,ny,nz;         /* surface normal */
} Vertex;

typedef struct Face {
  unsigned char nverts;    /* number of vertex indices in list */
  int *verts;              /* vertex index list */
} Face;


STRING comment1;
STRING comment2;


void write_polygons( const polygons_struct* p )
{
    static char* element_names[] = { "vertex", "face" };

    static PlyProperty vertex_prop[] = {
	{"x", Float32, Float32, offsetof(Vertex,x), 0, 0, 0, 0},
	{"y", Float32, Float32, offsetof(Vertex,y), 0, 0, 0, 0},
	{"z", Float32, Float32, offsetof(Vertex,z), 0, 0, 0, 0},
	{"nx", Float32, Float32, offsetof(Vertex,nx), 0, 0, 0, 0},
	{"ny", Float32, Float32, offsetof(Vertex,ny), 0, 0, 0, 0},
	{"nz", Float32, Float32, offsetof(Vertex,nz), 0, 0, 0, 0},
    };

    static PlyProperty face_prop[] = {
	{"vertex_indices", Int32, Int32, offsetof(Face,verts),
	 1, Uint8, Uint8, offsetof(Face,nverts)},
    };

    PlyFile* ply;
    int n_vertices = p->n_points;
    int n_faces = p->n_items;
    int i,j;


    /* Declare PLY file that has 2 elements; use ascii format */
    ply = write_ply( stdout, 2, element_names, PLY_ASCII );

    append_comment_ply( ply, comment1 );
    append_comment_ply( ply, comment2 );

    /* First set of elements in the file is the vertices. */
    /* Describe properties of each vertex element */
    describe_element_ply( ply, "vertex", n_vertices );
    describe_property_ply( ply, &vertex_prop[0] );
    describe_property_ply( ply, &vertex_prop[1] );
    describe_property_ply( ply, &vertex_prop[2] );

    /* Second set of elements in the file is the faces. */
    /* Describe properties of each face element. */
    describe_element_ply( ply, "face", n_faces );
    describe_property_ply( ply, &face_prop[0] );

    header_complete_ply( ply );

    /* Write out vertices */
    put_element_setup_ply( ply, "vertex" );
    for( i = 0; i < n_vertices; ++i ) {
	Vertex v = { Point_x( p->points[i] ), Point_y( p->points[i] ), Point_z( p->points[i] ),
		     0, 0, 0 };
	put_element_ply( ply, &v );
    }

    /* Write out faces */
    put_element_setup_ply( ply, "face" );
    for ( i = 0; i < n_faces; ++i ) {
	Face f;
	f.nverts = GET_OBJECT_SIZE( *p, i );
	f.verts = &(p->indices[START_INDEX(p->end_indices, i)]);
	put_element_ply( ply, &f );
    }

    close_ply( ply );
    free_ply( ply );
}


void process_file( char* filename ) 
{
    File_formats format;
    int i, num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, &format, 
			      &num_objects, &object_list ) != VIO_OK ) {
	fprintf( stderr, "input_graphics_file( %s ) failed.\n", filename );
	return;
    }

    for ( i = 0; i < num_objects; ++i ) {
	Object_types ot = (object_list[i])->object_type;

	if ( ot == POLYGONS ) {
	    write_polygons( &((object_list[i])->specific.polygons) );
	    break; /* PLY files contain only one object */
	}
	else
	    fprintf( stderr, "*** Unable to handle object type %d\n", ot );

    }
}
  


void usage( char* argv0 )
{
    fprintf( stderr, "usage: %s file[.obj]\n", argv0 );
    fprintf( stderr, "\tOutput an PLY file.\n" );
}


int main( int ac, char** av ) 
{
    if ( ac != 2 ) {
	usage( av[0] );
	return 1;
    }

    comment1 = concat_strings
	( create_string( "From BIC file: " ), create_string( av[1] ) );
    comment2 = create_string
	( "Converted by bicobj2ply $Id: bicobj2ply.c,v 1.1 2001-02-12 18:09:48 stever Exp $" );

    process_file( av[1] );

    return 0;
}


