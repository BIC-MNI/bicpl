/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>
#include  <vols.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/colour_coding.c,v 1.16 1995-07-31 13:45:42 david Exp $";
#endif

typedef struct {
    Real           position;
    Real           r, g, b;
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_table_piecewise_function
@INPUT      : colour_coding
@OUTPUT     : points
@RETURNS    : number of points
@DESCRIPTION: Gets a piecewise function from 0 to 1 representing the colour
              coding.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  int  get_colour_table_piecewise_function(
    colour_coding_struct  *colour_coding,
    colour_point          *points[] )
{
    int     n_points;
    static  colour_point  gray_scale_points[] =
                             { {0.0, 0.0, 0.0, 0.0, RGB_SPACE },
                               {1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  hot_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, RGB_SPACE },
                         {0.25, 0.5, 0.0, 0.0, RGB_SPACE },
                         {0.5,  1.0, 0.5, 0.0, RGB_SPACE },
                         {0.75, 1.0, 1.0, 0.5, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  spectral_points[] =
                    { { 0.00, 0.0000,0.0000,0.0000, RGB_SPACE},
                      { 0.05, 0.4667,0.0000,0.5333, RGB_SPACE},
                      { 0.10, 0.5333,0.0000,0.6000, RGB_SPACE},
                      { 0.15, 0.0000,0.0000,0.6667, RGB_SPACE},
                      { 0.20, 0.0000,0.0000,0.8667, RGB_SPACE},
                      { 0.25, 0.0000,0.4667,0.8667, RGB_SPACE},
                      { 0.30, 0.0000,0.6000,0.8667, RGB_SPACE},
                      { 0.35, 0.0000,0.6667,0.6667, RGB_SPACE},
                      { 0.40, 0.0000,0.6667,0.5333, RGB_SPACE},
                      { 0.45, 0.0000,0.6000,0.0000, RGB_SPACE},
                      { 0.50, 0.0000,0.7333,0.0000, RGB_SPACE},
                      { 0.55, 0.0000,0.8667,0.0000, RGB_SPACE},
                      { 0.60, 0.0000,1.0000,0.0000, RGB_SPACE},
                      { 0.65, 0.7333,1.0000,0.0000, RGB_SPACE},
                      { 0.70, 0.9333,0.9333,0.0000, RGB_SPACE},
                      { 0.75, 1.0000,0.8000,0.0000, RGB_SPACE},
                      { 0.80, 1.0000,0.6000,0.0000, RGB_SPACE},
                      { 0.85, 1.0000,0.0000,0.0000, RGB_SPACE},
                      { 0.90, 0.8667,0.0000,0.0000, RGB_SPACE},
                      { 0.95, 0.8000,0.0000,0.0000, RGB_SPACE},
                      { 1.00, 0.8000,0.8000,0.8000, RGB_SPACE}
                    };
    static  colour_point  red_points[] =
                             { {0.0, 0.0, 0.0, 0.0, RGB_SPACE },
                               {1.0, 1.0, 0.0, 0.0, RGB_SPACE } };
    static  colour_point  green_points[] =
                             { {0.0, 0.0, 0.0, 0.0, RGB_SPACE },
                               {1.0, 0.0, 1.0, 0.0, RGB_SPACE } };
    static  colour_point  blue_points[] =
                             { {0.0, 0.0, 0.0, 0.0, RGB_SPACE },
                               {1.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  contour_points[] =
                      { {0.0,   0.0, 0.0, 0.3, RGB_SPACE },
                        {0.166, 0.0, 0.0, 1.0, RGB_SPACE },
                        {0.166, 0.0, 0.3, 0.3, RGB_SPACE },
                        {0.333, 0.0, 1.0, 1.0, RGB_SPACE },
                        {0.333, 0.0, 0.3, 0.0, RGB_SPACE },
                        {0.5,   0.0, 1.0, 0.0, RGB_SPACE },
                        {0.5,   0.3, 0.3, 0.0, RGB_SPACE },
                        {0.666, 1.0, 1.0, 0.0, RGB_SPACE },
                        {0.666, 0.3, 0.0, 0.0, RGB_SPACE },
                        {0.833, 1.0, 0.0, 0.0, RGB_SPACE },
                        {0.833, 0.3, 0.3, 0.3, RGB_SPACE },
                        {1.0,   1.0, 1.0, 1.0, RGB_SPACE } };

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
        print_error( "User defined colour map not implemented\n" );
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
        if( pos <= points[i+1].position )
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

    r0 = p1->r;
    g0 = p1->g;
    b0 = p1->b;

    r1 = p2->r;
    g1 = p2->g;
    b1 = p2->b;

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
