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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/col_code_points.c,v 1.9 2005-08-17 22:26:19 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : colour_code_points
@INPUT      : colour_coding
              volume
              continuity
              n_points
              points
@OUTPUT     : colour_flag
              colours
@RETURNS    : 
@DESCRIPTION: VIO_Colour codes the points according to the associated value in the
              volume.  Adjust the colour_flag and reallocs the colours as
              necessary.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

static void  colour_code_points(
    colour_coding_struct  *colour_coding,
    VIO_Volume                volume,
    int                   continuity,
    Colour_flags          *colour_flag,
    VIO_Colour                *colours[],
    int                   n_points,
    VIO_Point                 points[] )
{
    int      i;
    VIO_Real     val;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        if( n_points > 0 )
        {
            REALLOC( *colours, n_points );
        }
        else
        {
            FREE( *colours );
        }
        *colour_flag = PER_VERTEX_COLOURS;
    }

    for_less( i, 0, n_points )
    {
        (void) evaluate_volume_in_world( volume,
                                  (VIO_Real) Point_x(points[i]),
                                  (VIO_Real) Point_y(points[i]),
                                  (VIO_Real) Point_z(points[i]), continuity, FALSE,
                                  get_volume_real_min(volume),
                                  &val, (VIO_Real *) NULL,
                                  (VIO_Real *) NULL, (VIO_Real *) NULL,
                                  (VIO_Real *) NULL, (VIO_Real *) NULL,
                                  (VIO_Real *) NULL, (VIO_Real *) NULL,
                                  (VIO_Real *) NULL, (VIO_Real *) NULL );

        (*colours)[i] = get_colour_code( colour_coding, val );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : colour_code_object
@INPUT      : volume
              continuity
              colour_coding
@OUTPUT     : object
@RETURNS    : 
@DESCRIPTION: VIO_Colour codes an object, modifying its per-vertex colours.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  colour_code_object(
    VIO_Volume                 volume,
    int                    continuity,
    colour_coding_struct   *colour_coding,
    object_struct          *object )
{
    polygons_struct         *polygons;
    quadmesh_struct         *quadmesh;
    lines_struct            *lines;

    switch( object->object_type )
    {
    case POLYGONS:
        polygons = get_polygons_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &polygons->colour_flag, &polygons->colours,
                            polygons->n_points, polygons->points );
        break;

    case QUADMESH:
        quadmesh = get_quadmesh_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &quadmesh->colour_flag, &quadmesh->colours,
                            quadmesh->m * quadmesh->n,
                            quadmesh->points );
        break;

    case LINES:
        lines = get_lines_ptr( object );
        colour_code_points( colour_coding, volume, continuity,
                            &lines->colour_flag, &lines->colours,
                            lines->n_points, lines->points );
        break;
    default:
	print_error( "colour_code_object: object type %d not handled.\n", 
		     object->object_type );
    }
}
