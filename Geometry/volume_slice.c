#include  <module.h>

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
