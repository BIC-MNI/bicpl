
#include  <mni.h>

public  void  get_mapping(
    Volume          volume,
    Real            origin[],
    Real            x_axis[],
    Real            y_axis[],
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            pix_origin[],
    Real            pix_x_axis[],
    Real            pix_y_axis[] )
{
    int   c, n_dims;
    Real  separations[MAX_DIMENSIONS];

    n_dims = get_volume_n_dimensions( volume );
    get_volume_separations( volume, separations );

    for_less( c, 0, n_dims )
    {
        pix_x_axis[c] = x_axis[c] / (x_scale * separations[c]);
        pix_y_axis[c] = y_axis[c] / (y_scale * separations[c]);
        pix_origin[c] = origin[c]
                        - pix_x_axis[c] * x_translation
                        - pix_y_axis[c] * y_translation;
    }
}

private  void  map_pixel_to_voxel(
    Real   x_pixel,
    Real   y_pixel,
    int    n_dims,
    Real   voxel_origin[],
    Real   voxel_x_axis[],
    Real   voxel_y_axis[],
    Real   voxel[] )
{
    int   c;

    for_less( c, 0, n_dims )
    {
        voxel[c] = voxel_origin[c] + (Real) x_pixel * voxel_x_axis[c] +
                                     (Real) y_pixel * voxel_y_axis[c];
    }
}

private  Real   dot(
    int    n,
    Real   v1[],
    Real   v2[] )
{
    int   i;
    Real  d;

    d = 0;
    for_less( i, 0, n )
        d += v1[i] * v2[i];

    return( d );
}

private  void   cross(
    int    n,
    Real   v1[],
    Real   v2[],
    Real   cr[] )
{
    int   c, a1, a2;

    for_less( c, 0, n )
    {
        a1 = (c + 1) % N_DIMENSIONS;
        a2 = (c + 2) % N_DIMENSIONS;
        cr[c] = v1[a1] * v2[a2] - v1[a2] * v2[a1];
    }
}

private  Real  get_axis_coordinates(
    int   n,
    Real  x_axis[],
    Real  y_axis[],
    Real  vector[] )
{
    Real  pos, x_dot_x, x_dot_v, x_dot_y, y_dot_v, v_dot_v;

    x_dot_x = dot( n, x_axis, x_axis );
    x_dot_v = dot( n, x_axis, vector );
    x_dot_y = dot( n, x_axis, y_axis );
    y_dot_v = dot( n, y_axis, vector );
    v_dot_v = dot( n, vector, vector );

    pos = (x_dot_x * v_dot_v - x_dot_v * x_dot_v) /
          (x_dot_x * y_dot_v - x_dot_v * x_dot_y);

    return( pos );
}

private  void  get_two_axes_coordinates(
    int   n,
    Real  vector[],
    Real  x_axis[],
    Real  y_axis[],
    Real  *x_pos,
    Real  *y_pos )
{
    *y_pos = get_axis_coordinates( n, x_axis, y_axis, vector );
    *x_pos = get_axis_coordinates( n, y_axis, x_axis, vector );
}

