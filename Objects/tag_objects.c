#include  <def_mni.h>

public  Status   input_objects_any_format(
    Volume         volume,
    char           filename[],
    Colour         marker_colour,
    Real           marker_size,
    Marker_types   marker_type,
    int            *n_objects,
    object_struct  **object_list[] )
{
    Status            status;
    File_formats      format;

    if( filename_extension_matches(filename,get_default_landmark_file_suffix()))
    {
        status = input_landmark_file( volume, filename,
                                      marker_colour, marker_size, marker_type,
                                      n_objects, object_list );
    }
    else if( filename_extension_matches(filename,get_default_tag_file_suffix()))
    {
        status = input_tag_objects_file( filename,
                                 marker_colour, marker_size, marker_type,
                                 n_objects, object_list );
    }
    else
    {
        status = input_graphics_file( filename, &format,
                                      n_objects, object_list );
    }

    return( status );
}

public  Status   input_tag_objects_file(
    char           filename[],
    Colour         marker_colour,
    Real           default_size,
    Marker_types   default_type,
    int            *n_objects,
    object_struct  **object_list[] )
{
    Status             status;
    object_struct      *object;
    marker_struct      *marker;
    int                i, n_volumes, n_tag_points, *structure_ids, *patient_ids;
    char               **labels;
    double             *weights;
    double             **tags1, **tags2;

    *n_objects = 0;

    status = input_tag_file( filename, &n_volumes, &n_tag_points,
                             &tags1, &tags2, &weights,
                             &structure_ids, &patient_ids, &labels );

    if( status == OK )
    {
        for_less( i, 0, n_tag_points )
        {
            object = create_object( MARKER );
            marker = get_marker_ptr( object );
            fill_Point( marker->position, tags1[i][X], tags1[i][Y],tags1[i][Z]);
            (void) strcpy( marker->label, labels[i] );

            if( structure_ids[i] >= 0 )
                marker->structure_id = structure_ids[i];
            else
                marker->structure_id = -1;

            if( patient_ids[i] >= 0 )
                marker->patient_id = patient_ids[i];
            else
                marker->patient_id = -1;

            if( weights[i] > 0.0 )
                marker->size = weights[i];
            else
                marker->size = default_size;

            marker->colour = marker_colour;
            marker->type = default_type;

            add_object_to_list( n_objects, object_list, object );
        }

        free_tag_points( n_volumes, n_tag_points, tags1, tags2, weights,
                         structure_ids, patient_ids, labels );
    }

    return( status );
}
