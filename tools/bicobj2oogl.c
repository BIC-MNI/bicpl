/**
 * Convert a David MacDonald .obj file to a Geomview OOGL file.
 **/

#include <stdio.h>

#include <volume_io.h>
#include <bicpl.h>


FILE* out = NULL;


void polygons_to_off( const polygons_struct* p )
{
    int n_vertices = p->n_points;
    int n_faces = p->n_items;
    int n_edges = -1;
    int i,e,f;

    fprintf( out, "# A polygon\n" );
    fprintf( out, "FALSE\n\n" );

    fprintf( out, "# NVertices NFaces NEdges\n" );
    fprintf( out, "#(Geomview does not check these numbers currently,"
	          " so we don't bother computing NEdges)\n" );
    fprintf( out, "%d %d %d\n\n", n_vertices, n_faces, n_edges );

    fprintf( out, "# Coordinates of the %d vertices, one per line.\n", n_vertices );
    for( i = 0; i < n_vertices; ++i ) {
	fprintf( out, "%f %f %f\n", 
		 Point_x( p->points[i] ), 
		 Point_y( p->points[i] ), 
		 Point_z( p->points[i] ) );
    }
    fprintf( out, "\n");

    fprintf( out, "# Vertex indices for each face\n" );
    fprintf( out, "# NVertices for the face, v[0] v[1] ...\n" );
    for ( f = 0; f < n_faces; ++f ) {
	int n_face_edges = GET_OBJECT_SIZE( *p, f );

	fprintf( out, "%d ", n_face_edges );
	for ( e = 0; e < n_face_edges; ++e ) {
	    int index = p->indices[POINT_INDEX(p->end_indices, f, e )];
	    fprintf( out,"%d ", index );
	}
	fprintf( out, "\n");
    }
    fprintf( out, "\n");
}


void process_file( char* filename ) 
{
    VIO_File_formats format;
    int i, num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, &format, 
			      &num_objects, &object_list ) != VIO_OK ) {
	fprintf( stderr, "input_graphics_file( %s ) failed.\n", filename );
	return;
    }

    fprintf( out, "LIST\n\n" );

    for ( i = 0; i < num_objects; ++i ) {
	Object_types ot = (object_list[i])->object_type;

	fprintf( out, "# Object %d of %d\n{\n", i+1, num_objects );

	if ( ot == POLYGONS )
	    polygons_to_off( &((object_list[i])->specific.polygons) );
	else
	    fprintf( stderr, "*** Unable to handle object type %d\n", ot );

	fprintf( out, "}\n\n" );
    }
}
  


void usage( char* argv0 )
{
    fprintf( stderr, "usage: %s input [output]\n", argv0 );
    fprintf( stderr, "\tOutput an OOGL file.\n" );
}


int main( int ac, char** av ) 
{
    out = stdout;

    if ( ac == 3 ) {
	out = fopen(av[2],"w");
	if ( out == NULL ) {
	    fprintf( stderr, "Cannot open \"%s\" for output.\n", av[2] );
	    return 1;
	}
    } else if ( ac != 2 ) {
	usage( av[0] );
	return 1;
    }

    fprintf( out, "## File '%s' converted to OOGL format\n", av[1] );
    fprintf( out, "## by bicobj2oogl $Id: bicobj2oogl.c,v 1.2 2001-06-06 01:49:58 stever Exp $\n\n" );

    process_file( av[1] );

    return 0;
}


