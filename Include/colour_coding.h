#ifndef  DEF_COLOUR_CODING
#define  DEF_COLOUR_CODING

#include  <def_basic.h>

typedef enum { GRAY_SCALE,
               HOT_METAL,
               SPECTRAL,
               RED_COLOUR_MAP,
               GREEN_COLOUR_MAP,
               BLUE_COLOUR_MAP,
               CONTOUR_COLOUR_MAP,
               USER_DEFINED }
             Colour_coding_types;

typedef  struct
{
    Colour_coding_types   type;

    Colour                user_defined_min_colour;
    Colour                user_defined_max_colour;
    Colour_spaces         user_defined_interpolation_space;

    Colour                under_colour;
    Colour                over_colour;

    Real                  min_value;
    Real                  max_value;
} colour_coding_struct;

#endif
