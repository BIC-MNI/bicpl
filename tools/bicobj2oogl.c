/**
 * Convert a David MacDonald .obj file to a Geomview OOGL file.
 **/

#include <stdio.h>

#include <volume_io.h>
#include <bicpl.h>




void polygons_to_off( const polygons_struct* p )
{
    int n_vertices = p->n_points;
    int n_faces = p->n_items;
    int n_edges = -1;
    int i,e,f;

    printf( "# A polygon\n" );
    printf( "OFF\n\n" );

    printf( "# NVertices NFaces NEdges\n" );
    printf( "#(Geomview does not check these numbers currently, so we don't bother"
	    " computing NEdges)\n" );
    printf( "%d %d %d\n\n", n_vertices, n_faces, n_edges );

    printf( "# Coordinates of the %d vertices, one per line.\n", n_vertices );
    for( i = 0; i < n_vertices; ++i ) {
	printf( "%f %f %f\n", 
		Point_x( p->points[i] ), Point_y( p->points[i] ), Point_z( p->points[i] ) );
    }
    printf("\n");

    printf( "# Vertex indices for each face\n" );
    printf( "# NVertices for the face, v[0] v[1] ...\n" );
    for ( f = 0; f < n_faces; ++f ) {
	int n_face_edges = GET_OBJECT_SIZE( *p, f );

	printf( "%d ", n_face_edges );
	for ( e = 0; e < n_face_edges; ++e ) {
	    int index = p->indices[POINT_INDEX(p->end_indices, f, e )];
	    printf("%d ", index );
	}
	printf("\n");
    }
    printf("\n");
}


void process_file( char* filename ) 
{
    File_formats format;
    int i, num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, &format, 
			      &num_objects, &object_list ) != OK ) {
	fprintf( stderr, "input_graphics_file( %s ) failed.\n", filename );
	return;
    }

    printf( "LIST\n\n" );

    for ( i = 0; i < num_objects; ++i ) {
	Object_types ot = (object_list[i])->object_type;

	printf( "# Object %d of %d\n{\n", i+1, num_objects );

	if ( ot == POLYGONS )
	    polygons_to_off( &((object_list[i])->specific.polygons) );
	else
	    fprintf( stderr, "*** Unable to handle object type %d\n", ot );

	printf( "}\n\n" );
    }
}
  


void usage( char* argv0 )
{
    fprintf( stderr, "usage: %s file[.obj]\n", argv0 );
    fprintf( stderr, "\tOutput an OOGL file.\n" );
}


int main( int ac, char** av ) 
{
    if ( ac != 2 ) {
	usage( av[0] );
	return 1;
    }

    printf( "## File '%s' converted to OOGL format\n", av[1] );
    printf( "## by bicobj2oogl $Id: bicobj2oogl.c,v 1.1 2001-02-12 18:09:48 stever Exp $\n\n" );

    process_file( av[1] );

    return 0;
}


