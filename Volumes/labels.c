#include  <internal_volume_io.h>
#include  <vols.h>

public  Volume  create_label_volume(
    Volume  volume,
    nc_type type )
{
    Volume   label_volume;

    if( type == NC_UNSPECIFIED )
        type = NC_BYTE;

    label_volume = copy_volume_definition_no_alloc( volume, type, FALSE,
                                                    0.0, -1.0 );

    label_volume->real_range_set = FALSE;

    return( label_volume );
}

private  void  check_alloc_label_data(
    Volume  volume )
{
    if( volume->data == (void *) NULL )
    {
        alloc_volume_data( volume );
        set_all_volume_label_data( volume, 0 );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_volume_label_data
@INPUT      : volume
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the label value of all voxels to the value specified.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_volume_label_data(
    Volume    volume,
    int       value )
{
    Data_types      type;
    void            *ptr;
    int             v0, v1, v2, v3, v4, n_voxels;

    check_alloc_label_data( volume );

    type = get_volume_data_type( volume );
    if( value == 0 && type != FLOAT && type != DOUBLE )
    {
        GET_VOXEL_PTR( ptr, volume, 0, 0, 0, 0, 0 );
        n_voxels = get_volume_total_n_voxels( volume );
        (void) memset( ptr, 0, n_voxels * get_type_size(type) );
    }
    else
    {
        BEGIN_ALL_VOXELS( volume, v0, v1, v2, v3, v4 )

            SET_VOXEL( volume, v0, v1, v2, v3, v4, value );

        END_ALL_VOXELS
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_label_data
@INPUT      : volume
              x
              y
              z
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the label data of the given voxel to the value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_volume_label_data(
    Volume          volume,
    int             voxel[],
    int             value )
{
    check_alloc_label_data( volume );

    SET_VOXEL( volume, voxel[0], voxel[1], voxel[2], voxel[3], voxel[4],
               value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_label_data
@INPUT      : volume
              x
              y
              z
@OUTPUT     : 
@RETURNS    : value
@DESCRIPTION: Returns the label data of the given voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_volume_label_data(
    Volume          volume,
    int             voxel[] )
{
    int    label;

    if( volume == (Volume) NULL || volume->data == (void *) NULL )
        return( 0 );
    else
    {
        GET_VOXEL( label, volume,
                   voxel[0], voxel[1], voxel[2], voxel[3], voxel[4] );
        return( label );
    }
}

public  int  get_3D_volume_label_data(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    int    label;

    if( volume == (Volume) NULL || volume->data == (void *) NULL )
        return( 0 );
    else
    {
        GET_VOXEL_3D( label, volume, x, y, z );
        return( label );
    }
}

public  BOOLEAN  get_voxel_label_bit(
    Volume          volume,
    int             voxel[],
    int             bit )
{
    return( (get_volume_label_data( volume, voxel ) & bit) == 0 );
}

public  void  set_voxel_label_bit(
    Volume          volume,
    int             voxel[],
    int             bit,
    BOOLEAN         value )
{
    int     i, n_dims, v[MAX_DIMENSIONS], label, anded, new_label;

    check_alloc_label_data( volume );

    n_dims = get_volume_n_dimensions(volume);

    for_less( i, 0, n_dims )
        v[i] = voxel[i];

    GET_VOXEL( label, volume, v[0], v[1], v[2], v[3], v[4] );

    anded = (label & bit);

    if( value )
    {
        if( anded != bit )
        {
            new_label = label | bit;
            SET_VOXEL( volume, v[0], v[1], v[2], v[3], v[4], new_label )
        }
    }
    else if( anded != 0 )
    {
        new_label = label & (~bit);
        SET_VOXEL( volume, v[0], v[1], v[2], v[3], v[4], new_label )
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_volume_label_data_bit
@INPUT      : volume
              bit
              value - ON or OFF
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets just the given bit of all the voxels' label data to the
              given value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_volume_label_data_bit(
    Volume         volume,
    int            bit,
    BOOLEAN        value )
{
    int             v[MAX_DIMENSIONS];

    check_alloc_label_data( volume );

    BEGIN_ALL_VOXELS( volume, v[0], v[1], v[2], v[3], v[4] )

        set_voxel_label_bit( volume, v, bit, value );

    END_ALL_VOXELS
}

public  BOOLEAN  get_volume_voxel_activity(
    Volume     volume,
    Real       voxel[],
    BOOLEAN    activity_if_mixed )
{
    BOOLEAN  active_found, inactive_found;
    int      c, int_index[MAX_DIMENSIONS], ind[MAX_DIMENSIONS];
    int      n[MAX_DIMENSIONS], sizes[MAX_DIMENSIONS];

    if( volume == (Volume) NULL || volume->data == (void *) NULL )
        return( TRUE );

    get_volume_sizes( volume, sizes );

    for_less( c, 0, get_volume_n_dimensions(volume) )
        if( voxel[c] < 0.0 || voxel[c] > sizes[c]-1 )
            return( FALSE );

    for_less( c, 0, get_volume_n_dimensions(volume) )
    {
        int_index[c] = (int) voxel[c];
        if( int_index[c] == sizes[c] - 1 )
            int_index[c] = sizes[c] - 2;
        n[c] = 2;
    }

    for_less( c, get_volume_n_dimensions(volume), MAX_DIMENSIONS )
    {
        n[c] = 1;
        int_index[c] = 0;
    }

    active_found = FALSE;
    inactive_found = FALSE;

    for_less( ind[X], int_index[X], int_index[X] + n[X] )
    for_less( ind[Y], int_index[Y], int_index[Y] + n[Y] )
    for_less( ind[Z], int_index[Z], int_index[Z] + n[Z] )
    for_less( ind[3], int_index[3], int_index[3] + n[3] )
    for_less( ind[4], int_index[4], int_index[4] + n[4] )
    {
        if( get_volume_label_data( volume, ind ) == 0 )
        {
            if( inactive_found )
                return( activity_if_mixed );
            active_found = TRUE;
        }
        else
        {
            if( active_found )
                return( activity_if_mixed );
            inactive_found = TRUE;
        }
    }

    if( active_found && !inactive_found )
        return( TRUE );
    else if( !active_found && inactive_found )
        return( FALSE );
    else
        return( activity_if_mixed );
}

public  Status  load_label_volume(
    char     filename[],
    Volume   label_volume )
{
    Status                status;
    int                   sizes[MAX_DIMENSIONS], new_sizes[MAX_DIMENSIONS];
    nc_type               type;
    BOOLEAN               signed_flag;
    Volume                new_volume;
    volume_input_struct   volume_input;

    check_alloc_label_data( label_volume );

    type = get_volume_nc_data_type( label_volume, &signed_flag );

    status = start_volume_input( filename, 3, XYZ_dimension_names,
                           type, signed_flag, 0.0, 0.0,
                           TRUE, &new_volume, NULL, &volume_input );

    if( status == OK )
    {
        get_volume_sizes( new_volume, new_sizes );
        get_volume_sizes( label_volume, sizes );

        if( new_sizes[0] != sizes[0] ||
            new_sizes[1] != sizes[1] ||
            new_sizes[2] != sizes[2] )
        {
            print( "Volume file %s is not the right size for label volume.\n",
                   filename );
            status = ERROR;
        }

        cancel_volume_input( new_volume, &volume_input );
    }

    if( status == OK )
    {
        status = input_volume( filename, 3, XYZ_dimension_names,
                               type, signed_flag, 0.0, 0.0,
                               FALSE, &label_volume, NULL );
    }

    return( status );
}

public  Status  save_label_volume(
    char     filename[],
    char     original_filename[],
    Volume   label_volume )
{
    Status   status;

    check_alloc_label_data( label_volume );

    if( original_filename != NULL && strlen(original_filename) > 0 &&
        file_exists( original_filename ) )
    {
        status = output_modified_volume( filename,
                                NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                label_volume, original_filename,
                                "Label volume", NULL );
    }
    else
    {
        status = output_volume( filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                                label_volume, "Label volume", NULL );
    }

    return( status );
}

public  Status  input_tags_as_labels(
    FILE    *file,
    Volume  volume,
    Volume  label_volume )
{
    int             i, c, label, ind[MAX_DIMENSIONS];
    Real            voxel[MAX_DIMENSIONS];
    int             n_volumes, n_tag_points;
    Real            **tags1, **tags2, *weights;
    int             *structure_ids, *patient_ids;
    Real            min_label, max_label;
    char            **labels;

    check_alloc_label_data( label_volume );

    get_volume_voxel_range( label_volume, &min_label, &max_label );

    if( input_tag_points( file, &n_volumes, &n_tag_points,
                          &tags1, &tags2, &weights, &structure_ids,
                          &patient_ids, &labels ) != OK )
        return( ERROR );

    for_less( i, 0, n_tag_points )
    {
        convert_world_to_voxel( volume,
                                tags1[i][X], tags1[i][Y], tags1[i][Z],
                                voxel );

        for_less( c, 0, get_volume_n_dimensions(volume) )
        {
            ind[c] = ROUND( voxel[c] );
        }

        label = structure_ids[i];
        if( label >= min_label && label <= max_label &&
            int_voxel_is_within_volume( volume, ind ) )
        {
            set_volume_label_data( label_volume, ind, label);
        }
    }

    free_tag_points( n_volumes, n_tag_points, tags1, tags2,
                     weights, structure_ids, patient_ids, labels );

    return( OK );
}

public  Status  output_labels_as_tags(
    FILE    *file,
    Volume  volume,
    Volume  label_volume,
    int     desired_label,
    Real    size,
    int     patient_id )
{
    Status          status;
    int             ind[N_DIMENSIONS];
    int             n_tags, label, sizes[MAX_DIMENSIONS];
    Real            x_world, y_world, z_world, real_ind[N_DIMENSIONS];
    Real            **tags, *weights;
    int             *structure_ids, *patient_ids;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        print( "output_labels_as_tags:  volume must be 3D\n" );
        return( ERROR );
    }

    check_alloc_label_data( label_volume );
    get_volume_sizes( label_volume, sizes );

    n_tags = 0;

    for_less( ind[X], 0, sizes[X] )
    {
        real_ind[X] = (Real) ind[X];
        for_less( ind[Y], 0, sizes[Y] )
        {
            real_ind[Y] = (Real) ind[Y];
            for_less( ind[Z], 0, sizes[Z] )
            {
                real_ind[Z] = (Real) ind[Z];
                label = get_volume_label_data( label_volume, ind );

                if( label == desired_label || (desired_label < 0 && label > 0) )
                {
                    convert_voxel_to_world( volume, real_ind,
                                            &x_world, &y_world, &z_world );

                    SET_ARRAY_SIZE( tags, n_tags, n_tags+1, DEFAULT_CHUNK_SIZE);
                    ALLOC( tags[n_tags], N_DIMENSIONS );
                    SET_ARRAY_SIZE( weights, n_tags, n_tags+1,
                                    DEFAULT_CHUNK_SIZE);
                    SET_ARRAY_SIZE( structure_ids, n_tags, n_tags+1,
                                    DEFAULT_CHUNK_SIZE);
                    SET_ARRAY_SIZE( patient_ids, n_tags, n_tags+1,
                                    DEFAULT_CHUNK_SIZE);

                    tags[n_tags][X] = x_world;
                    tags[n_tags][Y] = y_world;
                    tags[n_tags][Z] = z_world;
                    weights[n_tags] = size;
                    structure_ids[n_tags] = label;
                    patient_ids[n_tags] = patient_id;
                    ++n_tags;
                }
            }
        }
    }

    status = OK;

    if( n_tags > 0 )
    {
        status = output_tag_points( file, (char *) NULL,
                           1, n_tags, tags, (Real **) NULL, weights,
                           structure_ids, patient_ids, (char **) NULL );

        free_tag_points( 1, n_tags, tags, (Real **) NULL,
                         weights, structure_ids, patient_ids, (char **) NULL );
    }

    return( status );
}

public  Status  input_landmarks_as_labels(
    FILE    *file,
    Volume  volume,
    Volume  label_volume )
{
    int             c, label, ind[MAX_DIMENSIONS];
    Real            voxel[MAX_DIMENSIONS];
    marker_struct   marker;
    Real            min_label, max_label;

    check_alloc_label_data( label_volume );

    get_volume_voxel_range( label_volume, &min_label, &max_label );

    while( io_tag_point( file, READ_FILE, volume, 1.0, &marker ) == OK )
    {
        convert_world_to_voxel( volume,
                                Point_x(marker.position),
                                Point_y(marker.position),
                                Point_z(marker.position), voxel );

        for_less( c, 0, get_volume_n_dimensions(volume) )
            ind[c] = ROUND( voxel[c] );

        label = marker.structure_id;
        if( label >= min_label && label <= max_label &&
            int_voxel_is_within_volume( volume, ind ) )
        {
            set_volume_label_data( label_volume, ind, label );
        }
    }

    return( OK );
}
