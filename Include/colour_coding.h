#ifndef  DEF_COLOUR_CODING
#define  DEF_COLOUR_CODING

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

#ifndef lint
static char colour_coding_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/colour_coding.h,v 1.7 1996-12-09 20:20:23 david Exp $";
#endif

#include  <objects.h>

typedef enum { GRAY_SCALE,
               HOT_METAL,
               SPECTRAL,
               RED_COLOUR_MAP,
               GREEN_COLOUR_MAP,
               BLUE_COLOUR_MAP,
               CONTOUR_COLOUR_MAP,
               SINGLE_COLOUR_SCALE,
               USER_DEFINED_COLOUR_MAP }
             Colour_coding_types;

typedef struct {
    Real           position;
    Real           r, g, b, a;
    Colour_spaces  interpolation_space;
} colour_point;

typedef  struct
{
    Colour_coding_types   type;

    Colour                under_colour;
    Colour                over_colour;

    Real                  min_value;
    Real                  max_value;

    int                   n_colour_points;
    colour_point          *colour_points;

    int                   user_defined_n_colour_points;
    colour_point          *user_defined_colour_points;

} colour_coding_struct;

#endif
