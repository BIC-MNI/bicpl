/**
 * Convert a David MacDonald .obj file to a VTK-format file.
 *
 * TODO: 
 * - Generalize to allow multiple attributes of the same
 *   kind (e.g. POINT_DATA scalars).
 * - check for space in attribute name
 **/

#include <stdio.h>

#include <volume_io.h>
#include <bicpl.h>
#include "ParseArgv.h"


/* Size of structure */
int n_points = -1;
int n_cells = -1;


/* Array of string pairs specifying the data attributes. 
   I'm being lazy and using static-sized arrays.  
   The standard C++ vector<> would be so helpful here ...
 */
#define MAX_ATTRIBUTES  16
typedef struct {
    int count;
    char* data_name[MAX_ATTRIBUTES];
    char* file_name[MAX_ATTRIBUTES];
} AttributeArray;


AttributeArray vertex_scalars;
AttributeArray vertex_vectors;
AttributeArray facet_scalars;
AttributeArray facet_vectors;


int append_att_array( char* dst, char* key, int argc, char** argv );


ArgvInfo argTable[] = {
    { "-vertex_scalars", 
      ARGV_GENFUNC, (char*)append_att_array, (char*)&vertex_scalars,
      "specify name and filename of scalar vertex data" },
    { "-vertex_vectors",
      ARGV_GENFUNC, (char*)append_att_array, (char*)&vertex_vectors,
      "specify name and filename of vector vertex data" },
    { "-facet_scalars",
      ARGV_GENFUNC, (char*)append_att_array, (char*)&facet_scalars,
      "specify name and filename of scalar facet data" },
    { "-facet_vectors",
      ARGV_GENFUNC, (char*)append_att_array, (char*)&facet_vectors,
      "specify name and filename of vector facet data" },
    { NULL, ARGV_END, NULL, NULL, NULL }
};


/* Pull out next two parameters from the command line, stuff
   them into the AttributeArray passed in "dst", and compact
   the remaining arguments.
*/
int append_att_array( char* dst, char* key, int argc, char** argv )
{
    AttributeArray* aa = (AttributeArray*)dst;
    int i;

    if ( argc < 2 ) {
	fprintf( stderr, "%s requires two arguments\n", key );
	return -1;
    }

    if ( aa->count >= MAX_ATTRIBUTES ) {
	fprintf( stderr, "too many attributes of type %s\n", key );
	fprintf( stderr, "Ask a wizard to expand me.\n" );
	return -1;
    }

    aa->data_name[aa->count] = argv[0];
    aa->file_name[aa->count] = argv[1];
    ++aa->count;

    for( i = 0; i < argc-2; ++i )
	argv[i] = argv[i+2];

    return argc-2;
}
    

/* Copy a file of N float values, one per line.
 */
void copy_scalars( FILE* out, 
		   const char* data_name, const char* file_name, 
		   int N )
{
    FILE* in = fopen( file_name, "r" );
    float data;
    int count = 0;

    if ( in == NULL ) {
	fprintf( stderr, "Cannot open scalars file \"%s\" -- skipping.\n",
		 file_name );
	return;
    }

    fprintf( out, "SCALARS %s float\n", data_name );
    fprintf( out, "LOOKUP_TABLE default\n" );
    while ( !feof(in) && (fscanf(in,"%f\n",&data) == 1 )) {
	fprintf( out, "%f\n", data );
	++count;
    }
    fclose(in);

    if ( count != N ) {
	fprintf( stderr, "Expected %d scalars, but read %d from \"%s\".\n",
		 N, count, file_name );
    }
}



/* Copy a file of 3N float values, three per line.
 */
void copy_vectors( FILE* out, 
		   const char* data_name, const char* file_name, 
		   int N )
{
    FILE* in = fopen( file_name, "r" );
    float v0,v1,v2;
    int count = 0;

    if ( in == NULL ) {
	fprintf( stderr, "Cannot open vectors file \"%s\" -- skipping.\n",
		 file_name );
	return;
    }

    fprintf( out, "VECTORS %s float\n", data_name );
    while ( !feof(in) && (fscanf(in,"%f %f %f\n", &v0,&v1,&v2) == 3 )) {
	fprintf( out, "%f %f %f\n", v0,v1,v2 );
	++count;
    }
    fclose(in);

    if ( count != N ) {
	fprintf( stderr, "Expected %d vectors, but read %d from \"%s\".\n",
		 N, count, file_name );
    }
}


void write_points_section( FILE* out, int n_points, Point* points )
{
    int i;

    fprintf( out, "POINTS %d float\n", n_points );
    for( i = 0; i < n_points; ++i ) {
	fprintf( out, "%f %f %f\n", 
		 Point_x( points[i] ), 
		 Point_y( points[i] ), 
		 Point_z( points[i] ) );
    }
}


