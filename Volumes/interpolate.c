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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/interpolate.c,v 1.2 1995-07-31 13:45:54 david Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : interpolate_volume_to_slice
@INPUT      : volume1
              n_dims1
              sizes1
              origin1
              x_axis1
              y_axis1
              volume2
              n_dims2
              sizes2
              origin2
              x_axis2
              y_axis2
              degrees_continuity
              cmode_colour_map
              rgb_colour_map
              empty_colour
@OUTPUT     : pixels
@RETURNS    : 
@DESCRIPTION: Interpolates the volume to the given slice, using either
              nearest neighbour, linear or cubic interpolation.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

/* ARGSUSED */

public  void  interpolate_volume_to_slice(
    Volume          volume1,
    int             n_dims1,
    int             sizes1[],
    Real            origin1[],
    Real            x_axis1[],
    Real            y_axis1[],
    Volume          volume2,
    int             n_dims2,
    int             sizes2[],
    Real            origin2[],
    Real            x_axis2[],
    Real            y_axis2[],
    int             degrees_continuity,
    unsigned short  **cmode_colour_map,
    Colour          **rgb_colour_map,
    Colour          empty_colour,
    pixels_struct   *pixels )
{
    int             dim, x, y, x_size, y_size;
    int             int_voxel_value1, int_voxel_value2;
    Real            outside_value1, outside_value2;
    Real            start_voxel1[MAX_DIMENSIONS], voxel1[MAX_DIMENSIONS];
    Real            start_voxel2[MAX_DIMENSIONS], voxel2[MAX_DIMENSIONS];
    Real            value1, voxel_value1, value2, voxel_value2;
    unsigned short  *cmode_ptr;
    Colour          *rgb_ptr;
    BOOLEAN         inside1, inside2;
    Pixel_types     pixel_type;

    x_size = pixels->x_size;
    y_size = pixels->y_size;
    pixel_type = pixels->pixel_type;

    for_less( dim, 0, n_dims1 )
        start_voxel1[dim] = origin1[dim];
    outside_value1 = 0.0;

    if( volume2 != NULL )
    {
        for_less( dim, 0, n_dims2 )
            start_voxel2[dim] = origin2[dim];
        outside_value2 = 0.0;
    }

    for_less( y, 0, y_size )
    {
        for_less( dim, 0, n_dims1 )
            voxel1[dim] = start_voxel1[dim];

        if( volume2 != NULL )
        {
            for_less( dim, 0, n_dims2 )
                voxel2[dim] = start_voxel2[dim];
        }

        if( pixel_type == RGB_PIXEL )
            rgb_ptr = &PIXEL_RGB_COLOUR(*pixels,0,y);
        else
            cmode_ptr = &PIXEL_COLOUR_INDEX_16(*pixels,0,y);

        for_less( x, 0, x_size )
        {
            inside1 = voxel_is_within_volume( volume1, voxel1 );

            if( inside1 )
            {
                (void) evaluate_volume( volume1, voxel1, NULL,
                                        degrees_continuity,
                                        FALSE, outside_value1,
                                        &value1, NULL, NULL );

                voxel_value1 = CONVERT_VALUE_TO_VOXEL( volume1, value1 );
                int_voxel_value1 = ROUND( voxel_value1 );
            }

            for_less( dim, 0, n_dims1 )
                voxel1[dim] += x_axis1[dim];

            if( volume2 != NULL )
            {
                inside2 = voxel_is_within_volume( volume2, voxel2 );

                if( inside2 )
                {
                    (void) evaluate_volume( volume2, voxel2, NULL,
                                            degrees_continuity,
                                            FALSE, outside_value2,
                                            &value2, NULL, NULL );

                    voxel_value2 = CONVERT_VALUE_TO_VOXEL( volume2, value2 );
                    int_voxel_value2 = ROUND( voxel_value2 );
                }

                for_less( dim, 0, n_dims2 )
                    voxel2[dim] += x_axis2[dim];

                if( pixel_type == RGB_PIXEL )
                {
                    if( inside1 && inside2 )
                    {
                        *rgb_ptr = rgb_colour_map[int_voxel_value1]
                                                 [int_voxel_value2];
                    }
                    else
                        *rgb_ptr = empty_colour;

                    ++rgb_ptr;
                }
                else
                {
                    if( inside1 && inside2 )
                    {
                        *cmode_ptr = cmode_colour_map[int_voxel_value1]
                                                     [int_voxel_value2];
                    }
                    else
                        *cmode_ptr = (unsigned short) empty_colour;

                    ++cmode_ptr;
                }
            }
            else
            {
                if( pixel_type == RGB_PIXEL )
                {
                    if( inside1 )
                        *rgb_ptr = rgb_colour_map[0][int_voxel_value1];
                    else
                        *rgb_ptr = empty_colour;

                    ++rgb_ptr;
                }
                else
                {
                    if( inside1 )
                        *cmode_ptr = cmode_colour_map[0][int_voxel_value1];
                    else
                        *cmode_ptr = (unsigned short) empty_colour;

                    ++cmode_ptr;
                }
            }
        }

        for_less( dim, 0, n_dims1 )
            start_voxel1[dim] += y_axis1[dim];

        if( volume2 != NULL )
        {
            for_less( dim, 0, n_dims2 )
                start_voxel2[dim] += y_axis2[dim];
        }
    }
}
