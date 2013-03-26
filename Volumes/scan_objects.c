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
#include  <bicpl.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_objects.c,v 1.9 2005-08-17 22:26:19 bert Exp $";
#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : scan_object_to_volume
@INPUT      : object
              volume
              label_volume
              label
              max_distance
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Scans an object into a label_volume.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

BICAPI void  scan_object_to_volume(
    object_struct    *object,
    VIO_Volume           volume,
    VIO_Volume           label_volume,
    int              label,
    VIO_Real             max_distance )
{
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    initialize_object_traverse( &object_traverse, FALSE, 1, &object );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        switch( get_object_type( current_object ) )
        {
        case POLYGONS:
            scan_polygons_to_voxels( get_polygons_ptr(current_object),
                                     volume, label_volume,
                                     label, max_distance );
            break;

        case QUADMESH:
            scan_quadmesh_to_voxels( get_quadmesh_ptr(current_object),
                                     volume, label_volume,
                                     label, max_distance );
            break;

        case LINES:
            scan_lines_to_voxels( get_lines_ptr(current_object),
                                  volume, label_volume,
                                  label );
            break;

        case MARKER:
            scan_marker_to_voxels( get_marker_ptr(current_object),
                                   volume, label_volume,
                                   label );
            break;
	default:
	    print_error("scan_object_to_volume: object type %d not handled.\n",
			get_object_type( current_object ) );
        }
    }
}
