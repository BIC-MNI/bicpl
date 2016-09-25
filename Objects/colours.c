
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

#include "bicpl_internal.h"

static VIO_Colour ALICE_BLUE_COL;
static VIO_Colour ANTIQUE_WHITE_COL;
static VIO_Colour AQUAMARINE_COL;
static VIO_Colour AZURE_COL;
static VIO_Colour BEIGE_COL;
static VIO_Colour BISQUE_COL;
static VIO_Colour BLACK_COL;
static VIO_Colour BLANCHED_ALMOND_COL;
static VIO_Colour BLUE_COL;
static VIO_Colour BLUE_VIOLET_COL;
static VIO_Colour BRIGHT_MUSTARD_COL;
static VIO_Colour BROWN_COL;
static VIO_Colour BURLYWOOD_COL;
static VIO_Colour CADET_BLUE_COL;
static VIO_Colour CHARTREUSE_COL;
static VIO_Colour CHOCOLATE_COL;
static VIO_Colour CORAL_COL;
static VIO_Colour CORNFLOWER_BLUE_COL;
static VIO_Colour CORNSILK_COL;
static VIO_Colour CYAN_COL;
static VIO_Colour DARK_BROWN_COL;
static VIO_Colour DARK_FIREBRICK_COL;
static VIO_Colour DARK_GOLDENROD_COL;
static VIO_Colour DARK_GREEN_COL;
static VIO_Colour DARK_KHAKI_COL;
static VIO_Colour DARK_OLIVE_GREEN_COL;
static VIO_Colour DARK_ORANGE_COL;
static VIO_Colour DARK_ORCHID_COL;
static VIO_Colour DARK_RED_COL;
static VIO_Colour DARK_SALMON_COL;
static VIO_Colour DARK_SEA_GREEN_COL;
static VIO_Colour DARK_SLATE_BLUE_COL;
static VIO_Colour DARK_SLATE_GRAY_COL;
static VIO_Colour DARK_SLATE_GREY_COL;
static VIO_Colour DARK_TURQUOISE_COL;
static VIO_Colour DARK_VIOLET_COL;
static VIO_Colour DEEP_PINK_COL;
static VIO_Colour DEEP_SKY_BLUE_COL;
static VIO_Colour DIM_GRAY_COL;
static VIO_Colour DIM_GREY_COL;
static VIO_Colour DODGER_BLUE_COL;
static VIO_Colour FIREBRICK_COL;
static VIO_Colour FLORAL_WHITE_COL;
static VIO_Colour FOREST_GREEN_COL;
static VIO_Colour GAINSBORO_COL;
static VIO_Colour GHOST_WHITE_COL;
static VIO_Colour GOLD_COL;
static VIO_Colour GOLDENROD_COL;
static VIO_Colour GRAY_COL;
static VIO_Colour GREEN_COL;
static VIO_Colour GREEN_YELLOW_COL;
static VIO_Colour GREY_COL;
static VIO_Colour HONEYDEW_COL;
static VIO_Colour HOT_PINK_COL;
static VIO_Colour INDIAN_RED_COL;
static VIO_Colour IVORY_COL;
static VIO_Colour KHAKI_COL;
static VIO_Colour LAVENDER_COL;
static VIO_Colour LAVENDER_BLUSH_COL;
static VIO_Colour LAWN_GREEN_COL;
static VIO_Colour LEMON_CHIFFON_COL;
static VIO_Colour LIGHT_BLUE_COL;
static VIO_Colour LIGHT_CORAL_COL;
static VIO_Colour LIGHT_CYAN_COL;
static VIO_Colour LIGHT_GOLDENROD_COL;
static VIO_Colour LIGHT_GOLDENROD_YELLOW_COL;
static VIO_Colour LIGHT_GRAY_COL;
static VIO_Colour LIGHT_GREY_COL;
static VIO_Colour LIGHT_PINK_COL;
static VIO_Colour LIGHT_SALMON_COL;
static VIO_Colour LIGHT_SEA_GREEN_COL;
static VIO_Colour LIGHT_SKY_BLUE_COL;
static VIO_Colour LIGHT_SLATE_BLUE_COL;
static VIO_Colour LIGHT_SLATE_GRAY_COL;
static VIO_Colour LIGHT_SLATE_GREY_COL;
static VIO_Colour LIGHT_STEEL_BLUE_COL;
static VIO_Colour LIGHT_YELLOW_COL;
static VIO_Colour LIME_GREEN_COL;
static VIO_Colour LINEN_COL;
static VIO_Colour MAGENTA_COL;
static VIO_Colour MAROON_COL;
static VIO_Colour MEDIUM_AQUAMARINE_COL;
static VIO_Colour MEDIUM_BLUE_COL;
static VIO_Colour MEDIUM_ORCHID_COL;
static VIO_Colour MEDIUM_PURPLE_COL;
static VIO_Colour MEDIUM_SEA_GREEN_COL;
static VIO_Colour MEDIUM_SLATE_BLUE_COL;
static VIO_Colour MEDIUM_SPRING_GREEN_COL;
static VIO_Colour MEDIUM_TURQUOISE_COL;
static VIO_Colour MEDIUM_VIOLET_RED_COL;
static VIO_Colour MIAMI_PINK_COL;
static VIO_Colour MIAMI_TURQUOISE_COL;
static VIO_Colour MIDNIGHT_BLUE_COL;
static VIO_Colour MINT_CREAM_COL;
static VIO_Colour MISTY_ROSE_COL;
static VIO_Colour MOCCASIN_COL;
static VIO_Colour NAVAJO_WHITE_COL;
static VIO_Colour NAVY_COL;
static VIO_Colour NAVY_BLUE_COL;
static VIO_Colour OLD_LACE_COL;
static VIO_Colour OLIVE_DRAB_COL;
static VIO_Colour ORANGE_COL;
static VIO_Colour ORANGE_RED_COL;
static VIO_Colour ORCHID_COL;
static VIO_Colour PALE_GOLDENROD_COL;
static VIO_Colour PALE_GREEN_COL;
static VIO_Colour PALE_TURQUOISE_COL;
static VIO_Colour PALE_VIOLET_RED_COL;
static VIO_Colour PAPAYA_WHIP_COL;
static VIO_Colour PEACH_PUFF_COL;
static VIO_Colour PERU_COL;
static VIO_Colour PINK_COL;
static VIO_Colour PLUM_COL;
static VIO_Colour POWDER_BLUE_COL;
static VIO_Colour PURPLE_COL;
static VIO_Colour RED_COL;
static VIO_Colour ROSY_BROWN_COL;
static VIO_Colour ROYAL_BLUE_COL;
static VIO_Colour SADDLE_BROWN_COL;
static VIO_Colour SALMON_COL;
static VIO_Colour SANDY_BROWN_COL;
static VIO_Colour SEA_GREEN_COL;
static VIO_Colour SEASHELL_COL;
static VIO_Colour SIENNA_COL;
static VIO_Colour SKY_BLUE_COL;
static VIO_Colour SLATE_BLUE_COL;
static VIO_Colour SLATE_GRAY_COL;
static VIO_Colour SLATE_GREY_COL;
static VIO_Colour SNOW_COL;
static VIO_Colour SPRING_GREEN_COL;
static VIO_Colour STEEL_BLUE_COL;
static VIO_Colour TAN_COL;
static VIO_Colour THISTLE_COL;
static VIO_Colour TOMATO_COL;
static VIO_Colour TRANSPARENT_COL;
static VIO_Colour TURQUOISE_COL;
static VIO_Colour VIOLET_COL;
static VIO_Colour VIOLET_RED_COL;
static VIO_Colour WHEAT_COL;
static VIO_Colour WHITE_COL;
static VIO_Colour WHITE_SMOKE_COL;
static VIO_Colour YELLOW_COL;
static VIO_Colour YELLOW_GREEN_COL;

