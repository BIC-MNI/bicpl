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

#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/scan_objects.c,v 1.7 2000-02-05 21:27:28 stever Exp $";
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

public  void  scan_object_to_volume(
    object_struct    *object,
    Volume           volume,
    Volume           label_volume,
    int              label,
    Real             max_distance )
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
        }
    }
}
