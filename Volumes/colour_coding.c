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

#include  "bicpl_internal.h"

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/colour_coding.c,v 1.30 2014-10-29 20:50:19 claude Exp $";
#endif

static void  interpolate_colours(
    colour_point   *p1,
    colour_point   *p2,
    Real           pos,
    Real           *r,
    Real           *g,
    Real           *b,
    Real           *a );

static void  recreate_piecewise_function(
    colour_coding_struct  *colour_coding,
    Colour_coding_types   type,
    BOOLEAN               set_user_defined );

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

BICAPI void  initialize_colour_coding(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type,
    Colour                 under_colour,
    Colour                 over_colour,
    Real                   min_value,
    Real                   max_value )
{
    colour_coding->n_colour_points = 0;
    colour_coding->user_defined_n_colour_points = 0;

    set_colour_coding_type( colour_coding, type );
    set_colour_coding_min_max( colour_coding, min_value, max_value );
    set_colour_coding_under_colour( colour_coding, under_colour );
    set_colour_coding_over_colour( colour_coding, over_colour );

    recreate_piecewise_function( colour_coding, GRAY_SCALE, TRUE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : delete_colour_coding
@INPUT      : colour_coding
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Nov. 25, 1996    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  delete_colour_coding(
    colour_coding_struct   *colour_coding )
{
    if( colour_coding->n_colour_points > 0 )
        FREE( colour_coding->colour_points );
    if( colour_coding->user_defined_n_colour_points > 0 )
        FREE( colour_coding->user_defined_colour_points );
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

BICAPI void  set_colour_coding_type(
    colour_coding_struct   *colour_coding,
    Colour_coding_types    type )
{
    colour_coding->type = type;

    recreate_piecewise_function( colour_coding, type, FALSE );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_coding_type
@INPUT      : colour_coding
@OUTPUT     : 
@RETURNS    : type of colour_coding
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Nov. 25, 1996    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI Colour_coding_types  get_colour_coding_type(
    colour_coding_struct   *colour_coding )
{
    return( colour_coding->type );
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

BICAPI void  set_colour_coding_min_max(
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

BICAPI void  get_colour_coding_min_max(
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

BICAPI Colour  get_colour_coding_under_colour(
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

BICAPI void  set_colour_coding_under_colour(
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

BICAPI Colour  get_colour_coding_over_colour(
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

BICAPI void  set_colour_coding_over_colour(
    colour_coding_struct   *colour_coding,
    Colour                 over_colour )
{
    colour_coding->over_colour = over_colour;

    /*--- force regeneration of the piecewise colour function, if needed,
          since the over colour determines the colour map in the case
          of SINGLE_COLOUR_SCALE */

    if( get_colour_coding_type( colour_coding ) == SINGLE_COLOUR_SCALE )
    {
        recreate_piecewise_function( colour_coding,
                                     get_colour_coding_type(colour_coding),
                                     FALSE );
    }
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
@CREATED    :          1993    David MacDonald
@MODIFIED   : Nov. 25, 1996    D. MacDonald  - now stores the table in the
                                               structure
---------------------------------------------------------------------------- */

static void  recreate_piecewise_function(
    colour_coding_struct  *colour_coding,
    Colour_coding_types   type,
    BOOLEAN               set_user_defined )
{
    static  colour_point  gray_scale_points[] =
                             { {0.0, 0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                               {1.0, 1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  hot_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.5, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.5,  1.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {0.75, 1.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  hot_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 1.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {0.5,  1.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {0.75, 0.5, 0.0, 0.0, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  cold_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {0.5,  0.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {0.75, 0.5, 1.0, 1.0, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  cold_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 0.5, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.5,  0.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {0.75, 0.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  green_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {0.5,  0.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {0.75, 0.5, 1.0, 1.0, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  green_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 0.5, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.5,  0.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {0.75, 0.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  lime_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {0.5,  0.5, 1.0, 0.0, 1.0, RGB_SPACE },
                         {0.75, 1.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  lime_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 1.0, 1.0, 0.5, 1.0, RGB_SPACE },
                         {0.5,  0.5, 1.0, 0.0, 1.0, RGB_SPACE },
                         {0.75, 0.0, 0.5, 0.0, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  red_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.5, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.5,  1.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {0.75, 1.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  red_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 1.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {0.5,  1.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {0.75, 0.5, 0.0, 0.0, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  purple_metal_points[] = 
                       { {0.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                         {0.25, 0.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {0.5,  0.5, 0.0, 1.0, 1.0, RGB_SPACE },
                         {0.75, 1.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {1.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  purple_metal_neg_points[] = 
                       { {0.0,  1.0, 1.0, 1.0, 1.0, RGB_SPACE },
                         {0.25, 1.0, 0.5, 1.0, 1.0, RGB_SPACE },
                         {0.5,  0.5, 0.0, 1.0, 1.0, RGB_SPACE },
                         {0.75, 0.0, 0.0, 0.5, 1.0, RGB_SPACE },
                         {1.0,  0.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  spectral_points[] =
                    { { 0.00, 0.0000,0.0000,0.0000, 1.0, RGB_SPACE},
                      { 0.05, 0.4667,0.0000,0.5333, 1.0, RGB_SPACE},
                      { 0.10, 0.5333,0.0000,0.6000, 1.0, RGB_SPACE},
                      { 0.15, 0.0000,0.0000,0.6667, 1.0, RGB_SPACE},
                      { 0.20, 0.0000,0.0000,0.8667, 1.0, RGB_SPACE},
                      { 0.25, 0.0000,0.4667,0.8667, 1.0, RGB_SPACE},
                      { 0.30, 0.0000,0.6000,0.8667, 1.0, RGB_SPACE},
                      { 0.35, 0.0000,0.6667,0.6667, 1.0, RGB_SPACE},
                      { 0.40, 0.0000,0.6667,0.5333, 1.0, RGB_SPACE},
                      { 0.45, 0.0000,0.6000,0.0000, 1.0, RGB_SPACE},
                      { 0.50, 0.0000,0.7333,0.0000, 1.0, RGB_SPACE},
                      { 0.55, 0.0000,0.8667,0.0000, 1.0, RGB_SPACE},
                      { 0.60, 0.0000,1.0000,0.0000, 1.0, RGB_SPACE},
                      { 0.65, 0.7333,1.0000,0.0000, 1.0, RGB_SPACE},
                      { 0.70, 0.9333,0.9333,0.0000, 1.0, RGB_SPACE},
                      { 0.75, 1.0000,0.8000,0.0000, 1.0, RGB_SPACE},
                      { 0.80, 1.0000,0.6000,0.0000, 1.0, RGB_SPACE},
                      { 0.85, 1.0000,0.0000,0.0000, 1.0, RGB_SPACE},
                      { 0.90, 0.8667,0.0000,0.0000, 1.0, RGB_SPACE},
                      { 0.95, 0.8000,0.0000,0.0000, 1.0, RGB_SPACE},
                      { 1.00, 0.8000,0.8000,0.8000, 1.0, RGB_SPACE}
                    };
    static  colour_point  red_points[] =
                             { {0.0, 0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                               {1.0, 1.0, 0.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  green_points[] =
                             { {0.0, 0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                               {1.0, 0.0, 1.0, 0.0, 1.0, RGB_SPACE } };
    static  colour_point  blue_points[] =
                             { {0.0, 0.0, 0.0, 0.0, 1.0, RGB_SPACE },
                               {1.0, 0.0, 0.0, 1.0, 1.0, RGB_SPACE } };
    static  colour_point  contour_points[] =
                      { {0.0,   0.0, 0.0, 0.3, 1.0, RGB_SPACE },
                        {0.166, 0.0, 0.0, 1.0, 1.0, RGB_SPACE },
                        {0.166, 0.0, 0.3, 0.3, 1.0, RGB_SPACE },
                        {0.333, 0.0, 1.0, 1.0, 1.0, RGB_SPACE },
                        {0.333, 0.0, 0.3, 0.0, 1.0, RGB_SPACE },
                        {0.5,   0.0, 1.0, 0.0, 1.0, RGB_SPACE },
                        {0.5,   0.3, 0.3, 0.0, 1.0, RGB_SPACE },
                        {0.666, 1.0, 1.0, 0.0, 1.0, RGB_SPACE },
                        {0.666, 0.3, 0.0, 0.0, 1.0, RGB_SPACE },
                        {0.833, 1.0, 0.0, 0.0, 1.0, RGB_SPACE },
                        {0.833, 0.3, 0.3, 0.3, 1.0, RGB_SPACE },
                        {1.0,   1.0, 1.0, 1.0, 1.0, RGB_SPACE } };

    static  colour_point  label256_points[] = {
        {0,   1.0,  1.0,  1.0, 1.0, RGB_SPACE },
        {0.00392156862745098,  0,  1,  0, 1.0, RGB_SPACE },
        {0.00784313725490196,  0,  0,  1, 1.0, RGB_SPACE },
        {0.0117647058823529,  1,  0,  0, 1.0, RGB_SPACE },
        {0.0156862745098039,  0,  0.96552,  1, 1.0, RGB_SPACE },
        {0.0196078431372549,  1,  0.48276,  0.86207, 1.0, RGB_SPACE },
        {0.0235294117647059,  1,  0.86207,  0.48276, 1.0, RGB_SPACE },
        {0.0274509803921569,  0,  0.55172,  1, 1.0, RGB_SPACE },
        {0.0313725490196078,  0,  0.41379,  0.034483, 1.0, RGB_SPACE },
        {0.0352941176470588,  0.62069,  0.27586,  0.2069, 1.0, RGB_SPACE },
        {0.0392156862745098,  0.24138,  0,  0.41379, 1.0, RGB_SPACE },
        {0.0431372549019608,  1,  0.93103,  1, 1.0, RGB_SPACE },
        {0.0470588235294118,  0.55172,  1,  0.62069, 1.0, RGB_SPACE },
        {0.0509803921568627,  0.89655,  0,  1, 1.0, RGB_SPACE },
        {0.0549019607843137,  0.31034,  0.48276,  0.55172, 1.0, RGB_SPACE },
        {0.0588235294117647,  0.86207,  1,  0, 1.0, RGB_SPACE },
        {0.0627450980392157,  1,  0,  0.41379, 1.0, RGB_SPACE },
        {0.0666666666666667,  0.93103,  0.51724,  0, 1.0, RGB_SPACE },
        {0.0705882352941176,  0.48276,  0.34483,  0.96552, 1.0, RGB_SPACE },
        {0.0745098039215686,  0.55172,  0,  0.34483, 1.0, RGB_SPACE },
        {0.0784313725490196,  0.48276,  0.41379,  0, 1.0, RGB_SPACE },
        {0.0823529411764706,  0.62069,  0.72414,  0.55172, 1.0, RGB_SPACE },
        {0.0862745098039216,  0.55172,  0.41379,  0.68966, 1.0, RGB_SPACE },
        {0.0901960784313725,  0.37931,  0.72414,  0, 1.0, RGB_SPACE },
        {0.0941176470588235,  1,  0.62069,  0.68966, 1.0, RGB_SPACE },
        {0.0980392156862745,  0,  0.17241,  0.37931, 1.0, RGB_SPACE },
        {0.101960784313725,  0.24138,  0.75862,  1, 1.0, RGB_SPACE },
        {0.105882352941176,  0.24138,  0.31034,  0.17241, 1.0, RGB_SPACE },
        {0.109803921568627,  0.65517,  0.034483,  0.65517, 1.0, RGB_SPACE },
        {0.113725490196078,  0.37931,  0.24138,  0.31034, 1.0, RGB_SPACE },
        {0.117647058823529,  0,  0.72414,  0.55172, 1.0, RGB_SPACE },
        {0.12156862745098,  1,  0.82759,  0, 1.0, RGB_SPACE },
        {0.125490196078431,  1,  0.41379,  0.31034, 1.0, RGB_SPACE },
        {0.129411764705882,  0,  0,  0.62069, 1.0, RGB_SPACE },
        {0.133333333333333,  0.27586,  0,  0, 1.0, RGB_SPACE },
        {0.137254901960784,  0.93103,  0.72414,  1, 1.0, RGB_SPACE },
        {0.141176470588235,  0.7931,  0.58621,  0.41379, 1.0, RGB_SPACE },
        {0.145098039215686,  1,  0,  0.65517, 1.0, RGB_SPACE },
        {0.149019607843137,  0.86207,  1,  0.48276, 1.0, RGB_SPACE },
        {0.152941176470588,  0.17241,  0.27586,  0.68966, 1.0, RGB_SPACE },
        {0.156862745098039,  0.62069,  0.65517,  0, 1.0, RGB_SPACE },
        {0.16078431372549,  0.58621,  0.82759,  0.86207, 1.0, RGB_SPACE },
        {0.164705882352941,  0.68966,  0.58621,  0.65517, 1.0, RGB_SPACE },
        {0.168627450980392,  0.7931,  0.51724,  1, 1.0, RGB_SPACE },
        {0.172549019607843,  0,  1,  0.44828, 1.0, RGB_SPACE },
        {0.176470588235294,  0.65517,  0,  0, 1.0, RGB_SPACE },
        {0.180392156862745,  0.51724,  0,  0.89655, 1.0, RGB_SPACE },
        {0.184313725490196,  0.17241,  0,  0.17241, 1.0, RGB_SPACE },
        {0.188235294117647,  1,  1,  0.7931, 1.0, RGB_SPACE },
        {0.192156862745098,  0.89655,  0.31034,  0.55172, 1.0, RGB_SPACE },
        {0.196078431372549,  0.68966,  1,  0.82759, 1.0, RGB_SPACE },
        {0.2,  0,  0.68966,  0.31034, 1.0, RGB_SPACE },
        {0.203921568627451,  0.34483,  0.2069,  0, 1.0, RGB_SPACE },
        {0.207843137254902,  0,  0.41379,  0.65517, 1.0, RGB_SPACE },
        {0.211764705882353,  0.58621,  0.65517,  0.89655, 1.0, RGB_SPACE },
        {0.215686274509804,  0.55172,  0.75862,  0.37931, 1.0, RGB_SPACE },
        {0.219607843137255,  0.13793,  0.48276,  0.37931, 1.0, RGB_SPACE },
        {0.223529411764706,  0.68966,  0.37931,  0.51724, 1.0, RGB_SPACE },
        {0.227450980392157,  0.68966,  0,  0.24138, 1.0, RGB_SPACE },
        {0.231372549019608,  0.65517,  0.34483,  0, 1.0, RGB_SPACE },
        {0.235294117647059,  1,  0.24138,  0.89655, 1.0, RGB_SPACE },
        {0.23921568627451,  0,  0.24138,  0.27586, 1.0, RGB_SPACE },
        {0.243137254901961,  0.58621,  1,  0.24138, 1.0, RGB_SPACE },
        {0.247058823529412,  0.51724,  0.41379,  0.34483, 1.0, RGB_SPACE },
        {0.250980392156863,  0,  1,  0.7931, 1.0, RGB_SPACE },
        {0.254901960784314,  0.7931,  0.58621,  0.17241, 1.0, RGB_SPACE },
        {0.258823529411765,  1,  0,  0.24138, 1.0, RGB_SPACE },
        {0.262745098039216,  0.41379,  0.51724,  0.24138, 1.0, RGB_SPACE },
        {0.266666666666667,  0,  0.24138,  1, 1.0, RGB_SPACE },
        {0.270588235294118,  0,  0.65517,  0.68966, 1.0, RGB_SPACE },
        {0.274509803921569,  0.7931,  0.34483,  1, 1.0, RGB_SPACE },
        {0.27843137254902,  1,  0.7931,  0.72414, 1.0, RGB_SPACE },
        {0.282352941176471,  0.41379,  0.17241,  0.41379, 1.0, RGB_SPACE },
        {0.286274509803922,  1,  0.62069,  0.37931, 1.0, RGB_SPACE },
        {0.290196078431373,  0.51724,  0.27586,  0.72414, 1.0, RGB_SPACE },
        {0.294117647058824,  0.68966,  0.27586,  0.62069, 1.0, RGB_SPACE },
        {0.298039215686275,  0.58621,  0.58621,  1, 1.0, RGB_SPACE },
        {0.301960784313725,  1,  0.41379,  0.48276, 1.0, RGB_SPACE },
        {0.305882352941176,  0,  0.41379,  1, 1.0, RGB_SPACE },
        {0.309803921568627,  0.44828,  0.034483,  0.68966, 1.0, RGB_SPACE },
        {0.313725490196078,  0.68966,  0.65517,  0.31034, 1.0, RGB_SPACE },
        {0.317647058823529,  0.89655,  0.31034,  0, 1.0, RGB_SPACE },
        {0.32156862745098,  0,  0,  0.27586, 1.0, RGB_SPACE },
        {0.325490196078431,  0.2069,  0.13793,  0.068966, 1.0, RGB_SPACE },
        {0.329411764705882,  0.34483,  0.55172,  0, 1.0, RGB_SPACE },
        {0.333333333333333,  0,  0.13793,  0.034483, 1.0, RGB_SPACE },
        {0.337254901960784,  0.10345,  0.82759,  0.068966, 1.0, RGB_SPACE },
        {0.341176470588235,  0.48276,  0.17241,  0.24138, 1.0, RGB_SPACE },
        {0.345098039215686,  0.34483,  0.34483,  0.51724, 1.0, RGB_SPACE },
        {0.349019607843137,  0.93103,  0.93103,  0.27586, 1.0, RGB_SPACE },
        {0.352941176470588,  0,  0.24138,  0, 1.0, RGB_SPACE },
        {0.356862745098039,  0.82759,  0.89655,  0.82759, 1.0, RGB_SPACE },
        {0.36078431372549,  0.44828,  0,  0, 1.0, RGB_SPACE },
        {0.364705882352941,  0.7931,  0.96552,  0.65517, 1.0, RGB_SPACE },
        {0.368627450980392,  0.65517,  0.82759,  0.034483, 1.0, RGB_SPACE },
        {0.372549019607843,  0.96552,  0.55172,  0.48276, 1.0, RGB_SPACE },
        {0.376470588235294,  0.10345,  0.55172,  0.31034, 1.0, RGB_SPACE },
        {0.380392156862745,  0.44828,  0.55172,  0.48276, 1.0, RGB_SPACE },
        {0.384313725490196,  0.75862,  0,  0.51724, 1.0, RGB_SPACE },
        {0.388235294117647,  0,  0.068966,  0.17241, 1.0, RGB_SPACE },
        {0.392156862745098,  0.41379,  0.86207,  0.7931, 1.0, RGB_SPACE },
        {0.396078431372549,  0.27586,  0.31034,  0, 1.0, RGB_SPACE },
        {0.4,  0.58621,  0.37931,  0.17241, 1.0, RGB_SPACE },
        {0.403921568627451,  0.72414,  0.44828,  0.41379, 1.0, RGB_SPACE },
        {0.407843137254902,  0,  0.10345,  0.55172, 1.0, RGB_SPACE },
        {0.411764705882353,  0,  1,  0.62069, 1.0, RGB_SPACE },
        {0.415686274509804,  1,  0.68966,  0.034483, 1.0, RGB_SPACE },
        {0.419607843137255,  0.48276,  0.7931,  0.55172, 1.0, RGB_SPACE },
        {0.423529411764706,  0.48276,  0.93103,  0.37931, 1.0, RGB_SPACE },
        {0.427450980392157,  0.31034,  0.34483,  0.34483, 1.0, RGB_SPACE },
        {0.431372549019608,  0.86207,  0.51724,  0.75862, 1.0, RGB_SPACE },
        {0.435294117647059,  0.44828,  0,  0.44828, 1.0, RGB_SPACE },
        {0.43921568627451,  0.82759,  0.75862,  0.55172, 1.0, RGB_SPACE },
        {0.443137254901961,  0.72414,  0.75862,  0.86207, 1.0, RGB_SPACE },
        {0.447058823529412,  0.31034,  0.37931,  0.68966, 1.0, RGB_SPACE },
        {0.450980392156863,  0.34483,  0,  0.2069, 1.0, RGB_SPACE },
        {0.454901960784314,  0.75862,  0.31034,  0.37931, 1.0, RGB_SPACE },
        {0.458823529411765,  0.27586,  0.2069,  0.48276, 1.0, RGB_SPACE },
        {0.462745098039216,  0.86207,  0.75862,  0.2069, 1.0, RGB_SPACE },
        {0.466666666666667,  0.72414,  0.65517,  0.58621, 1.0, RGB_SPACE },
        {0.470588235294118,  0.86207,  0.37931,  0.89655, 1.0, RGB_SPACE },
        {0.474509803921569,  0.55172,  0.48276,  0.27586, 1.0, RGB_SPACE },
        {0.47843137254902,  0,  0.55172,  0.72414, 1.0, RGB_SPACE },
        {0.482352941176471,  0.034483,  0.24138,  0.82759, 1.0, RGB_SPACE },
        {0.486274509803922,  0.44828,  0.41379,  0.89655, 1.0, RGB_SPACE },
        {0.490196078431373,  1,  0.75862,  0.89655, 1.0, RGB_SPACE },
        {0.494117647058824,  0.27586,  0.58621,  0.89655, 1.0, RGB_SPACE },
        {0.498039215686275,  0,  0.58621,  0, 1.0, RGB_SPACE },
        {0.501960784313725,  0.17241,  0.034483,  0.068966, 1.0, RGB_SPACE },
        {0.505882352941176,  0.75862,  0.31034,  0.17241, 1.0, RGB_SPACE },
        {0.509803921568627,  1,  0.75862,  0.51724, 1.0, RGB_SPACE },
        {0.513725490196078,  0.17241,  0.17241,  0.2069, 1.0, RGB_SPACE },
        {0.517647058823529,  0.2069,  0.86207,  1, 1.0, RGB_SPACE },
        {0.52156862745098,  0.82759,  0.2069,  0.24138, 1.0, RGB_SPACE },
        {0.525490196078431,  0.55172,  0.41379,  0.55172, 1.0, RGB_SPACE },
        {0.529411764705882,  0.41379,  0.68966,  0.58621, 1.0, RGB_SPACE },
        {0.533333333333333,  0.13793,  0,  0.7931, 1.0, RGB_SPACE },
        {0.537254901960784,  0.82759,  0.86207,  0.48276, 1.0, RGB_SPACE },
        {0.541176470588235,  0.48276,  0.2069,  0, 1.0, RGB_SPACE },
        {0.545098039215686,  0.65517,  0,  0.82759, 1.0, RGB_SPACE },
        {0.549019607843137,  0.068966,  0.31034,  0.44828, 1.0, RGB_SPACE },
        {0.552941176470588,  0.034483,  0.31034,  0.24138, 1.0, RGB_SPACE },
        {0.556862745098039,  0.96552,  0.62069,  1, 1.0, RGB_SPACE },
        {0.56078431372549,  0.44828,  0.68966,  0.82759, 1.0, RGB_SPACE },
        {0.564705882352941,  0.41379,  0.24138,  0.2069, 1.0, RGB_SPACE },
        {0.568627450980392,  0.2069,  0.41379,  0.2069, 1.0, RGB_SPACE },
        {0.572549019607843,  0.86207,  0.24138,  0.72414, 1.0, RGB_SPACE },
        {0.576470588235294,  0,  0.7931,  0.27586, 1.0, RGB_SPACE },
        {0.580392156862745,  0.24138,  0.17241,  0.31034, 1.0, RGB_SPACE },
        {0.584313725490196,  0.96552,  0.034483,  0.51724, 1.0, RGB_SPACE },
        {0.588235294117647,  0.72414,  0.58621,  0.7931, 1.0, RGB_SPACE },
        {0.592156862745098,  0.86207,  0.068966,  0.068966, 1.0, RGB_SPACE },
        {0.596078431372549,  0.68966,  1,  0.96552, 1.0, RGB_SPACE },
        {0.6,  0.24138,  0.86207,  0.58621, 1.0, RGB_SPACE },
        {0.603921568627451,  0.65517,  0.7931,  0.27586, 1.0, RGB_SPACE },
        {0.607843137254902,  0.51724,  0.55172,  0.68966, 1.0, RGB_SPACE },
        {0.611764705882353,  0.34483,  0.2069,  0.72414, 1.0, RGB_SPACE },
        {0.615686274509804,  0.75862,  0.58621,  0.93103, 1.0, RGB_SPACE },
        {0.619607843137255,  0.37931,  0.44828,  0, 1.0, RGB_SPACE },
        {0.623529411764706,  1,  0.51724,  0.72414, 1.0, RGB_SPACE },
        {0.627450980392157,  0.72414,  0,  0.37931, 1.0, RGB_SPACE },
        {0.631372549019608,  0.65517,  0.93103,  0.48276, 1.0, RGB_SPACE },
        {0.635294117647059,  1,  0.96552,  0, 1.0, RGB_SPACE },
        {0.63921568627451,  1,  0.75862,  0.31034, 1.0, RGB_SPACE },
        {0.643137254901961,  0.86207,  0.41379,  0.13793, 1.0, RGB_SPACE },
        {0.647058823529412,  0.48276,  0,  0.13793, 1.0, RGB_SPACE },
        {0.650980392156863,  0.75862,  1,  0.34483, 1.0, RGB_SPACE },
        {0.654901960784314,  0.62069,  0.72414,  0.68966, 1.0, RGB_SPACE },
        {0.658823529411765,  0.72414,  0.27586,  0.51724, 1.0, RGB_SPACE },
        {0.662745098039216,  0.7931,  0.48276,  0.27586, 1.0, RGB_SPACE },
        {0.666666666666667,  0.68966,  0.41379,  0.7931, 1.0, RGB_SPACE },
        {0.670588235294118,  0.31034,  0.58621,  0.24138, 1.0, RGB_SPACE },
        {0.674509803921569,  0,  0.24138,  0.55172, 1.0, RGB_SPACE },
        {0.67843137254902,  0.34483,  0.17241,  0.82759, 1.0, RGB_SPACE },
        {0.682352941176471,  0.96552,  0.31034,  0.68966, 1.0, RGB_SPACE },
        {0.686274509803922,  0.62069,  0.55172,  0.034483, 1.0, RGB_SPACE },
        {0.690196078431373,  0.48276,  0.48276,  0.51724, 1.0, RGB_SPACE },
        {0.694117647058824,  0.51724,  0.2069,  0.37931, 1.0, RGB_SPACE },
        {0.698039215686274,  0.41379,  0.62069,  0.41379, 1.0, RGB_SPACE },
        {0.701960784313725,  0.58621,  0.37931,  0.41379, 1.0, RGB_SPACE },
        {0.705882352941177,  0.41379,  0.7931,  0.34483, 1.0, RGB_SPACE },
        {0.709803921568627,  0.82759,  0.62069,  0.62069, 1.0, RGB_SPACE },
        {0.713725490196078,  0.62069,  0.34483,  0.58621, 1.0, RGB_SPACE },
        {0.717647058823529,  0.10345,  0.13793,  0.48276, 1.0, RGB_SPACE },
        {0.72156862745098,  0.24138,  0.034483,  0.31034, 1.0, RGB_SPACE },
        {0.725490196078431,  0.86207,  0.75862,  0.7931, 1.0, RGB_SPACE },
        {0.729411764705882,  0.58621,  0.24138,  0.93103, 1.0, RGB_SPACE },
        {0.733333333333333,  0.65517,  0.37931,  0.93103, 1.0, RGB_SPACE },
        {0.737254901960784,  0.37931,  0.31034,  0.17241, 1.0, RGB_SPACE },
        {0.741176470588235,  0,  0.41379,  0.41379, 1.0, RGB_SPACE },
        {0.745098039215686,  1,  0.96552,  0.48276, 1.0, RGB_SPACE },
        {0.749019607843137,  0.34483,  1,  0.89655, 1.0, RGB_SPACE },
        {0.752941176470588,  0.82759,  0.93103,  1, 1.0, RGB_SPACE },
        {0.756862745098039,  0.37931,  0.13793,  1, 1.0, RGB_SPACE },
        {0.76078431372549,  0.41379,  0.034483,  0.55172, 1.0, RGB_SPACE },
        {0.764705882352941,  0.51724,  0.24138,  0.58621, 1.0, RGB_SPACE },
        {0.768627450980392,  0.41379,  0.44828,  0.31034, 1.0, RGB_SPACE },
        {0.772549019607843,  0.51724,  1,  0.75862, 1.0, RGB_SPACE },
        {0.776470588235294,  0.62069,  0,  0.51724, 1.0, RGB_SPACE },
        {0.780392156862745,  0.37931,  0.62069,  0.62069, 1.0, RGB_SPACE },
        {0.784313725490196,  0.7931,  0,  0.7931, 1.0, RGB_SPACE },
        {0.788235294117647,  0.13793,  0.41379,  0.86207, 1.0, RGB_SPACE },
        {0.792156862745098,  0.13793,  0.17241,  0, 1.0, RGB_SPACE },
        {0.796078431372549,  0.86207,  0.37931,  0.34483, 1.0, RGB_SPACE },
        {0.8,  0.44828,  0.48276,  0.72414, 1.0, RGB_SPACE },
        {0.803921568627451,  0.58621,  0.068966,  0.27586, 1.0, RGB_SPACE },
        {0.807843137254902,  0.82759,  0.7931,  1, 1.0, RGB_SPACE },
        {0.811764705882353,  0.72414,  0,  1, 1.0, RGB_SPACE },
        {0.815686274509804,  0,  0.62069,  0.55172, 1.0, RGB_SPACE },
        {0.819607843137255,  0.82759,  0.58621,  0.72414, 1.0, RGB_SPACE },
        {0.823529411764706,  0.17241,  0.34483,  0, 1.0, RGB_SPACE },
        {0.827450980392157,  0.37931,  0.13793,  0.034483, 1.0, RGB_SPACE },
        {0.831372549019608,  0.48276,  0.37931,  0.75862, 1.0, RGB_SPACE },
        {0.835294117647059,  1,  0.68966,  0.58621, 1.0, RGB_SPACE },
        {0.83921568627451,  0.51724,  0.62069,  0.24138, 1.0, RGB_SPACE },
        {0.843137254901961,  0.10345,  0.068966,  0.24138, 1.0, RGB_SPACE },
        {0.847058823529412,  0.65517,  0.82759,  1, 1.0, RGB_SPACE },
        {0.850980392156863,  0.37931,  1,  0.31034, 1.0, RGB_SPACE },
        {0.854901960784314,  0.96552,  0.62069,  0.24138, 1.0, RGB_SPACE },
        {0.858823529411765,  0,  0.68966,  0.82759, 1.0, RGB_SPACE },
        {0.862745098039216,  0.24138,  0,  0.55172, 1.0, RGB_SPACE },
        {0.866666666666667,  0.86207,  0.44828,  0.55172, 1.0, RGB_SPACE },
        {0.870588235294118,  0.34483,  0.2069,  0.58621, 1.0, RGB_SPACE },
        {0.874509803921569,  0,  0.44828,  0.7931, 1.0, RGB_SPACE },
        {0.87843137254902,  0.65517,  0.51724,  0.2069, 1.0, RGB_SPACE },
        {0.882352941176471,  0.27586,  0.10345,  0.2069, 1.0, RGB_SPACE },
        {0.886274509803922,  0.27586,  0.41379,  0.34483, 1.0, RGB_SPACE },
        {0.890196078431372,  0.89655,  0.2069,  0.41379, 1.0, RGB_SPACE },
        {0.894117647058824,  0.10345,  0,  0.10345, 1.0, RGB_SPACE },
        {0.898039215686275,  0.62069,  0,  0.10345, 1.0, RGB_SPACE },
        {0.901960784313726,  1,  0.27586,  0.37931, 1.0, RGB_SPACE },
        {0.905882352941176,  0.62069,  0.68966,  0.41379, 1.0, RGB_SPACE },
        {0.909803921568627,  0.62069,  0.86207,  0.75862, 1.0, RGB_SPACE },
        {0.913725490196078,  0.41379,  0.27586,  0.44828, 1.0, RGB_SPACE },
        {0.917647058823529,  0,  0.51724,  0.58621, 1.0, RGB_SPACE },
        {0.92156862745098,  0.82759,  0.96552,  0.7931, 1.0, RGB_SPACE },
        {0.925490196078431,  0.58621,  0.17241,  0, 1.0, RGB_SPACE },
        {0.929411764705882,  0.96552,  0.89655,  0.7931, 1.0, RGB_SPACE },
        {0.933333333333333,  0.58621,  0.86207,  0.27586, 1.0, RGB_SPACE },
        {0.937254901960784,  0.31034,  0.48276,  0.65517, 1.0, RGB_SPACE },
        {0.941176470588235,  0.68966,  0.96552,  0, 1.0, RGB_SPACE },
        {0.945098039215686,  0.65517,  0.62069,  0.44828, 1.0, RGB_SPACE },
        {0.949019607843137,  0.93103,  0.27586,  1, 1.0, RGB_SPACE },
        {0.952941176470588,  0.65517,  0.24138,  0.7931, 1.0, RGB_SPACE },
        {0.956862745098039,  0.96552,  0.89655,  0.62069, 1.0, RGB_SPACE },
        {0.96078431372549,  0.34483,  0.93103,  0.48276, 1.0, RGB_SPACE },
        {0.964705882352941,  0.44828,  0.89655,  0.93103, 1.0, RGB_SPACE },
        {0.968627450980392,  0,  0,  0.37931, 1.0, RGB_SPACE },
        {0.972549019607843,  0.7931,  0.7931,  0.31034, 1.0, RGB_SPACE },
        {0.976470588235294,  0.17241,  0.2069,  0.13793, 1.0, RGB_SPACE },
        {0.980392156862745,  0.82759,  0.24138,  0.13793, 1.0, RGB_SPACE },
        {0.984313725490196,  0.82759,  0.65517,  0.34483, 1.0, RGB_SPACE },
        {0.988235294117647,  0.37931,  0.86207,  0.68966, 1.0, RGB_SPACE },
        {0.992156862745098,  1,  0,  0.75862, 1.0, RGB_SPACE },
        {0.996078431372549,  0.7931,  0.86207,  0.62069, 1.0, RGB_SPACE },
        {1,  0.41379,  0.51724,  0.86207, 1.0, RGB_SPACE }
        };

    colour_point          *points, **points_ptr;
    int                   p, n_points, *n_points_ptr;
    Real                  r, g, b, a;

    switch( type )
    {
    case  GRAY_SCALE:
    case  SINGLE_COLOUR_SCALE:
        n_points = SIZEOF_STATIC_ARRAY( gray_scale_points );
        points = gray_scale_points;
        break;

    case  HOT_METAL:
        n_points = SIZEOF_STATIC_ARRAY( hot_metal_points );
        points = hot_metal_points;
        break;

    case  HOT_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( hot_metal_neg_points );
        points = hot_metal_neg_points;
        break;

    case  COLD_METAL:
        n_points = SIZEOF_STATIC_ARRAY( cold_metal_points );
        points = cold_metal_points;
        break;

    case  COLD_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( cold_metal_neg_points );
        points = cold_metal_neg_points;
        break;

    case  GREEN_METAL:
        n_points = SIZEOF_STATIC_ARRAY( green_metal_points );
        points = green_metal_points;
        break;

    case  GREEN_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( green_metal_neg_points );
        points = green_metal_neg_points;
        break;

    case  LIME_METAL:
        n_points = SIZEOF_STATIC_ARRAY( lime_metal_points );
        points = lime_metal_points;
        break;

    case  LIME_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( lime_metal_neg_points );
        points = lime_metal_neg_points;
        break;

    case  RED_METAL:
        n_points = SIZEOF_STATIC_ARRAY( red_metal_points );
        points = red_metal_points;
        break;

    case  RED_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( red_metal_neg_points );
        points = red_metal_neg_points;
        break;

    case  PURPLE_METAL:
        n_points = SIZEOF_STATIC_ARRAY( purple_metal_points );
        points = purple_metal_points;
        break;

    case  PURPLE_METAL_NEG:
        n_points = SIZEOF_STATIC_ARRAY( purple_metal_neg_points );
        points = purple_metal_neg_points;
        break;

    case  SPECTRAL:
        n_points = SIZEOF_STATIC_ARRAY( spectral_points );
        points = spectral_points;
        break;

    case  RED_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( red_points );
        points = red_points;
        break;

    case  GREEN_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( green_points );
        points = green_points;
        break;

    case  BLUE_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( blue_points );
        points = blue_points;
        break;

    case  CONTOUR_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( contour_points );
        points = contour_points;
        break;

    case  LABEL256_COLOUR_MAP:
        n_points = SIZEOF_STATIC_ARRAY( label256_points );
        points = label256_points;
        break;

    case  USER_DEFINED_COLOUR_MAP:
        n_points = colour_coding->user_defined_n_colour_points;
        points = colour_coding->user_defined_colour_points;
        break;
    }

    if( set_user_defined )
    {
        n_points_ptr = &colour_coding->user_defined_n_colour_points;
        points_ptr = &colour_coding->user_defined_colour_points;
    }
    else
    {
        n_points_ptr = &colour_coding->n_colour_points;
        points_ptr = &colour_coding->colour_points;
    }

    if( n_points != *n_points_ptr )
    {
        if( *n_points_ptr > 0 )
            FREE( *points_ptr );

        *n_points_ptr = n_points;
        ALLOC( *points_ptr, n_points );
    }

    for_less( p, 0, n_points )
        (*points_ptr)[p] = points[p];

    if( type == SINGLE_COLOUR_SCALE )
    {
        r = get_Colour_r_0_1( colour_coding->over_colour );
        g = get_Colour_g_0_1( colour_coding->over_colour );
        b = get_Colour_b_0_1( colour_coding->over_colour );
        a = get_Colour_a_0_1( colour_coding->over_colour );

        for_less( p, 0, n_points )
        {
            (*points_ptr)[p].r *= r;
            (*points_ptr)[p].g *= g;
            (*points_ptr)[p].b *= b;
            (*points_ptr)[p].a *= a;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : define_colour_coding_user_defined
@INPUT      : colour_code
              n_colours
              colours
              positions
              interpolation_space
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the piecewise function of the user defined colour coding.
              The positions must be monotonically between 0 and 1.
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Nov. 25, 1996    David MacDonald
@MODIFIED   : Apr. 16, 1997    D. MacDonald - now rescales to [0..1]
---------------------------------------------------------------------------- */

BICAPI BOOLEAN  define_colour_coding_user_defined(
    colour_coding_struct  *colour_code,
    int                   n_colours,
    Colour                colours[],
    Real                  positions[],
    Colour_spaces         interpolation_space )
{
    Real      pos;
    int       p;

    if( n_colours < 2 )
    {
        print( "User defined colour coding must have at least 2 colours.\n" );
        return( FALSE );
    }

    for_less( p, 0, n_colours )
    {
        if( p > 0 && positions[p-1] > positions[p] )
        {
            print( "User defined colour coding must have\n" );
            print( "monotonic positions.\n" );
            return( FALSE );
        }
    }

    if( positions[0] == positions[n_colours-1] )
    {
        print(
          "User defined colour coding must have non-empty position range.\n" );
        return( FALSE );
    }

    if( colour_code->user_defined_n_colour_points > 0 )
        FREE( colour_code->user_defined_colour_points );

    colour_code->user_defined_n_colour_points = n_colours;
    ALLOC( colour_code->user_defined_colour_points, n_colours );

    for_less( p, 0, n_colours )
    {
        if( p == 0 )
            pos = 0.0;
        else if( p == n_colours - 1 )
            pos = 1.0;
        else
            pos = (positions[p] - positions[0]) /
                  (positions[n_colours-1] - positions[0]);

        colour_code->user_defined_colour_points[p].position = pos;
        colour_code->user_defined_colour_points[p].r =
                                             get_Colour_r_0_1(colours[p]);
        colour_code->user_defined_colour_points[p].g =
                                             get_Colour_g_0_1(colours[p]);
        colour_code->user_defined_colour_points[p].b =
                                             get_Colour_b_0_1(colours[p]);
        colour_code->user_defined_colour_points[p].a =
                                             get_Colour_a_0_1(colours[p]);
        colour_code->user_defined_colour_points[p].interpolation_space =
                                             interpolation_space;
    }

    if( get_colour_coding_type( colour_code ) == USER_DEFINED_COLOUR_MAP )
    {
        recreate_piecewise_function( colour_code,
                                     get_colour_coding_type(colour_code),
                                     FALSE );
    }

    return( TRUE );
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

BICAPI Colour  get_colour_code(
    colour_coding_struct  *colour_coding,
    Real                  value )
{
    Real           pos, r, g, b, a;
    int            i, n_points;
    colour_point   *points;

    if( colour_coding->min_value <= colour_coding->max_value )
    {
        if( value < colour_coding->min_value )
            return( colour_coding->under_colour );
        else if( value >= colour_coding->max_value )
            return( colour_coding->over_colour );
    }
    else
    {
        if( value > colour_coding->min_value )
            return( colour_coding->under_colour );
        else if( value <= colour_coding->max_value )
            return( colour_coding->over_colour );
    }

    pos = (value - colour_coding->min_value) /
          (colour_coding->max_value - colour_coding->min_value);

    n_points = colour_coding->n_colour_points;

    if( n_points < 2 )
    {
        print_error( "get_colour_code(): invalid piecewise function.\n" );
        return( make_rgba_Colour( 0, 0, 0, 0 ) );
    }

    points = colour_coding->colour_points;

    for_less( i, 0, n_points-1 ) {
        if( pos <= points[i+1].position ) break;
    }

    interpolate_colours( &points[i], &points[i+1], pos, &r, &g, &b, &a );

    return( make_rgba_Colour_0_1( r, g, b, a ) );
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

static void  interpolate_colours(
    colour_point   *p1,
    colour_point   *p2,
    Real           pos,
    Real           *r,
    Real           *g,
    Real           *b,
    Real           *a )
{
    Real  ratio, r0, g0, b0, a0, r1, g1, b1, a1;

    ratio = (pos - p1->position) / (p2->position - p1->position);

    r0 = p1->r;
    g0 = p1->g;
    b0 = p1->b;
    a0 = p1->a;

    r1 = p2->r;
    g1 = p2->g;
    b1 = p2->b;
    a1 = p2->a;

    if( p1->interpolation_space == HSL_SPACE )
    {
        rgb_to_hsl( r0, g0, b0, &r0, &g0, &b0 );
        rgb_to_hsl( r1, g1, b1, &r1, &g1, &b1 );

        if( r0 == 1.0 && r1 < 0.5 )
            r0 = 0.0;
        if( r1 == 1.0 && r0 < 0.5 )
            r1 = 0.0;
    }

    *r = r0 + (r1 - r0) * ratio;
    *g = g0 + (g1 - g0) * ratio;
    *b = b0 + (b1 - b0) * ratio;
    *a = a0 + (a1 - a0) * ratio;

    if( p1->interpolation_space == HSL_SPACE )
        hsl_to_rgb( *r, *g, *b, r, g, b );
}
