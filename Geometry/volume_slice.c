#include  <internal_volume_io.h>
#include  <vols.h>
#include  <geom.h>

public  void   create_slice_quadmesh(
    Volume           volume,
    int              axis_index,
    Real             voxel_position,
    int              x_tess,
    int              y_tess,
    Real             x_min,
    Real             x_max,
    Real             y_min,
    Real             y_max,
    quadmesh_struct  *quadmesh )
{
    Point            point;
    Vector           normal;
    Real             x_w, y_w, z_w;
    int              sizes[N_DIMENSIONS];
    Real             voxel[N_DIMENSIONS];
    int              x, y, x_axis, y_axis;
    Surfprop         spr;

    x_axis = (axis_index + 1) % N_DIMENSIONS;
    y_axis = (axis_index + 2) % N_DIMENSIONS;
    get_volume_sizes( volume, sizes );

    if( x_tess <= 1 )
        x_tess = MAX( 2, sizes[x_axis] );
    if( y_tess <= 1 )
        y_tess = MAX( 2, sizes[y_axis] );

    get_default_surfprop( &spr );
    initialize_quadmesh( quadmesh, WHITE, &spr, x_tess, y_tess );

    voxel[axis_index] = voxel_position;

    fill_Vector( normal, 0.0, 0.0, 0.0 );
    Vector_coord( normal, axis_index ) = 1.0;

    if( x_min >= x_max )
    {
        x_min = 0.0;
        x_max = (Real) (sizes[x_axis] - 1);
    }

    if( y_min >= y_max )
    {
        y_min = 0.0;
        y_max = (Real) (sizes[y_axis] - 1);
    }

    for_less( x, 0, x_tess )
    {
        voxel[x_axis] = x_min + (x_max - x_min) * (Real) x / (Real) (x_tess-1);
        for_less( y, 0, y_tess )
        {
            voxel[y_axis] = y_min + (y_max - y_min) * (Real)y/(Real)(y_tess-1);

            convert_voxel_to_world( volume, voxel, &x_w, &y_w, &z_w );
            fill_Point( point, x_w, y_w, z_w );
            set_quadmesh_point( quadmesh, x, y, &point, &normal );
        }
    }
}

public  void   create_slice_3d(
    Volume           volume,
    Point            *origin,
    Vector           *normal,
    polygons_struct  *polygons )
{
    int              i, n_points;
    Point            point;
    Real             xw, yw, zw;
    Real             voxel[MAX_DIMENSIONS];
    Real             origin_voxel[MAX_DIMENSIONS];
    Real             y_axis[MAX_DIMENSIONS];
    Real             x_axis[MAX_DIMENSIONS];
    Real             outline[6][MAX_DIMENSIONS];
    Vector           v1, v2;

    create_two_orthogonal_vectors( normal, &v1, &v2 );

    convert_world_vector_to_voxel( volume,
                                   (Real) Vector_x(v1),
                                   (Real) Vector_y(v1),
                                   (Real) Vector_z(v1),
                                   x_axis );
    convert_world_vector_to_voxel( volume,
                                   (Real) Vector_x(v2),
                                   (Real) Vector_y(v2),
                                   (Real) Vector_z(v2),
                                   y_axis );

    convert_world_to_voxel( volume,
                            (Real) Point_x(*origin), (Real) Point_y(*origin),
                            (Real) Point_z(*origin), origin_voxel );

    n_points = get_volume_cross_section( volume, origin_voxel, x_axis, y_axis,
                                         outline );

    initialize_polygons( polygons, WHITE, NULL );

    for_less( i, 0, n_points )
    {
        voxel[0] = outline[i][0];
        voxel[1] = outline[i][1];
        voxel[2] = outline[i][2];

        convert_voxel_to_world( volume, voxel, &xw, &yw, &zw );

        fill_Point( point, xw, yw, zw );

        add_point_to_polygon( polygons, &point, normal );
    }
}
