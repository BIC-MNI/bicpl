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
static char colour_coding_rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Include/Attic/colour_coding.h,v 1.5 1995-07-31 13:44:40 david Exp $";
#endif

#include  <objects.h>

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
