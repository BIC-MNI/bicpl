#include  <internal_volume_io.h>
#include  <vols.h>

typedef struct {
    Real           position;
    Colour         colour;
    Colour_spaces  interpolation_space;
} colour_point;

private  Colour  interpolate_colours( colour_point *, colour_point *, Real );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : initialize_colour_coding
@INPUT      : type
              under_colour
              over_colour
              min_value
              max_value
@OUTPUT     : colour_coding
@RETURNS    : 
@DESCRIPTION: Initializes the colour coding struct to the given values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  initialize_colour_coding(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type,
    Colour                 under_colour,
    Colour                 over_colour,
    Real                   min_value,
    Real                   max_value )
{
    set_colour_coding_type( colour_coding, type );
    set_colour_coding_min_max( colour_coding, min_value, max_value );
    set_colour_coding_under_colour( colour_coding, under_colour );
    set_colour_coding_over_colour( colour_coding, over_colour );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_colour_coding_type
@INPUT      : colour_coding
              type
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the type of the colour coding (e.g. GRAY_SCALE, HOT_METAL).
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_colour_coding_type(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type )
{
    colour_coding->type = type;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_colour_coding_min_max
@INPUT      : colour_coding
              min_value
              max_value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the limits of the colour coding.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    Real                   min_value,
    Real                   max_value )
{
    colour_coding->min_value = min_value;
    colour_coding->max_value = max_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_coding_min_max
@INPUT      : colour_coding
@OUTPUT     : min_value
              max_value
@RETURNS    : 
@DESCRIPTION: Passes back the limits of the colour coding.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  get_colour_coding_min_max(
    colour_coding_struct   *colour_coding,
    Real                   *min_value,
    Real                   *max_value )
{
    *min_value = colour_coding->min_value;
    *max_value = colour_coding->max_value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_coding_under_colour
@INPUT      : colour_coding
@OUTPUT     : 
@RETURNS    : Colour
@DESCRIPTION: Returns the colour used to code values below the min_value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Colour  get_colour_coding_under_colour(
    colour_coding_struct   *colour_coding )
{
    return( colour_coding->under_colour );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_colour_coding_under_colour
@INPUT      : colour_coding
              under_colour
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the colour used to code values below the min_value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_colour_coding_under_colour(
    colour_coding_struct   *colour_coding,
    Colour                 under_colour )
{
    colour_coding->under_colour = under_colour;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_coding_over_colour
@INPUT      : colour_coding
@OUTPUT     : 
@RETURNS    : Colour
@DESCRIPTION: Returns the colour used to code values above the max_value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Colour  get_colour_coding_over_colour(
    colour_coding_struct   *colour_coding )
{
    return( colour_coding->over_colour );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_colour_coding_over_colour
@INPUT      : colour_coding
              over_colour
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the colour used to code values above the max_value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_colour_coding_over_colour(
    colour_coding_struct   *colour_coding,
    Colour                 over_colour )
{
    colour_coding->over_colour = over_colour;
}

private  int  get_colour_table_piecewise_function(
    colour_coding_struct  *colour_coding,
    colour_point          *points[] )
{
    int     n_points;
    static  colour_point  gray_scale_points[] =
                             { {0.0, BLACK, RGB_SPACE },
                               {1.0, WHITE, RGB_SPACE } };
    static  colour_point  hot_metal_points[] = 
                       { {0.0,  make_Colour_0_1( 0.0, 0.0, 0.0 ), RGB_SPACE },
                         {0.25, make_Colour_0_1( 0.5, 0.0, 0.0 ), RGB_SPACE },
                         {0.5,  make_Colour_0_1( 1.0, 0.5, 0.0 ), RGB_SPACE },
                         {0.75, make_Colour_0_1( 1.0, 1.0, 0.5 ), RGB_SPACE },
                         {1.0,  make_Colour_0_1( 1.0, 1.0, 1.0 ), RGB_SPACE } };
    static  colour_point  spectral_points[] =
                    { { 0.00, make_Colour_0_1(0.0000,0.0000,0.0000), RGB_SPACE},
                      { 0.05, make_Colour_0_1(0.4667,0.0000,0.5333), RGB_SPACE},
                      { 0.10, make_Colour_0_1(0.5333,0.0000,0.6000), RGB_SPACE},
                      { 0.15, make_Colour_0_1(0.0000,0.0000,0.6667), RGB_SPACE},
                      { 0.20, make_Colour_0_1(0.0000,0.0000,0.8667), RGB_SPACE},
                      { 0.25, make_Colour_0_1(0.0000,0.4667,0.8667), RGB_SPACE},
                      { 0.30, make_Colour_0_1(0.0000,0.6000,0.8667), RGB_SPACE},
                      { 0.35, make_Colour_0_1(0.0000,0.6667,0.6667), RGB_SPACE},
                      { 0.40, make_Colour_0_1(0.0000,0.6667,0.5333), RGB_SPACE},
                      { 0.45, make_Colour_0_1(0.0000,0.6000,0.0000), RGB_SPACE},
                      { 0.50, make_Colour_0_1(0.0000,0.7333,0.0000), RGB_SPACE},
                      { 0.55, make_Colour_0_1(0.0000,0.8667,0.0000), RGB_SPACE},
                      { 0.60, make_Colour_0_1(0.0000,1.0000,0.0000), RGB_SPACE},
                      { 0.65, make_Colour_0_1(0.7333,1.0000,0.0000), RGB_SPACE},
                      { 0.70, make_Colour_0_1(0.9333,0.9333,0.0000), RGB_SPACE},
                      { 0.75, make_Colour_0_1(1.0000,0.8000,0.0000), RGB_SPACE},
                      { 0.80, make_Colour_0_1(1.0000,0.6000,0.0000), RGB_SPACE},
                      { 0.85, make_Colour_0_1(1.0000,0.0000,0.0000), RGB_SPACE},
                      { 0.90, make_Colour_0_1(0.8667,0.0000,0.0000), RGB_SPACE},
                      { 0.95, make_Colour_0_1(0.8000,0.0000,0.0000), RGB_SPACE},
                      { 1.00, make_Colour_0_1(0.8000,0.8000,0.8000), RGB_SPACE}
                    };
#ifdef  OLD
                    { {0.0,     make_Colour_0_1( 0.0, 0.0, 1.0 ), HSL_SPACE },
                      {0.333,   make_Colour_0_1( 0.0, 1.0, 0.0 ), HSL_SPACE },
                      {0.74,    make_Colour_0_1( 1.0, 0.0, 0.0 ), HSL_SPACE },
                      {0.86,    make_Colour_0_1( 1.0, 0.0, 1.0 ), HSL_SPACE },
                      {1.0,     make_Colour_0_1( 1.0, 1.0, 1.0 ), HSL_SPACE } };
#endif
    static  colour_point  red_points[] =
                             { {0.0, BLACK, RGB_SPACE },
                               {1.0, RED, RGB_SPACE } };
    static  colour_point  green_points[] =
                             { {0.0, BLACK, RGB_SPACE },
                               {1.0, GREEN, RGB_SPACE } };
    static  colour_point  blue_points[] =
                             { {0.0, BLACK, RGB_SPACE },
                               {1.0, BLUE, RGB_SPACE } };
    static  colour_point  contour_points[] =
                      { {0.0,   make_Colour_0_1( 0.0, 0.0, 0.3 ), RGB_SPACE },
                        {0.166, make_Colour_0_1( 0.0, 0.0, 1.0 ), RGB_SPACE },
                        {0.166, make_Colour_0_1( 0.0, 0.3, 0.3 ), RGB_SPACE },
                        {0.333, make_Colour_0_1( 0.0, 1.0, 1.0 ), RGB_SPACE },
                        {0.333, make_Colour_0_1( 0.0, 0.3, 0.0 ), RGB_SPACE },
                        {0.5,   make_Colour_0_1( 0.0, 1.0, 0.0 ), RGB_SPACE },
                        {0.5,   make_Colour_0_1( 0.3, 0.3, 0.0 ), RGB_SPACE },
                        {0.666, make_Colour_0_1( 1.0, 1.0, 0.0 ), RGB_SPACE },
                        {0.666, make_Colour_0_1( 0.3, 0.0, 0.0 ), RGB_SPACE },
                        {0.833, make_Colour_0_1( 1.0, 0.0, 0.0 ), RGB_SPACE },
                        {0.833, make_Colour_0_1( 0.3, 0.3, 0.3 ), RGB_SPACE },
                        {1.0,   make_Colour_0_1( 1.0, 1.0, 1.0 ), RGB_SPACE } };

    switch( colour_coding->type )
    {
    case  GRAY_SCALE:
        n_points = SIZEOF_STATIC_ARRAY( gray_scale_points );
        *points = gray_scale_points;
        break;

    case  HOT_METAL:
        n_points = SIZEOF_STATIC_ARRAY( hot_metal_points );
        *points = hot_metal_points;
        break;

    case  SPECTRAL:
        n_points = SIZEOF_STATIC_ARRAY( spectral_points );
        *points = spectral_points;
        break;

    case  RED_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( red_points );
        *points = red_points;
        break;

    case  GREEN_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( green_points );
        *points = green_points;
        break;

    case  BLUE_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( blue_points );
        *points = blue_points;
        break;

    case  CONTOUR_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( contour_points );
        *points = contour_points;
        break;

    default:
        print( "User defined colour map not implemented\n" );
        n_points = SIZEOF_STATIC_ARRAY( gray_scale_points );
        *points = gray_scale_points;
        break;
    }

    return( n_points );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_code
@INPUT      : colour_coding
              value
@OUTPUT     : 
@RETURNS    : Colour
@DESCRIPTION: Returns the colour associated with the value.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Colour  get_colour_code(
    colour_coding_struct  *colour_coding,
    Real                  value )
{
    Real           pos;
    int            i, n_points;
    colour_point   *points;

    if( colour_coding->min_value <= colour_coding->max_value )
    {
        if( value < colour_coding->min_value )
            return( colour_coding->under_colour );
        else if( value > colour_coding->max_value )
            return( colour_coding->over_colour );
    }
    else
    {
        if( value > colour_coding->min_value )
            return( colour_coding->under_colour );
        else if( value < colour_coding->max_value )
            return( colour_coding->over_colour );
    }

    pos = (value - colour_coding->min_value) /
          (colour_coding->max_value - colour_coding->min_value);

    n_points = get_colour_table_piecewise_function( colour_coding, &points );

    for_less( i, 0, n_points-1 )
    {
        if( pos < points[i+1].position )
            break;
    }

    return( interpolate_colours( &points[i], &points[i+1], pos ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : interpolate_colours
@INPUT      : p1    - rgb or hls colours
              p2
              pos   - position relative to position of colours
@OUTPUT     : 
@RETURNS    : interpolated colour
@DESCRIPTION: Interpolates a colour based on two positions on a piecewise
              linear function.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  Colour  interpolate_colours(
    colour_point   *p1,
    colour_point   *p2,
    Real           pos )
{
    Real  ratio, r,g, b, r0, g0, b0, r1, g1, b1;

    ratio = (pos - p1->position) / (p2->position - p1->position);

    r0 = get_Colour_r_0_1( p1->colour );
    g0 = get_Colour_g_0_1( p1->colour );
    b0 = get_Colour_b_0_1( p1->colour );

    r1 = get_Colour_r_0_1( p2->colour );
    g1 = get_Colour_g_0_1( p2->colour );
    b1 = get_Colour_b_0_1( p2->colour );

    if( p1->interpolation_space == HSL_SPACE )
    {
        rgb_to_hsl( r0, g0, b0, &r0, &g0, &b0 );
        rgb_to_hsl( r1, g1, b1, &r1, &g1, &b1 );

        if( r0 == 1.0 && r1 < 0.5 )
            r0 = 0.0;
        if( r1 == 1.0 && r0 < 0.5 )
            r1 = 0.0;
    }

    r = r0 + (r1 - r0) * ratio;
    g = g0 + (g1 - g0) * ratio;
    b = b0 + (b1 - b0) * ratio;

    if( p1->interpolation_space == HSL_SPACE )
        hsl_to_rgb( r, g, b, &r, &g, &b );

    return( make_Colour_0_1( r, g, b ) );
}