private  void  map_voxel_to_pixel(
    int    n,
    Real   voxel[],
    Real   origin[],
    Real   x_axis[],
    Real   y_axis[],
    Real   *x_pixel,
    Real   *y_pixel )
{
    int   c;
    Real  len_x, len_y, x_dot_y, factor, z_mag;
    Real  vector[MAX_DIMENSIONS];
    Real  z_axis[MAX_DIMENSIONS];

    for_less( c, 0, n )
        vector[c] = voxel[c] - origin[c];

    len_x = sqrt( dot( n, x_axis, x_axis ) );
    len_y = sqrt( dot( n, y_axis, y_axis ) );

    x_dot_y = dot( n, x_axis, y_axis );
    if( len_x != 0.0 )
        x_dot_y /= len_x;
    if( len_y != 0.0 )
        x_dot_y /= len_y;

    if( ABS( x_dot_y ) < 0.001 )
    {
        *x_pixel = dot( n, vector, x_axis ) / len_x / len_x;
        *y_pixel = dot( n, vector, y_axis ) / len_y / len_y;
    }
    else if( n == 3 )
    {
        cross( n, x_axis, y_axis, z_axis );
        z_mag = dot( n, z_axis, z_axis );
        if( z_mag != 0.0 )
        {
            factor = dot( n, vector, z_axis ) / z_mag;
            for_less( c, 0, n )
                vector[c] -= factor * z_axis[c];
        }

        get_two_axes_coordinates( n, vector, x_axis, y_axis,
                                  x_pixel, y_pixel );
    }
    else
    {
        print( "Cannot handle nonortho axis of non-3D data.\n" );
        *x_pixel = 0.0;
        *y_pixel = 0.0;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_slice_pixel_to_voxel
@INPUT      : volume
              x_pixel
              y_pixel
              slice_position
              x_axis_index
              y_axis_index
              x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : pixel_slice_position
@RETURNS    : 
@DESCRIPTION: Converts pixel from the viewport to a voxel position.  The
              centres of pixels correspond to .0, as opposed to .5.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  convert_slice_pixel_to_voxel(
    Volume          volume,
    Real            x_pixel,
    Real            y_pixel,
    Real            origin[],
    Real            x_axis[],
    Real            y_axis[],
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            voxel[] )
{
    int          n_dims;
    Real         real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];
    Real         real_origin[MAX_DIMENSIONS];

    get_mapping( volume, origin, x_axis, y_axis,
                 x_translation, y_translation, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

    n_dims = get_volume_n_dimensions( volume );

    map_pixel_to_voxel( x_pixel, y_pixel, n_dims, real_origin,
                        real_x_axis, real_y_axis, voxel );

    return( voxel_is_within_volume( volume, voxel ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_voxel_to_slice_pixel
@INPUT      : volume
              voxel_position
              x_axis_index
              y_axis_index
              x_translation
              y_translation
              x_scale
              y_scale
@OUTPUT     : x_pixel
              y_pixel
@RETURNS    : 
@DESCRIPTION: Converts the voxel position (centres of voxels are at .0,
              not .5) to a pixel position in the viewport.  To get the
              integer pixel position, take ROUND( x_pixel ).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_voxel_to_slice_pixel(
    Volume          volume,
    Real            voxel[],
    Real            origin[],
    Real            x_axis[],
    Real            y_axis[],
    Real            x_translation,
    Real            y_translation,
    Real            x_scale,
    Real            y_scale,
    Real            *x_pixel,
    Real            *y_pixel )
{
    int          n_dims;
    Real         real_x_axis[MAX_DIMENSIONS], real_y_axis[MAX_DIMENSIONS];
    Real         real_origin[MAX_DIMENSIONS];

    get_mapping( volume, origin, x_axis, y_axis,
                 x_translation, y_translation, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

    n_dims = get_volume_n_dimensions( volume );

    map_voxel_to_pixel( n_dims, voxel, real_origin,
                        real_x_axis, real_y_axis, x_pixel, y_pixel );
}

public  void  resize_volume_slice(
    int          old_x_viewport_size,
    int          old_y_viewport_size,
    int          old_used_x_viewport_size,
    int          old_used_y_viewport_size,
    int          new_x_viewport_size,
    int          new_y_viewport_size,
    Real         *x_translation,
    Real         *y_translation,
    Real         *x_scale,
    Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size )
{
    Real      x_scale_factor, y_scale_factor, scale_factor;

    x_scale_factor = (Real) new_x_viewport_size /
                     (Real) old_used_x_viewport_size;
    y_scale_factor = (Real) new_y_viewport_size /
                     (Real) old_used_y_viewport_size;
    scale_factor = MIN( x_scale_factor, y_scale_factor );

    if( used_x_viewport_size != (int *) NULL )
        *used_x_viewport_size = scale_factor * old_used_x_viewport_size;
    if( used_y_viewport_size != (int *) NULL )
        *used_y_viewport_size = scale_factor * old_used_y_viewport_size;

    scale_slice_about_viewport_centre( scale_factor,
                                       old_x_viewport_size, old_y_viewport_size,
                                       x_translation, y_translation,
                                       x_scale, y_scale );

    *x_translation += (new_x_viewport_size - old_x_viewport_size) / 2.0;
    *y_translation += (new_y_viewport_size - old_y_viewport_size) / 2.0;
}

public  void  fit_volume_slice_to_viewport(
    Volume       volume,
    Real         origin[],
    Real         x_axis[],
    Real         y_axis[],
    int          x_viewport_size,
    int          y_viewport_size,
    Real         fraction_oversize,
    Real         *x_translation,
    Real         *y_translation,
    Real         *x_scale,
    Real         *y_scale,
    int          *used_x_viewport_size,
    int          *used_y_viewport_size )
{
    Real         x_min, x_max, y_min, y_max;

    get_volume_mapping_range( volume, origin, x_axis, y_axis,
                              0.0, 0.0, 1.0, 1.0,
                              &x_min, &x_max, &y_min, &y_max );

    if( x_min == x_max || y_min == y_max )
    {
        *x_translation = 0.0;
        *y_translation = 0.0;
        *x_scale = 1.0;
        *y_scale = 1.0;
        return;
    }

    *x_scale = (Real) x_viewport_size / (x_max - x_min) /
               (1.0 + fraction_oversize);
    *y_scale = (Real) y_viewport_size / (y_max - y_min) /
               (1.0 + fraction_oversize);

    if( *x_scale < *y_scale )
        *y_scale = *x_scale;
    else
        *x_scale = *y_scale;

    if( used_x_viewport_size != (int *) NULL )
    {
        *used_x_viewport_size = *x_scale * (x_max - x_min) *
                                (1.0 + fraction_oversize);
    }

    if( used_y_viewport_size != (int *) NULL )
    {
        *used_y_viewport_size = *y_scale * (y_max - y_min) *
                                (1.0 + fraction_oversize);
    }

    *x_translation = ((Real) x_viewport_size - *x_scale * (x_max - x_min))/2.0 -
                     *x_scale * x_min;
    *y_translation = ((Real) y_viewport_size - *y_scale * (y_max - y_min))/2.0 -
                     *y_scale * y_min;
}

public  void   scale_slice_about_viewport_centre(
    Real        scale_factor,
    int         x_viewport_size,
    int         y_viewport_size,
    Real        *x_translation,
    Real        *y_translation,
    Real        *x_scale,
    Real        *y_scale )
{
    Real  x_centre, y_centre;

    x_centre = (Real) x_viewport_size / 2.0;
    y_centre = (Real) y_viewport_size / 2.0;

    *x_translation = x_centre - scale_factor * (x_centre - *x_translation);
    *y_translation = y_centre - scale_factor * (y_centre - *y_translation);
    *x_scale *= scale_factor;
    *y_scale *= scale_factor;
}
