#include  <bicpl.h>

#define  BINTREE_FACTOR  0.5

int  main(
    int   argc,
    char  *argv[] )
{
    VIO_STR               src_polygons_filename, dest_polygons_filename;
    VIO_STR               input_filename, output_filename;
    VIO_File_formats         format;
    int                  i, n_points;
    int                  n_src_objects, n_dest_objects, n_objects;
    VIO_Point                *points, point;
    VIO_Real                 x, y, z;
    object_struct        **objects, **src_objects, **dest_objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &src_polygons_filename ) ||
        !get_string_argument( NULL, &dest_polygons_filename ) )
    {
        fprintf(stderr,
             "Usage: %s  src_polygons  dest_polygons input_file output_file\n",
               argv[0] );
        fprintf(stderr,
             "        [input2.obj output2.obj] [input3.obj output3.obj]\n" );
        return( 1 );
    }

    if( input_graphics_file( src_polygons_filename,
                             &format, &n_src_objects, &src_objects ) != VIO_OK )
        return( 1 );

    if( input_graphics_file( dest_polygons_filename,
                             &format, &n_dest_objects, &dest_objects ) != VIO_OK )
        return( 1 );

    if( n_src_objects != 1 || get_object_type( src_objects[0] ) != POLYGONS ||
        n_dest_objects != 1 || get_object_type( dest_objects[0] ) != POLYGONS )
    {
        print( "Must specify polygons files.\n" );
        return( 1 );
    }

    create_polygons_bintree( get_polygons_ptr(src_objects[0]),
                             VIO_ROUND( (VIO_Real) get_polygons_ptr(
                                            src_objects[0])->n_items *
                                    BINTREE_FACTOR ) );

    while( get_string_argument( NULL, &input_filename ) )
    {
        if( equal_strings( input_filename, "-" ) )
        {
            if( !get_real_argument( 0.0, &x ) ||
                !get_real_argument( 0.0, &y ) ||
                !get_real_argument( 0.0, &z ) )
            {
                print( "Missing x y z\n" );
                return( 1 );
            }
            fill_Point( point, x, y, z );
            polygon_transform_points( get_polygons_ptr(src_objects[0]),
                                      get_polygons_ptr(dest_objects[0]),
                                      1, &point, &point );
            print( "Transformed point: %g %g %g\n",
                   RPoint_x( point ),
                   RPoint_y( point ),
                   RPoint_z( point ) );
        }
        else
        {
            if( !get_string_argument( NULL, &output_filename ) )
            {
                print( "Missing output filename\n" );
                return( 1 );
            }

            if( input_objects_any_format( NULL, input_filename, WHITE, 1.0,
                              SPHERE_MARKER, &n_objects, &objects ) != VIO_OK )

                return( 1 );

            for_less( i, 0, n_objects )
            {
                n_points = get_object_points( objects[i], &points );

                polygon_transform_points( get_polygons_ptr(src_objects[0]),
                                          get_polygons_ptr(dest_objects[0]),
                                          n_points, points, points );

                
                switch( get_object_type(objects[i]) )
                {
                case POLYGONS:
                    compute_polygon_normals( get_polygons_ptr(objects[i]) );
                    break;

                case QUADMESH:
                    compute_quadmesh_normals( get_quadmesh_ptr(objects[i]) );
                    break;

                default:
                    break;
                }
            }

            if( output_graphics_file( output_filename, ASCII_FORMAT, n_objects,
                                      objects ) != VIO_OK )
                return( 1 );

            delete_object_list( n_objects, objects );
        }
    }

    delete_object_list( n_src_objects, src_objects );
    delete_object_list( n_dest_objects, dest_objects );

    return( 0 );
}