void lines_to_vtk( FILE* out, const lines_struct* l )
{
    int e, size_cell_list = 0;

    n_points = l->n_points;
    n_cells = l->n_items;

    write_points_section( out, l->n_points, l->points );

    for( e = 0; e < n_cells; ++e ) 
	size_cell_list += 1 + GET_OBJECT_SIZE( *l, e );

    fprintf( out, "LINES %d %d\n", n_cells, size_cell_list );
    for ( e = 0; e < n_cells; ++e ) {
	int n_line_points = GET_OBJECT_SIZE( *l, e );
	int p;

	fprintf( out, "%d", n_line_points );
	for ( p = 0; p < n_line_points; ++p ) {
	    int index = l->indices[POINT_INDEX(l->end_indices, e, p )];
	    fprintf( out," %d", index );
	}
	fprintf( out, "\n");
    }

}


void polygons_to_vtk( FILE* out, const polygons_struct* p )
{
    int size_cell_list = 0;
    int f;

    n_points = p->n_points;
    n_cells = p->n_items;

    write_points_section( out, p->n_points, p->points );

    /* The cell list is the list of vertices of each cell, i.e.
       it is a face cycle.  For each cell, the list is given using
       N+1 integers: first the integer N is written, followed by
       N vertex indices. 

       We need to compute the total size of this cell list.
    */
    for( f = 0; f < n_cells; ++f ) 
	size_cell_list += 1 + GET_OBJECT_SIZE( *p, f );

    fprintf( out, "POLYGONS %d %d\n", n_cells, size_cell_list );
    for ( f = 0; f < n_cells; ++f ) {
	int n_facet_edges = GET_OBJECT_SIZE( *p, f );
	int e;

	fprintf( out, "%d", n_facet_edges );
	for ( e = 0; e < n_facet_edges; ++e ) {
	    int index = p->indices[POINT_INDEX(p->end_indices, f, e )];
	    fprintf( out," %d", index );
	}
	fprintf( out, "\n");
    }
}


void process_file( FILE* out, char* filename ) 
{
    File_formats format;
    int i, num_objects;
    object_struct** object_list;

    if ( input_graphics_file( filename, &format, 
			      &num_objects, &object_list ) != OK ) {
	fprintf( stderr, "input_graphics_file( %s ) failed.\n", filename );
	return;
    }

    if ( num_objects != 1 ) {
	fprintf( stderr, "Can only handle 1 object per file (has %d)\n",
		 num_objects );
	exit(1);
    }

    for ( i = 0; i < num_objects; ++i ) {
	Object_types ot = get_object_type(object_list[i]);

	if ( ot == LINES )
	    lines_to_vtk( out, get_lines_ptr(object_list[i]) );
	else if ( ot == POLYGONS )
	    polygons_to_vtk( out, get_polygons_ptr(object_list[i]) );
	else
	    fprintf( stderr, "*** Unable to handle object type %d\n", ot );
    }
}
  


void usage( char* argv0 )
{
    fprintf( stderr, "usage: %s [options] input [output]\n", argv0 );
    fprintf( stderr, "\tOutput a VTK file.\n" );
}


int main( int ac, char** av ) 
{
    FILE* out = stdout;

    vertex_scalars.count = 0;
    vertex_vectors.count = 0;
    facet_scalars.count = 0;
    facet_vectors.count = 0;

    if ( ParseArgv( &ac, av, argTable, 0 ) ) {
	usage( av[0] );
	return 0;
    }

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

    /* Spit out the header */
    fprintf( out, "# vtk DataFile Version 2.0\n" );
    fprintf( out, "File '%s' (converted by bicobj2vtk $Id: bicobj2vtk.c,v 1.3 2002-11-27 22:48:44 stever Exp $\n", av[1] );
    fprintf( out, "ASCII\n" );
    fprintf( out, "DATASET POLYDATA\n" );
    
    process_file( out, av[1] );

    /* Add attributes, if given. */
    if ( vertex_scalars.count > 0 || vertex_vectors.count > 0 ) {
	int a;

	fprintf( out, "POINT_DATA %d\n", n_points );

	for( a = 0; a < vertex_scalars.count; ++a )
	    copy_scalars( out, 
			  vertex_scalars.data_name[a],
			  vertex_scalars.file_name[a],
			  n_points );

	for( a = 0; a < vertex_vectors.count; ++a )
	    copy_vectors( out, 
			  vertex_vectors.data_name[a],
			  vertex_vectors.file_name[a],
			  n_points );
    }

    if ( facet_scalars.count > 0 || facet_vectors.count > 0 ) {
	int a;

	fprintf( out, "CELL_DATA %d\n", n_cells );

	for( a = 0; a < facet_scalars.count; ++a )
	    copy_scalars( out, 
			  facet_scalars.data_name[a],
			  facet_scalars.file_name[a],
			  n_cells );

	for( a = 0; a < facet_vectors.count; ++a )
	    copy_vectors( out, 
			  facet_vectors.data_name[a],
			  facet_vectors.file_name[a],
			  n_cells );
    }


    return 0;
}


