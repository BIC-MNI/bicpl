
#include  <def_mni.h>

private  Real  calculate_weight(
    int      x,
    Real     dx,
    Real     x_min,
    Real     x_max );

public  void  smooth_resample_volume(
    volume_struct       *volume,
    int                 new_nx,
    int                 new_ny,
    int                 new_nz,
    volume_struct       *resampled_volume )
{
    int         nx, ny, nz;
    int         x, y, z, xv, yv, zv;
    Real        x_min, x_max, y_min, y_max, z_min, z_max;
    Real        dx, dy, dz;
    Real        x_weight, xy_weight, xyz_weight;
    Real        val;
    Boolean     voxel_valid;
    Transform   scale_transform;

    get_volume_size( volume, &nx, &ny, &nz );

    if( new_nx <= 0 )
        new_nx = nx;

    if( new_ny <= 0 )
        new_ny = ny;

    if( new_nz <= 0 )
        new_nz = nz;

    *resampled_volume = *volume;

    resampled_volume->sizes[X] = new_nx;
    resampled_volume->sizes[Y] = new_ny;
    resampled_volume->sizes[Z] = new_nz;

    dx = (Real) nx / (Real) new_nx;
    dy = (Real) ny / (Real) new_ny;
    dz = (Real) nz / (Real) new_nz;

    resampled_volume->thickness[X] *= dx;
    resampled_volume->thickness[Y] *= dy;
    resampled_volume->thickness[Z] *= dz;

    make_scale_transform( dx, dy, dz, &scale_transform );
    concat_transforms( &resampled_volume->voxel_to_world_transform,
                       &scale_transform,
                       &resampled_volume->voxel_to_world_transform );
    compute_transform_inverse( &volume->voxel_to_world_transform,
                               &volume->world_to_voxel_transform );

    alloc_volume( resampled_volume );
    alloc_auxiliary_data( resampled_volume );

    for_less( x, 0, new_nx )
    {
        x_min = (Real)   x    / dz;
        x_max = (Real)  (x+1) / dz;

        for_less( y, 0, new_ny )
        {
            y_min = (Real)  y    / dy;
            y_max = (Real) (y+1) / dy;

            for_less( z, 0, new_nz )
            {
                z_min = (Real)  z    / dz;
                z_max = (Real) (z+1) / dz;

                val = 0.0;

                voxel_valid = FALSE;

                for_inclusive( xv, (int) x_min, (int) x_max )
                {
                    x_weight = calculate_weight( xv, dx, x_min, x_max );

                    for_inclusive( yv, (int) y_min, (int) y_max )
                    {
                        xy_weight = x_weight *
                                    calculate_weight( yv, dy, y_min, y_max );
                        for_inclusive( zv, (int) z_min, (int) z_max )
                        {
                            xyz_weight = xy_weight *
                                     calculate_weight( zv, dz, z_min, z_max );

                            if( xyz_weight > 0.0 )
                            {
                                val += xyz_weight *
                                       (Real) GET_VOLUME_DATA(*volume,xv,yv,zv);
                                if( get_voxel_activity_flag(volume,xv,yv,zv) )
                                    voxel_valid = TRUE;
                            }
                        }
                    }
                }

                ASSIGN_VOLUME_DATA(*resampled_volume,x,y,z, val + 0.5 );

                set_voxel_activity_flag( resampled_volume, x, y, z,
                                         voxel_valid );
            }
        }
    }
}

private  Real  calculate_weight(
    int      x,
    Real     dx,
    Real     x_min,
    Real     x_max )
{
    Real   start, end, weight;

    start = MAX( x_min, (Real) x );
    end = MIN( x_max, (Real) (x+1) );

    if( end < start || end - start > 1.0 )
    {
        HANDLE_INTERNAL_ERROR( "calculate_weight" );
    }

    weight = dx * (end - start);

    return( weight );
}
