
#include  <def_mni.h>

private  Boolean  clip_slice_to_viewport_1d(
    int           viewport_size,
    Real          translation,
    Real          delta,
    Real          start_voxel,
    Real          end_voxel,
    Real          *clipped_start,
    int           *n_pixels,
    int           *position );

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

public  Boolean  clip_slice_to_viewport(
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
    Real          *clipped_x_start,
    Real          *clipped_y_start,
    int           *x_size,
    int           *y_size,
    int           *x_position,
    int           *y_position )
{
    Boolean   overlap;

    overlap = clip_slice_to_viewport_1d( x_viewport_size, x_translation,
                                       x_delta, x_start, x_end,
                                       clipped_x_start, x_size, x_position );

    if( overlap )
    {
        overlap = clip_slice_to_viewport_1d(
                            y_viewport_size, y_translation,
                            y_delta, y_start, y_end,
                            clipped_y_start, y_size, y_position );
    }

    return( overlap );
}

private  Boolean  clip_slice_to_viewport_1d(
    int           viewport_size,
    Real          translation,
    Real          delta,
    Real          start_voxel,
    Real          end_voxel,
    Real          *clipped_start,
    int           *n_pixels,
    int           *position )
{
    Boolean   empty;
    Real      left_pos, right_pos, clipped_end, start_pixel, end_pixel;

    left_pos = map_viewport_to_slice_1d( -0.5, translation, delta );

    if( delta < 0.0 )
        *clipped_start = MIN( left_pos, start_voxel );
    else
        *clipped_start = MAX( left_pos, start_voxel );

    right_pos = map_viewport_to_slice_1d( (Real) viewport_size - 0.5,
                                          translation, delta );

    if( delta < 0.0 )
        clipped_end = MAX( right_pos, end_voxel );
    else
        clipped_end = MIN( right_pos, end_voxel );

    if( delta > 0.0 && clipped_end > *clipped_start ||
        delta < 0.0 && clipped_end < *clipped_start )
    {
        start_pixel = map_slice_to_viewport_1d( *clipped_start,
                                                translation, delta );
        end_pixel = map_slice_to_viewport_1d( clipped_end,
                                              translation, delta );
        *position = (int) (start_pixel + 0.5);
        *n_pixels = (int) (end_pixel - start_pixel);
        empty = (*n_pixels <= 0);
    }
    else
    {
        *n_pixels = 0;
        empty = TRUE;
    }

    return( !empty );
}
