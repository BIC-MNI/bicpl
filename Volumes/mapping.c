
#include  <internal_volume_io.h>
#include  <vols.h>
#include  <numerical.h>

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
    Real  separations[MAX_DIMENSIONS], x_len, y_len, comp;

    n_dims = get_volume_n_dimensions( volume );
    get_volume_separations( volume, separations );

    x_len = 0.0;
    y_len = 0.0;
    for_less( c, 0, n_dims )
    {
        comp = x_axis[c] * separations[c];
        x_len += comp * comp;
        comp = y_axis[c] * separations[c];
        y_len += comp * comp;
    }

    x_len = sqrt( x_len );
    if( x_len == 0.0 )
        x_len = 1.0;
    y_len = sqrt( y_len );
    if( y_len == 0.0 )
        y_len = 1.0;

    x_scale *= x_len;
    y_scale *= y_len;

    for_less( c, 0, n_dims )
    {
        pix_x_axis[c] = x_axis[c] / x_scale;
        pix_y_axis[c] = y_axis[c] / y_scale;
        pix_origin[c] = origin[c] - pix_x_axis[c] * x_translation
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

private  void  get_two_axes_coordinates(
    int   n,
    Real  vector[],
    Real  x_axis[],
    Real  y_axis[],
    Real  *x_pos,
    Real  *y_pos )
{
    Real  x_dot_x, x_dot_v, x_dot_y, y_dot_v, y_dot_y;

    x_dot_x = dot( n, x_axis, x_axis );
    x_dot_v = dot( n, x_axis, vector );
    x_dot_y = dot( n, x_axis, y_axis );
    y_dot_y = dot( n, y_axis, y_axis );
    y_dot_v = dot( n, y_axis, vector );

    *x_pos = (x_dot_v * y_dot_y - x_dot_y * y_dot_v) /
             (x_dot_x * y_dot_y - x_dot_y * x_dot_y);
    *y_pos = (y_dot_v * x_dot_x - x_dot_y * x_dot_v) /
             (y_dot_y * x_dot_x - x_dot_y * x_dot_y);
}

public  void  map_voxel_to_pixel(
    int    n,
    Real   voxel[],
    Real   origin[],
    Real   x_axis[],
    Real   y_axis[],
    Real   *x_pixel,
    Real   *y_pixel )
{
    int   c;
    Real  vector[MAX_DIMENSIONS];

    for_less( c, 0, n )
        vector[c] = voxel[c] - origin[c];

    get_two_axes_coordinates( n, vector, x_axis, y_axis,
                              x_pixel, y_pixel );
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

    n_dims = get_volume_n_dimensions( volume );

    get_mapping( volume, origin, x_axis, y_axis,
                 x_translation, y_translation, x_scale, y_scale,
                 real_origin, real_x_axis, real_y_axis );

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

    if( old_used_x_viewport_size <= 0 )
        old_used_x_viewport_size = 1;
    if( old_used_y_viewport_size <= 0 )
        old_used_y_viewport_size = 1;

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
        *x_scale = 0.0;
        *y_scale = 0.0;
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_is_within_volume
@INPUT      : volume
              voxel_position
@OUTPUT     : 
@RETURNS    : TRUE if voxel is within volume.
@DESCRIPTION: Determines if a voxel position is within the volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  voxel_is_within_volume(
    Volume   volume,
    Real     voxel_position[] )
{
    int      i, sizes[MAX_DIMENSIONS];
    BOOLEAN  inside;

    inside = TRUE;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, get_volume_n_dimensions(volume) )
    {
        if( voxel_position[i] < -0.5 ||
            voxel_position[i] >= (Real) sizes[i] - 0.5 )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : int_voxel_is_within_volume
@INPUT      : volume
              indices
@OUTPUT     : 
@RETURNS    : TRUE if voxel within volume
@DESCRIPTION: Determines if the voxel with integer coordinates is within the
              volume.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  int_voxel_is_within_volume(
    Volume   volume,
    int      indices[] )
{
    int      i, sizes[MAX_DIMENSIONS];
    BOOLEAN  inside;

    inside = TRUE;

    get_volume_sizes( volume, sizes );

    for_less( i, 0, get_volume_n_dimensions(volume) )
    {
        if( indices[i] < 0 || indices[i] >= sizes[i] )
        {
            inside = FALSE;
            break;
        }
    }

    return( inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_real_to_int_voxel
@INPUT      : n_dimensions
              voxel
@OUTPUT     : int_voxel
@RETURNS    : 
@DESCRIPTION: Converts real valued voxel positions to integer positions, by
              rounding.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  convert_real_to_int_voxel(
    int      n_dimensions,
    Real     voxel[],
    int      int_voxel[] )
{
    int   i;

    for_less( i, 0, n_dimensions )
        int_voxel[i] = ROUND( voxel[i] );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : voxel_contains_range
@INPUT      : volume
              int_voxel
              target_value
@OUTPUT     : 
@RETURNS    : TRUE if voxel contains this value
@DESCRIPTION: Determines if the voxel contains this value, by assuming
              trilinear interpolation between the 8 corner values of this
              voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  voxel_contains_range(
    Volume   volume,
    int      int_voxel[],
    Real     min_value,
    Real     max_value )
{               
    BOOLEAN  less, greater;
    int      n_dimensions;
    int      c, mx, my, mz, ms, mt;
    int      base_indices[MAX_DIMENSIONS], indices[MAX_DIMENSIONS];
    Real     value;

    n_dimensions = get_volume_n_dimensions( volume );

    if( n_dimensions >= 1 )
        mx = 2;
    else
        mx = 1;

    if( n_dimensions >= 2 )
        my = 2;
    else
        my = 1;

    if( n_dimensions >= 3 )
        mz = 2;
    else
        mz = 1;

    if( n_dimensions >= 4 )
        ms = 2;
    else
        ms = 1;

    if( n_dimensions >= 5 )
        mt = 2;
    else
        mt = 1;

    for_less( c, 0, n_dimensions )
        base_indices[c] = int_voxel[c];

    for_less( c, n_dimensions, MAX_DIMENSIONS )
        base_indices[c] = 0;

    less = FALSE;
    greater = FALSE;

    for_less( indices[X], base_indices[X], base_indices[X] + mx )
    for_less( indices[Y], base_indices[Y], base_indices[Y] + my )
    for_less( indices[Z], base_indices[Z], base_indices[Z] + mz )
    for_less( indices[3], base_indices[3], base_indices[3] + ms )
    for_less( indices[4], base_indices[4], base_indices[4] + mt )
    {
        value = get_volume_real_value( volume,
                   indices[0], indices[1], indices[2], indices[3], indices[4] );

        if( value < min_value )
        {
            if( greater )
                return( TRUE );
            less = TRUE;
        }
        else if( value > max_value )
        {
            if( less )
                return( TRUE );
            greater = TRUE;
        }
        else
            return( TRUE );
    }

    return( FALSE );
}
