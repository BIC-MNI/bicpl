#include  <internal_volume_io.h>
#include  <bicpl.h>

public  void  scan_object_to_volume(
    object_struct    *object,
    Volume           volume,
    Volume           label_volume,
    int              label,
    Real             max_distance )
{
    object_struct           *current_object;
    object_traverse_struct  object_traverse;

    initialize_object_traverse( &object_traverse, 1, &object );

    while( get_next_object_traverse(&object_traverse,&current_object) )
    {
        switch( get_object_type( current_object ) )
        {
        case POLYGONS:
            scan_polygons_to_voxels( get_polygons_ptr(current_object),
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
