
#include  <def_register.h>

public  Real  map_viewport_to_slice_1d(
    Real viewport_pos,
    Real translation,
    Real scale )
{
    return( (viewport_pos - translation) / scale );
}

public  Real  map_slice_to_viewport_1d(
    Real slice_pos,
    Real translation,
    Real scale )
{
    return( slice_pos * scale + translation );
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
    Boolean   empty;
    Real      left_pos, right_pos, clipped_x_end, x_start_pixel, x_end_pixel;
    Real      bottom_pos, top_pos, clipped_y_end, y_start_pixel, y_end_pixel;

    empty = FALSE;

    left_pos = map_viewport_to_slice_1d( -0.5, x_translation, x_delta );
    *clipped_x_start = MAX( left_pos, x_start );

    right_pos = map_viewport_to_slice_1d( (Real) x_viewport_size - 0.5,
                   x_translation, x_delta );
    clipped_x_end = MIN( right_pos, x_end );

    if( x_end > x_start )
    {
        x_start_pixel = map_slice_to_viewport_1d( *clipped_x_start,
                                                  x_translation, x_delta );
        x_end_pixel = map_slice_to_viewport_1d( clipped_x_end,
                                                x_translation, x_delta );
        *x_position = (int) (x_start_pixel + 0.5);
        *x_size = (int) (x_end_pixel - x_start_pixel);
    }
    else
        empty = TRUE;

    bottom_pos = map_viewport_to_slice_1d( -0.5, y_translation, y_delta );
    *clipped_y_start = MAX( bottom_pos, y_start );

    top_pos = map_viewport_to_slice_1d( (Real) y_viewport_size - 0.5,
                                        y_translation, y_delta );
    clipped_y_end = MIN( top_pos, y_end );

    if( y_end > y_start )
    {
        y_start_pixel = map_slice_to_viewport_1d( *clipped_y_start,
                                                  y_translation, y_delta );
        y_end_pixel = map_slice_to_viewport_1d( clipped_y_end,
                                                y_translation, y_delta );
        *y_position = (int) (y_start_pixel + 0.5);
        *y_size = (int) (y_end_pixel - y_start_pixel);
    }
    else
        empty = TRUE;

    if( empty )
    {
        *x_size = 0;
        *y_size = 0;
    }

    return( !empty );
}
