
#include  <def_mni.h>

public  Real  map_viewport_to_slice_1d(
    Real viewport_pos,
    Real translation,
    Real scale_factor )
{
    return( (viewport_pos - translation) / scale_factor );
}

public  Real  map_slice_to_viewport_1d(
    Real slice_pos,
    Real translation,
    Real scale_factor )
{
    return( slice_pos * scale_factor + translation );
}

private  void  clip_slice_to_pixels_1d(
    int           viewport_size,
    Real          translation,
    Real          delta,
    Real          start_voxel,
    Real          end_voxel,
    int           *start_pixel,
    int           *end_pixel )
{
    Real      clipped_start, clipped_end;
    Real      left_pos, right_pos, real_start_pixel, real_end_pixel;

    left_pos = map_viewport_to_slice_1d( -0.5, translation, delta );

    if( delta < 0.0 )
        clipped_start = MIN( left_pos, start_voxel );
    else
        clipped_start = MAX( left_pos, start_voxel );

    right_pos = map_viewport_to_slice_1d( (Real) viewport_size - 0.5,
                                          translation, delta );

    if( delta < 0.0 )
        clipped_end = MAX( right_pos, end_voxel );
    else
        clipped_end = MIN( right_pos, end_voxel );

    if( delta > 0.0 && clipped_end > clipped_start ||
        delta < 0.0 && clipped_end < clipped_start )
    {
        real_start_pixel = map_slice_to_viewport_1d( clipped_start,
                                                     translation, delta );
        real_end_pixel = map_slice_to_viewport_1d( clipped_end,
                                                   translation, delta );
        if( IS_INT( real_start_pixel ) )
            *start_pixel = (int) real_start_pixel;
        else
            *start_pixel = (int) real_start_pixel + 1;

        *end_pixel = (int) real_end_pixel;
    }
    else
    {
        *start_pixel = 0;
        *end_pixel = -1;
    }
}

public  void  clip_slice_to_pixel_range(
    int           x_viewport_size,
    int           y_viewport_size,
    Real          x_translation,
    Real          y_translation,
    Real          x_delta,
    Real          y_delta,
    Real          x_start,
    Real          y_start,
    Real          x_end,
    Real          y_end,
    int           *x_pixel_start,
    int           *y_pixel_start,
    int           *x_pixel_end,
    int           *y_pixel_end )
{
    clip_slice_to_pixels_1d( x_viewport_size, x_translation,
                             x_delta, x_start, x_end,
                             x_pixel_start, x_pixel_end );

    clip_slice_to_pixels_1d( y_viewport_size, y_translation,
                             y_delta, y_start, y_end,
                             y_pixel_start, y_pixel_end );
}

public  void  convert_pixel_start_to_voxel(
    int           x_pixel,
    int           y_pixel,
    Real          x_translation,
    Real          y_translation,
    Real          x_delta,
    Real          y_delta,
    Real          *x_voxel_start,
    Real          *y_voxel_start )
{
    *x_voxel_start = map_viewport_to_slice_1d( (Real) x_pixel,
                                               x_translation, x_delta );
    *y_voxel_start = map_viewport_to_slice_1d( (Real) y_pixel,
                                               y_translation, y_delta );
}