typedef  struct
{
    VIO_STR   name;
    VIO_Colour   *colour;
} colours_struct;

static  colours_struct  colour_lookup[] =
{
    { "ALICE_BLUE", &ALICE_BLUE_COL },
    { "ANTIQUE_WHITE", &ANTIQUE_WHITE_COL },
    { "AQUAMARINE", &AQUAMARINE_COL },
    { "AZURE", &AZURE_COL },
    { "BEIGE", &BEIGE_COL },
    { "BISQUE", &BISQUE_COL },
    { "BLACK", &BLACK_COL },
    { "BLANCHED_ALMOND", &BLANCHED_ALMOND_COL },
    { "BLUE", &BLUE_COL },
    { "BLUE_VIOLET", &BLUE_VIOLET_COL },
    { "BRIGHT_MUSTARD", &BRIGHT_MUSTARD_COL },
    { "BROWN", &BROWN_COL },
    { "BURLYWOOD", &BURLYWOOD_COL },
    { "CADET_BLUE", &CADET_BLUE_COL },
    { "CHARTREUSE", &CHARTREUSE_COL },
    { "CHOCOLATE", &CHOCOLATE_COL },
    { "CORAL", &CORAL_COL },
    { "CORNFLOWER_BLUE", &CORNFLOWER_BLUE_COL },
    { "CORNSILK", &CORNSILK_COL },
    { "CYAN", &CYAN_COL },
    { "DARK_BROWN", &DARK_BROWN_COL },
    { "DARK_FIREBRICK", &DARK_FIREBRICK_COL },
    { "DARK_GOLDENROD", &DARK_GOLDENROD_COL },
    { "DARK_GREEN", &DARK_GREEN_COL },
    { "DARK_KHAKI", &DARK_KHAKI_COL },
    { "DARK_OLIVE_GREEN", &DARK_OLIVE_GREEN_COL },
    { "DARK_ORANGE", &DARK_ORANGE_COL },
    { "DARK_ORCHID", &DARK_ORCHID_COL },
    { "DARK_RED", &DARK_RED_COL },
    { "DARK_SALMON", &DARK_SALMON_COL },
    { "DARK_SEA_GREEN", &DARK_SEA_GREEN_COL },
    { "DARK_SLATE_BLUE", &DARK_SLATE_BLUE_COL },
    { "DARK_SLATE_GRAY", &DARK_SLATE_GRAY_COL },
    { "DARK_SLATE_GREY", &DARK_SLATE_GREY_COL },
    { "DARK_TURQUOISE", &DARK_TURQUOISE_COL },
    { "DARK_VIOLET", &DARK_VIOLET_COL },
    { "DEEP_PINK", &DEEP_PINK_COL },
    { "DEEP_SKY_BLUE", &DEEP_SKY_BLUE_COL },
    { "DIM_GRAY", &DIM_GRAY_COL },
    { "DIM_GREY", &DIM_GREY_COL },
    { "DODGER_BLUE", &DODGER_BLUE_COL },
    { "FIREBRICK", &FIREBRICK_COL },
    { "FLORAL_WHITE", &FLORAL_WHITE_COL },
    { "FOREST_GREEN", &FOREST_GREEN_COL },
    { "GAINSBORO", &GAINSBORO_COL },
    { "GHOST_WHITE", &GHOST_WHITE_COL },
    { "GOLD", &GOLD_COL },
    { "GOLDENROD", &GOLDENROD_COL },
    { "GRAY", &GRAY_COL },
    { "GREEN", &GREEN_COL },
    { "GREEN_YELLOW", &GREEN_YELLOW_COL },
    { "GREY", &GREY_COL },
    { "HONEYDEW", &HONEYDEW_COL },
    { "HOT_PINK", &HOT_PINK_COL },
    { "INDIAN_RED", &INDIAN_RED_COL },
    { "IVORY", &IVORY_COL },
    { "KHAKI", &KHAKI_COL },
    { "LAVENDER", &LAVENDER_COL },
    { "LAVENDER_BLUSH", &LAVENDER_BLUSH_COL },
    { "LAWN_GREEN", &LAWN_GREEN_COL },
    { "LEMON_CHIFFON", &LEMON_CHIFFON_COL },
    { "LIGHT_BLUE", &LIGHT_BLUE_COL },
    { "LIGHT_CORAL", &LIGHT_CORAL_COL },
    { "LIGHT_CYAN", &LIGHT_CYAN_COL },
    { "LIGHT_GOLDENROD", &LIGHT_GOLDENROD_COL },
    { "LIGHT_GOLDENROD_YELLOW", &LIGHT_GOLDENROD_YELLOW_COL },
    { "LIGHT_GRAY", &LIGHT_GRAY_COL },
    { "LIGHT_GREY", &LIGHT_GREY_COL },
    { "LIGHT_PINK", &LIGHT_PINK_COL },
    { "LIGHT_SALMON", &LIGHT_SALMON_COL },
    { "LIGHT_SEA_GREEN", &LIGHT_SEA_GREEN_COL },
    { "LIGHT_SKY_BLUE", &LIGHT_SKY_BLUE_COL },
    { "LIGHT_SLATE_BLUE", &LIGHT_SLATE_BLUE_COL },
    { "LIGHT_SLATE_GRAY", &LIGHT_SLATE_GRAY_COL },
    { "LIGHT_SLATE_GREY", &LIGHT_SLATE_GREY_COL },
    { "LIGHT_STEEL_BLUE", &LIGHT_STEEL_BLUE_COL },
    { "LIGHT_YELLOW", &LIGHT_YELLOW_COL },
    { "LIME_GREEN", &LIME_GREEN_COL },
    { "LINEN", &LINEN_COL },
    { "MAGENTA", &MAGENTA_COL },
    { "MAROON", &MAROON_COL },
    { "MEDIUM_AQUAMARINE", &MEDIUM_AQUAMARINE_COL },
    { "MEDIUM_BLUE", &MEDIUM_BLUE_COL },
    { "MEDIUM_ORCHID", &MEDIUM_ORCHID_COL },
    { "MEDIUM_PURPLE", &MEDIUM_PURPLE_COL },
    { "MEDIUM_SEA_GREEN", &MEDIUM_SEA_GREEN_COL },
    { "MEDIUM_SLATE_BLUE", &MEDIUM_SLATE_BLUE_COL },
    { "MEDIUM_SPRING_GREEN", &MEDIUM_SPRING_GREEN_COL },
    { "MEDIUM_TURQUOISE", &MEDIUM_TURQUOISE_COL },
    { "MEDIUM_VIOLET_RED", &MEDIUM_VIOLET_RED_COL },
    { "MIAMI_PINK", &MIAMI_PINK_COL },
    { "MIAMI_TURQUOISE", &MIAMI_TURQUOISE_COL },
    { "MIDNIGHT_BLUE", &MIDNIGHT_BLUE_COL },
    { "MINT_CREAM", &MINT_CREAM_COL },
    { "MISTY_ROSE", &MISTY_ROSE_COL },
    { "MOCCASIN", &MOCCASIN_COL },
    { "NAVAJO_WHITE", &NAVAJO_WHITE_COL },
    { "NAVY", &NAVY_COL },
    { "NAVY_BLUE", &NAVY_BLUE_COL },
    { "OLD_LACE", &OLD_LACE_COL },
    { "OLIVE_DRAB", &OLIVE_DRAB_COL },
    { "ORANGE", &ORANGE_COL },
    { "ORANGE_RED", &ORANGE_RED_COL },
    { "ORCHID", &ORCHID_COL },
    { "PALE_GOLDENROD", &PALE_GOLDENROD_COL },
    { "PALE_GREEN", &PALE_GREEN_COL },
    { "PALE_TURQUOISE", &PALE_TURQUOISE_COL },
    { "PALE_VIOLET_RED", &PALE_VIOLET_RED_COL },
    { "PAPAYA_WHIP", &PAPAYA_WHIP_COL },
    { "PEACH_PUFF", &PEACH_PUFF_COL },
    { "PERU", &PERU_COL },
    { "PINK", &PINK_COL },
    { "PLUM", &PLUM_COL },
    { "POWDER_BLUE", &POWDER_BLUE_COL },
    { "PURPLE", &PURPLE_COL },
    { "RED", &RED_COL },
    { "ROSY_BROWN", &ROSY_BROWN_COL },
    { "ROYAL_BLUE", &ROYAL_BLUE_COL },
    { "SADDLE_BROWN", &SADDLE_BROWN_COL },
    { "SALMON", &SALMON_COL },
    { "SANDY_BROWN", &SANDY_BROWN_COL },
    { "SEASHELL", &SEASHELL_COL },
    { "SEA_GREEN", &SEA_GREEN_COL },
    { "SIENNA", &SIENNA_COL },
    { "SKY_BLUE", &SKY_BLUE_COL },
    { "SLATE_BLUE", &SLATE_BLUE_COL },
    { "SLATE_GRAY", &SLATE_GRAY_COL },
    { "SLATE_GREY", &SLATE_GREY_COL },
    { "SNOW", &SNOW_COL },
    { "SPRING_GREEN", &SPRING_GREEN_COL },
    { "STEEL_BLUE", &STEEL_BLUE_COL },
    { "TAN", &TAN_COL },
    { "THISTLE", &THISTLE_COL },
    { "TOMATO", &TOMATO_COL },
    { "TRANSPARENT", &TRANSPARENT_COL },
    { "TURQUOISE", &TURQUOISE_COL },
    { "VIOLET", &VIOLET_COL },
    { "VIOLET_RED", &VIOLET_RED_COL },
    { "WHEAT", &WHEAT_COL },
    { "WHITE", &WHITE_COL },
    { "WHITE_SMOKE", &WHITE_SMOKE_COL },
    { "YELLOW", &YELLOW_COL },
    { "YELLOW_GREEN", &YELLOW_GREEN_COL }
};

