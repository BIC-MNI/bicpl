#include  <def_mni.h>

public  void  resample_volume(
    volume_struct  *src_volume,
    Transform      *dest_to_src_transform,
    volume_struct  *dest_volume )
{
    int         x, y, z, value;
    int         xi, yi, zi;
    int         v000, v001, v010, v011, v100, v101, v110, v111;
    Real        v00, v01, v10, v11, v0, v1;
    Real        xv, yv, zv;
    Real        u, v, w;
    Transform   transform;
    Vector      x_axis, y_axis, z_axis;
    Point       origin;
    Point       point;

    transform = dest_volume->voxel_to_world_transform;

    if( dest_to_src_transform != (Transform *) NULL )
        concat_transforms( &transform, dest_to_src_transform, &transform );

    concat_transforms( &transform, &src_volume->world_to_voxel_transform,
                       &transform );

    get_transform_origin( &transform, &origin );
    get_transform_x_axis( &transform, &x_axis );
    get_transform_y_axis( &transform, &y_axis );
    get_transform_z_axis( &transform, &z_axis );

    for_less( x, 0, dest_volume->sizes[X] )
    {
        for_less( y, 0, dest_volume->sizes[Y] )
        {
            fill_Point( point, (Real) x, (Real) y, 0.0 );
            transform_point( &transform, &point, &point );
            xv = Point_x(point);
            yv = Point_y(point);
            zv = Point_z(point);
            for_less( z, 0, dest_volume->sizes[Z] )
            {
                if( xv < 0.0 || xv >= (Real) (dest_volume->sizes[X]-1) ||
                    yv < 0.0 || yv >= (Real) (dest_volume->sizes[Y]-1) ||
                    zv < 0.0 || zv >= (Real) (dest_volume->sizes[Z]-1) )
                {
                    if( xv < -0.5 || xv >= (Real) dest_volume->sizes[X] - 0.5 ||
                        yv < -0.5 || yv >= (Real) dest_volume->sizes[Y] - 0.5 ||
                        zv < -0.5 || zv >= (Real) dest_volume->sizes[Z] - 0.5 )
                    {
                        value = 0;
                    }
                    else
                    {
                        value = GET_VOLUME_DATA( *src_volume, ROUND( xv ),
                                                 ROUND( yv ), ROUND( zv ) );
                    }
                }
                else
                {
                    xi = (int) xv;
                    yi = (int) yv;
                    zi = (int) zv;
                    u = FRACTION( xv );
                    v = FRACTION( yv );
                    w = FRACTION( zv );

                    v000 = GET_VOLUME_DATA( *src_volume, xi  , yi  , zi );
                    v001 = GET_VOLUME_DATA( *src_volume, xi  , yi  , zi+1 );
                    v010 = GET_VOLUME_DATA( *src_volume, xi  , yi+1, zi );
                    v011 = GET_VOLUME_DATA( *src_volume, xi  , yi+1, zi+1 );
                    v100 = GET_VOLUME_DATA( *src_volume, xi+1, yi  , zi );
                    v101 = GET_VOLUME_DATA( *src_volume, xi+1, yi  , zi+1 );
                    v110 = GET_VOLUME_DATA( *src_volume, xi+1, yi+1, zi );
                    v111 = GET_VOLUME_DATA( *src_volume, xi+1, yi+1, zi+1 );
                    
                    v00 = (Real) v000 + (Real) (v001 - v000) * w;
                    v01 = (Real) v010 + (Real) (v011 - v010) * w;
                    v10 = (Real) v100 + (Real) (v101 - v100) * w;
                    v11 = (Real) v110 + (Real) (v111 - v110) * w;

                    v0 = (Real) v00 + (Real) (v01 - v00) * v;
                    v1 = (Real) v10 + (Real) (v11 - v10) * v;

                    value = ROUND( (Real) v0 + (Real) (v1 - v0) * u );
                }

                ASSIGN_VOLUME_DATA( *dest_volume, x, y, z, value );

                xv += Point_x(z_axis);
                yv += Point_y(z_axis);
                zv += Point_z(z_axis);
            }
        }
print( "%d/%d\n", x+1, dest_volume->sizes[X] );
    }
}