static   VIO_BOOL  strings_equivalent( VIO_STR, VIO_STR );

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_initialize_colours
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: The first time it is called, this function creates the global
              colour variables.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  void  check_initialize_colours( void )
{
    static  VIO_BOOL  first = TRUE;

    if( first )
    {
        first = FALSE;

        ALICE_BLUE_COL = ALICE_BLUE;
        ANTIQUE_WHITE_COL = ANTIQUE_WHITE;
        AQUAMARINE_COL = AQUAMARINE;
        AZURE_COL = AZURE;
        BEIGE_COL = BEIGE;
        BISQUE_COL = BISQUE;
        BLACK_COL = BLACK;
        BLANCHED_ALMOND_COL = BLANCHED_ALMOND;
        BLUE_COL = BLUE;
        BLUE_VIOLET_COL = BLUE_VIOLET;
        BRIGHT_MUSTARD_COL = BRIGHT_MUSTARD;
        BROWN_COL = BROWN;
        BURLYWOOD_COL = BURLYWOOD;
        CADET_BLUE_COL = CADET_BLUE;
        CHARTREUSE_COL = CHARTREUSE;
        CHOCOLATE_COL = CHOCOLATE;
        CORAL_COL = CORAL;
        CORNFLOWER_BLUE_COL = CORNFLOWER_BLUE;
        CORNSILK_COL = CORNSILK;
        CYAN_COL = CYAN;
        DARK_BROWN_COL = DARK_BROWN;
        DARK_FIREBRICK_COL = DARK_FIREBRICK;
        DARK_GOLDENROD_COL = DARK_GOLDENROD;
        DARK_GREEN_COL = DARK_GREEN;
        DARK_KHAKI_COL = DARK_KHAKI;
        DARK_OLIVE_GREEN_COL = DARK_OLIVE_GREEN;
        DARK_ORANGE_COL = DARK_ORANGE;
        DARK_ORCHID_COL = DARK_ORCHID;
        DARK_RED_COL = DARK_RED;
        DARK_SALMON_COL = DARK_SALMON;
        DARK_SEA_GREEN_COL = DARK_SEA_GREEN;
        DARK_SLATE_BLUE_COL = DARK_SLATE_BLUE;
        DARK_SLATE_GRAY_COL = DARK_SLATE_GRAY;
        DARK_SLATE_GREY_COL = DARK_SLATE_GREY;
        DARK_TURQUOISE_COL = DARK_TURQUOISE;
        DARK_VIOLET_COL = DARK_VIOLET;
        DEEP_PINK_COL = DEEP_PINK;
        DEEP_SKY_BLUE_COL = DEEP_SKY_BLUE;
        DIM_GRAY_COL = DIM_GRAY;
        DIM_GREY_COL = DIM_GREY;
        DODGER_BLUE_COL = DODGER_BLUE;
        FIREBRICK_COL = FIREBRICK;
        FLORAL_WHITE_COL = FLORAL_WHITE;
        FOREST_GREEN_COL = FOREST_GREEN;
        GAINSBORO_COL = GAINSBORO;
        GHOST_WHITE_COL = GHOST_WHITE;
        GOLDENROD_COL = GOLDENROD;
        GOLD_COL = GOLD;
        GRAY_COL = GRAY;
        GREEN_COL = GREEN;
        GREEN_YELLOW_COL = GREEN_YELLOW;
        GREY_COL = GREY;
        HONEYDEW_COL = HONEYDEW;
        HOT_PINK_COL = HOT_PINK;
        INDIAN_RED_COL = INDIAN_RED;
        IVORY_COL = IVORY;
        KHAKI_COL = KHAKI;
        LAVENDER_BLUSH_COL = LAVENDER_BLUSH;
        LAVENDER_COL = LAVENDER;
        LAWN_GREEN_COL = LAWN_GREEN;
        LEMON_CHIFFON_COL = LEMON_CHIFFON;
        LIGHT_BLUE_COL = LIGHT_BLUE;
        LIGHT_CORAL_COL = LIGHT_CORAL;
        LIGHT_CYAN_COL = LIGHT_CYAN;
        LIGHT_GOLDENROD_COL = LIGHT_GOLDENROD;
        LIGHT_GOLDENROD_YELLOW_COL = LIGHT_GOLDENROD_YELLOW;
        LIGHT_GRAY_COL = LIGHT_GRAY;
        LIGHT_GREY_COL = LIGHT_GREY;
        LIGHT_PINK_COL = LIGHT_PINK;
        LIGHT_SALMON_COL = LIGHT_SALMON;
        LIGHT_SEA_GREEN_COL = LIGHT_SEA_GREEN;
        LIGHT_SKY_BLUE_COL = LIGHT_SKY_BLUE;
        LIGHT_SLATE_BLUE_COL = LIGHT_SLATE_BLUE;
        LIGHT_SLATE_GRAY_COL = LIGHT_SLATE_GRAY;
        LIGHT_SLATE_GREY_COL = LIGHT_SLATE_GREY;
        LIGHT_STEEL_BLUE_COL = LIGHT_STEEL_BLUE;
        LIGHT_YELLOW_COL = LIGHT_YELLOW;
        LIME_GREEN_COL = LIME_GREEN;
        LINEN_COL = LINEN;
        MAGENTA_COL = MAGENTA;
        MAROON_COL = MAROON;
        MEDIUM_AQUAMARINE_COL = MEDIUM_AQUAMARINE;
        MEDIUM_BLUE_COL = MEDIUM_BLUE;
        MEDIUM_ORCHID_COL = MEDIUM_ORCHID;
        MEDIUM_PURPLE_COL = MEDIUM_PURPLE;
        MEDIUM_SEA_GREEN_COL = MEDIUM_SEA_GREEN;
        MEDIUM_SLATE_BLUE_COL = MEDIUM_SLATE_BLUE;
        MEDIUM_SPRING_GREEN_COL = MEDIUM_SPRING_GREEN;
        MEDIUM_TURQUOISE_COL = MEDIUM_TURQUOISE;
        MEDIUM_VIOLET_RED_COL = MEDIUM_VIOLET_RED;
        MIAMI_PINK_COL = MIAMI_PINK;
        MIAMI_TURQUOISE_COL = MIAMI_TURQUOISE;
        MIDNIGHT_BLUE_COL = MIDNIGHT_BLUE;
        MINT_CREAM_COL = MINT_CREAM;
        MISTY_ROSE_COL = MISTY_ROSE;
        MOCCASIN_COL = MOCCASIN;
        NAVAJO_WHITE_COL = NAVAJO_WHITE;
        NAVY_BLUE_COL = NAVY_BLUE;
        NAVY_COL = NAVY;
        OLD_LACE_COL = OLD_LACE;
        OLIVE_DRAB_COL = OLIVE_DRAB;
        ORANGE_COL = ORANGE;
        ORANGE_RED_COL = ORANGE_RED;
        ORCHID_COL = ORCHID;
        PALE_GOLDENROD_COL = PALE_GOLDENROD;
        PALE_GREEN_COL = PALE_GREEN;
        PALE_TURQUOISE_COL = PALE_TURQUOISE;
        PALE_VIOLET_RED_COL = PALE_VIOLET_RED;
        PAPAYA_WHIP_COL = PAPAYA_WHIP;
        PEACH_PUFF_COL = PEACH_PUFF;
        PERU_COL = PERU;
        PINK_COL = PINK;
        PLUM_COL = PLUM;
        POWDER_BLUE_COL = POWDER_BLUE;
        PURPLE_COL = PURPLE;
        RED_COL = RED;
        ROSY_BROWN_COL = ROSY_BROWN;
        ROYAL_BLUE_COL = ROYAL_BLUE;
        SADDLE_BROWN_COL = SADDLE_BROWN;
        SALMON_COL = SALMON;
        SANDY_BROWN_COL = SANDY_BROWN;
        SEASHELL_COL = SEASHELL;
        SEA_GREEN_COL = SEA_GREEN;
        SIENNA_COL = SIENNA;
        SKY_BLUE_COL = SKY_BLUE;
        SLATE_BLUE_COL = SLATE_BLUE;
        SLATE_GRAY_COL = SLATE_GRAY;
        SLATE_GREY_COL = SLATE_GREY;
        SNOW_COL = SNOW;
        SPRING_GREEN_COL = SPRING_GREEN;
        STEEL_BLUE_COL = STEEL_BLUE;
        TAN_COL = TAN;
        THISTLE_COL = THISTLE;
        TOMATO_COL = TOMATO;
        TURQUOISE_COL = TURQUOISE;
        VIOLET_COL = VIOLET;
        VIOLET_RED_COL = VIOLET_RED;
        WHEAT_COL = WHEAT;
        WHITE_COL = WHITE;
        WHITE_SMOKE_COL = WHITE_SMOKE;
        YELLOW_COL = YELLOW;
        YELLOW_GREEN_COL = YELLOW_GREEN;
        TRANSPARENT_COL = TRANSPARENT;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_default_surfprop
@INPUT      : 
@OUTPUT     : surfprop
@RETURNS    : 
@DESCRIPTION: Passes back a default surface property.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  get_default_surfprop(
    VIO_Surfprop  *surfprop )
{
    Surfprop_a(*surfprop) = 0.3f;
    Surfprop_d(*surfprop) = 0.3f;
    Surfprop_s(*surfprop) = 0.4f;
    Surfprop_se(*surfprop) = 10.0f;
    Surfprop_t(*surfprop) = 1.0f;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup_colour
@INPUT      : colour_name
@OUTPUT     : col
@RETURNS    : TRUE if colour found
@DESCRIPTION: Given a colour name, such as "red" or "Green", finds the
              corresponding VIO_Colour.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  lookup_colour(
    VIO_STR    colour_name,
    VIO_Colour    *col )
{
    VIO_BOOL  found;
    int      i;

    check_initialize_colours();

    found = FALSE;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(colour_lookup) )
    {
        if( strings_equivalent( colour_lookup[i].name, colour_name ) )
        {
            *col = *colour_lookup[i].colour;
            found = TRUE;
            break;
        }
    }

    return( found );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : lookup_colour_name
@INPUT      : col
@OUTPUT     : colour_name
@RETURNS    : TRUE if found
@DESCRIPTION: Given a colour, tries to find a named colour with the same
              values, and passes back the colour name.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  lookup_colour_name(
    VIO_Colour  col,
    VIO_STR  *colour_name )
{
    VIO_BOOL  found;
    int      i;

    check_initialize_colours();

    found = FALSE;

    for_less( i, 0, VIO_SIZEOF_STATIC_ARRAY(colour_lookup) )
    {
        if( col == *colour_lookup[i].colour )
        {
            *colour_name = create_string( colour_lookup[i].name );
            found = TRUE;
            break;
        }
    }

    return( found );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : strings_equivalent
@INPUT      : str1
              str2
@OUTPUT     : 
@RETURNS    : TRUE if strings equiv
@DESCRIPTION: Checks if two strings are equivalent, ignoring CASE and white
              space.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static  VIO_BOOL  strings_equivalent(
    VIO_STR   str1,
    VIO_STR   str2 )
{
    VIO_BOOL  equivalent;
    int      i1, i2;

    equivalent = TRUE;
    i1 = 0;
    i2 = 0;

    while( str1[i1] != VIO_END_OF_STRING || str2[i2] != VIO_END_OF_STRING )
    {
        if( str1[i1] == ' ' || str1[i1] == '\t' || str1[i1] == '_' )
            ++i1;
        else if( str2[i2] == ' ' || str2[i2] == '\t' || str2[i2] == '_' )
            ++i2;
        else if( get_lower_case(str1[i1]) == get_lower_case(str2[i2]) )
        {
            ++i1;
            ++i2;
        }
        else
        {
            equivalent = FALSE;
            break;
        }
    }

    return( equivalent );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : rgb_to_hsl
@INPUT      : r
              g
              b
@OUTPUT     : h
              s
              l
@RETURNS    : 
@DESCRIPTION: Converts an RGB colour to a HSL (Hue-Saturation-Light) colour.
              This code was taken from Graphics Gems.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  rgb_to_hsl(
    VIO_Real    r,
    VIO_Real    g,
    VIO_Real    b,
    VIO_Real    *h,
    VIO_Real    *s,
    VIO_Real    *l )
{
    VIO_Real  v, m, vm, r2, g2, b2;

    check_initialize_colours();

    v = MAX3( r, g, b );

    m = MIN3( r, g, b );

    *l = (m + v) / 2.0;

    if( *l > 0.0 )
    {
        vm = v - m;
        *s = vm;

        if( *s > 0.0 )
        {
            if( *l <= 0.5 )
                *s /= v + m;
            else
                *s /= 2.0 - v - m;

            r2 = (v - r) / vm;
            g2 = (v - g) / vm;
            b2 = (v - b) / vm;

            if( r == v )
            {
                if( g == m )
                    *h = 5.0 + b2;
                else
                    *h = 1.0 - g2;
            }
            else if( g == v )
            {
                if( b == m )
                    *h = 1.0 + r2;
                else
                    *h = 3.0 - b2;
            }
            else
            {
                if( r == m )
                    *h = 3.0 + g2;
                else
                    *h = 5.0 - r2;
            }

            *h /= 6.0;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : hsl_to_rgb
@INPUT      : h
              s
              l
@OUTPUT     : r
              g
              b
@RETURNS    : 
@DESCRIPTION: Converts a HSL (Hue-Saturation-Light) colour to RGB colour.
              This code was taken from Graphics Gems.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  hsl_to_rgb(
    VIO_Real   h,
    VIO_Real   sl,
    VIO_Real   l,
    VIO_Real   *r,
    VIO_Real   *g,
    VIO_Real   *b )
{
    VIO_Real  v;
    VIO_Real  m, sv;
    int   sextant;
    VIO_Real  fract, vsf, mid1, mid2;

    if( l <= 0.5 )
    {
        v = l * (1.0 + sl);
    }
    else
    {
        v = l + sl - l * sl;
    }

    if( v <= 0.0 )
    {
        *r = 0.0;
        *g = 0.0;
        *b = 0.0;
    }
    else
    {
        m = l + l - v;
        sv = (v - m) / v;
        h *= 6.0;
        sextant = (int) h;
        fract = h - (VIO_Real) sextant;
        vsf = v * sv * fract;
        mid1 = m + vsf;
        mid2 = v - vsf;

        switch( sextant )
        {
        case 0:
        case 6:  *r = v;     *g = mid1;  *b = m;     break;
        case 1:  *r = mid2;  *g = v;     *b = m;     break;
        case 2:  *r = m;     *g = v;     *b = mid1;  break;
        case 3:  *r = m;     *g = mid2;  *b = v;     break;
        case 4:  *r = mid1;  *g = m;     *b = v;     break;
        case 5:  *r = v;     *g = m;     *b = mid2;  break;
        }
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_colour_to_hsl
@INPUT      : rgb
@OUTPUT     : hsl
@RETURNS    : 
@DESCRIPTION: Converts an rgb colour to hsl.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  convert_colour_to_hsl(
    VIO_Colour   rgb,
    VIO_Colour   *hsl )
{
    VIO_Real   h, s, l;

    rgb_to_hsl( get_Colour_r_0_1(rgb),
                get_Colour_g_0_1(rgb),
                get_Colour_b_0_1(rgb),
                &h, &s, &l );

    *hsl = make_Colour_0_1( h, s, l );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_colour_to_rgb
@INPUT      : hsl
@OUTPUT     : rgb
@RETURNS    : 
@DESCRIPTION: Converts a hsl colour to rgb.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  void  convert_colour_to_rgb(
    VIO_Colour   hsl,
    VIO_Colour   *rgb )
{
    VIO_Real   r, g, b;

    hsl_to_rgb( get_Colour_r_0_1(hsl),
                get_Colour_g_0_1(hsl),
                get_Colour_b_0_1(hsl),
                &r, &g, &b );

    *rgb = make_Colour_0_1( r, g, b );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : equal_colours
@INPUT      : col1
              col2
@OUTPUT     : 
@RETURNS    : TRUE if equal
@DESCRIPTION: Determines if two colours are equal.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_BOOL  equal_colours(
    VIO_Colour  col1,
    VIO_Colour  col2 )
{
    return( col1 == col2 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_colour_to_string
@INPUT      : col
@OUTPUT     : string
@RETURNS    : 
@DESCRIPTION: Converts a colour to a string.  If a named colour exists for
              the colour, then the name is assigned to the string.  Otherwise
              the 3 numerical components of the colour are assigned to the
              string, e.g. "0.3 0.7 1.0".
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_STR  convert_colour_to_string(
    VIO_Colour   col )
{
    char       buffer[VIO_EXTREMELY_LARGE_STRING_SIZE];
    VIO_STR     string;

    if( !lookup_colour_name( col, &string ) )
    {
        (void) sprintf( buffer, "%g %g %g",
                        get_Colour_r_0_1(col),
                        get_Colour_g_0_1(col),
                        get_Colour_b_0_1(col) );
        string = create_string( buffer );
    }

    return( string );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : string_to_colour
@INPUT      : string
@OUTPUT     : colour_ptr
@RETURNS    : status
@DESCRIPTION: Converts a string to a colour.  If the string is a named colour
              such as "RED", then the corresponding colour is returned.
              Otherwise, the string is assumed to be of the form "0.3 1.0 0.7"
              and the corresponding colour returned.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 2016            Robert D Vincent
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Status  string_to_colour( VIO_STR     string,
                                      VIO_Colour  *colour_ptr )
{
    VIO_Colour   colour;
    double   r, g, b, a;
    VIO_Status   status = VIO_OK;

    if( !lookup_colour( string, &colour ) )
    {
        if( sscanf( string, "%lf %lf %lf %lf", &r, &g, &b, &a ) == 4 )
            colour = make_rgba_Colour_0_1( r, g, b, a );
        else if( sscanf( string, "%lf,%lf,%lf,%lf", &r, &g, &b, &a ) == 4 )
            colour = make_rgba_Colour_0_1( r, g, b, a );
        else if( sscanf( string, "%lf %lf %lf", &r, &g, &b ) == 3 )
            colour = make_Colour_0_1( r, g, b );
        else if( sscanf( string, "%lf,%lf,%lf", &r, &g, &b ) == 3 )
            colour = make_Colour_0_1( r, g, b );
        else
        {
            colour = make_Colour( 0, 0, 0 );
            status = VIO_ERROR;
        }
    }
    *colour_ptr = colour;
    return( status );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : convert_string_to_colour
@INPUT      : string
@OUTPUT     :
@RETURNS    : colour
@DESCRIPTION: Converts a string to a colour.  If the string is a named colour
              such as "RED", then the corresponding colour is returned.
              Otherwise, the string is assumed to be of the form "0.3 1.0 0.7"
              and the corresponding colour returned.
@METHOD     :
@GLOBALS    :
@CALLS      :
@CREATED    : 1993            David MacDonald
@MODIFIED   :
---------------------------------------------------------------------------- */

BICAPI  VIO_Colour  convert_string_to_colour( VIO_STR     string )
{
    VIO_Colour colour;
    string_to_colour( string, &colour );
    return( colour );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_colour_distance
@INPUT      : r   : in range 0 to 255
              g
              b
@OUTPUT     : 
@RETURNS    : Squared Distance
@DESCRIPTION: Returns the squared distance between two colours, one
              specified by its rgb components, and the other as a VIO_Colour
              entity.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  int  get_colour_distance(
    int      r,
    int      g,
    int      b,
    VIO_Colour   c2 )
{
    int  dr, dg, db;

    dr = r - get_Colour_r( c2 );
    dg = g - get_Colour_g( c2 );
    db = b - get_Colour_b( c2 );

    return( dr * dr + dg * dg + db * db );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_closest_colour
@INPUT      : r
              g
              b
              n_colours
              colours[]
@OUTPUT     : 
@RETURNS    : index into colours[]
@DESCRIPTION: Finds the closest colour in a list of colours, returning the
              index.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  int   find_closest_colour(
    int     r,
    int     g,
    int     b,
    int     n_colours,
    VIO_Colour  colours[] )
{
    int   i, dist, min_dist, min_index;

    min_index = -1;
    min_dist = 0;

    for_less( i, 0, n_colours )
    {
        dist = get_colour_distance( r, g, b, colours[i] );
        if( i == 0 || dist < min_dist )
        {
            min_index = i;
            min_dist = dist;
        }
    }

    return( min_index );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_Colour_luminance
@INPUT      : colour
@OUTPUT     : 
@RETURNS    : value between 0 and 255
@DESCRIPTION: Converts a colour to gray-scale.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  int  get_Colour_luminance(
    VIO_Colour   colour )
{
    return( VIO_ROUND( 0.299 * (VIO_Real) get_Colour_r(colour) +
                   0.587 * (VIO_Real) get_Colour_g(colour) +
                   0.114 * (VIO_Real) get_Colour_b(colour) ) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scale_colour
@INPUT      : colour
              factor
@OUTPUT     : 
@RETURNS    : VIO_Colour
@DESCRIPTION: Scales a colour by a factor, but not the alpha component.  Does
              not check for overflow.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI  VIO_Colour  scale_colour(
    VIO_Colour   colour,
    VIO_Real     factor )
{
    return( make_rgba_Colour( VIO_ROUND( (VIO_Real) get_Colour_r(colour) * factor ),
                              VIO_ROUND( (VIO_Real) get_Colour_g(colour) * factor ),
                              VIO_ROUND( (VIO_Real) get_Colour_b(colour) * factor ),
                              get_Colour_a(colour) ) );
}
