#include  <def_mni.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : alloc_auxiliary_data
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Allocates memory for the auxiliary data, which is used for
              setting voxels active and inactive, and in general,
              labeling voxels with an integer value between 0 and 255.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  alloc_auxiliary_data(
    Volume  volume )
{
    ALLOC3D( volume->labels, volume->sizes[X], volume->sizes[Y],
             volume->sizes[Z] );
    set_all_volume_auxiliary_data( volume, ACTIVE_BIT );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : free_auxiliary_data
@INPUT      : volume
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Frees the memory associated with the auxiliary volume data.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  free_auxiliary_data(
    Volume   volume )
{
    if( volume->labels != (unsigned char ***) NULL )
        FREE3D( volume->labels );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_volume_auxiliary_data
@INPUT      : volume
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the auxiliary value of all voxels to the value specified.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_volume_auxiliary_data(
    Volume    volume,
    int       value )
{
    int             n_voxels, i;
    unsigned char   *label_ptr;

    n_voxels = volume->sizes[X] * volume->sizes[Y] * volume->sizes[Z];

    label_ptr = volume->labels[0][0];

    for_less( i, 0, n_voxels )
    {
        *label_ptr = (unsigned char) value;
        ++label_ptr;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_volume_auxiliary_data_bit
@INPUT      : volume
              bit
              value - ON or OFF
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets just the given bit of all the voxels' auxiliary data to the
              given value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_volume_auxiliary_data_bit(
    Volume         volume,
    int            bit,
    Boolean        value )
{
    int             n_voxels, i;
    unsigned char   *label_ptr;

    n_voxels = volume->sizes[X] * volume->sizes[Y] * volume->sizes[Z];

    label_ptr = volume->labels[0][0];

    for_less( i, 0, n_voxels )
    {
        if( value )
            *label_ptr |= bit;
        else if( (*label_ptr & bit) != 0 )
            *label_ptr ^= bit;

        ++label_ptr;
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_voxel_activity_flags
@INPUT      : volume
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets all voxels active or inactive.  (TRUE = active).
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_voxel_activity_flags(
    Volume         volume,
    Boolean        value )
{
    set_all_volume_auxiliary_data_bit( volume, ACTIVE_BIT, value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_all_voxel_label_flags
@INPUT      : volume
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the label bit of all auxiliary data to the given value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_all_voxel_label_flags(
    Volume         volume,
    Boolean        value )
{
    set_all_volume_auxiliary_data_bit( volume, LABEL_BIT, value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_volume_auxiliary_data
@INPUT      : volume
              x
              y
              z
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the auxiliary data of the given voxel to the value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_volume_auxiliary_data(
    Volume          volume,
    int             x,
    int             y,
    int             z,
    int             value )
{
    volume->labels[x][y][z] = (unsigned char) value;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_volume_auxiliary_data
@INPUT      : volume
              x
              y
              z
@OUTPUT     : 
@RETURNS    : value
@DESCRIPTION: Gets the auxiliary data of the given voxel to the value.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_volume_auxiliary_data(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    return( (int) (volume->labels[x][y][z]) );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_voxel_activity_flag
@INPUT      : volume
              x
              y
              z
@OUTPUT     : 
@RETURNS    : TRUE if voxel is active
@DESCRIPTION: Returns the active bit of the voxel, or if no auxiliary data,
              then all voxels are active.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  get_voxel_activity_flag(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    if( volume->labels == (unsigned char ***) NULL )
        return( TRUE );
    else
        return( (volume->labels[x][y][z] & ACTIVE_BIT) != 0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_voxel_activity_flag
@INPUT      : volume
              x
              y
              z
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the activity flag for the given voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_voxel_activity_flag(
    Volume          volume,
    int             x,
    int             y,
    int             z,
    Boolean         value )
{
    unsigned  char  *ptr;

    ptr = &volume->labels[x][y][z];
    if( value )
        *ptr |= ACTIVE_BIT;
    else if( (*ptr & ACTIVE_BIT) != 0 )
        *ptr ^= ACTIVE_BIT;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_voxel_label_flag
@INPUT      : 
@OUTPUT     : 
@RETURNS    : TRUE if voxel labeled
@DESCRIPTION: Returns the label bit of the voxel's auxiliary data.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Boolean  get_voxel_label_flag(
    Volume          volume,
    int             x,
    int             y,
    int             z )
{
    if( volume->labels == (unsigned char ***) NULL )
        return( FALSE );
    else
        return( (volume->labels[x][y][z] & LABEL_BIT) != 0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_voxel_label_flag
@INPUT      : volume
              x
              y
              z
              value
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Sets the label flag for the given voxel.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  set_voxel_label_flag(
    Volume          volume,
    int             x,
    int             y,
    int             z,
    Boolean         value )
{
    unsigned  char  *ptr;

    ptr = &volume->labels[x][y][z];
    if( value )
        *ptr |= LABEL_BIT;
    else if( (*ptr & LABEL_BIT) != 0 )
        *ptr ^= LABEL_BIT;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : io_volume_auxiliary_bit
@INPUT      : file
              io_type  - READ_FILE or WRITE_FILE
              volume
              bit
@OUTPUT     : 
@RETURNS    : OK if successful
@DESCRIPTION: Reads or writes the given bit of the auxiliary data for all
              voxels.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  io_volume_auxiliary_bit(
    FILE           *file,
    IO_types       io_type,
    Volume         volume,
    int            bit )
{
    Status             status;
    bitlist_3d_struct  bitlist;
    int                x, y, z, nx, ny, nz;
    unsigned char      *label_ptr;

    status = OK;

    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    create_bitlist_3d( nx, ny, nz, &bitlist );

    if( io_type == WRITE_FILE )
    {
        label_ptr = volume->labels[0][0];

        for_less( x, 0, nx )
        {
            for_less( y, 0, ny )
            {
                for_less( z, 0, nz )
                {
                    if( (*label_ptr & bit) != 0 )
                        set_bitlist_bit_3d( &bitlist, x, y, z, TRUE );

                    ++label_ptr;
                }
            }
        }
    }

    status = io_bitlist_3d( file, io_type, &bitlist );

    if( status == OK && io_type == READ_FILE )
    {
        label_ptr = volume->labels[0][0];

        for_less( x, 0, nx )
        {
            for_less( y, 0, ny )
            {
                for_less( z, 0, nz )
                {
                    if( get_bitlist_bit_3d( &bitlist, x, y, z ) )
                        *label_ptr |= bit;
                    else if( (*label_ptr & bit) != 0 )
                        *label_ptr ^= bit;

                    ++label_ptr;
                }
            }
        }
    }

    if( status == OK )
        delete_bitlist_3d( &bitlist );

    return( status );
}

public  Status  input_tags_as_labels(
    FILE    *file,
    Volume  volume )
{
    int             i, label, ind[N_DIMENSIONS];
    Real            x_voxel, y_voxel, z_voxel;
    int             n_volumes, n_tag_points;
    Real            **tags1, **tags2, *weights;
    int             *structure_ids, *patient_ids;
    char            **labels;

    if( input_tag_points( file, &n_volumes, &n_tag_points,
                               &tags1, &tags2, &weights, &structure_ids,
                               &patient_ids, &labels ) != OK )
        return( ERROR );

    for_less( i, 0, n_tag_points )
    {
        convert_world_to_voxel( volume,
                                tags1[i][X], tags1[i][Y], tags1[i][Z],
                                &x_voxel, &y_voxel, &z_voxel );

        ind[X] = ROUND( x_voxel );
        ind[Y] = ROUND( y_voxel );
        ind[Z] = ROUND( z_voxel );

        label = structure_ids[i];
        if( label >= 0 && label <= LOWER_AUXILIARY_BITS &&
            cube_is_within_volume( volume, ind ) )
        {
            set_volume_auxiliary_data( volume, ind[X], ind[Y], ind[Z],
                                       label | ACTIVE_BIT );
        }
    }

    free_tag_points( n_volumes, n_tag_points, tags1, tags2,
                     weights, structure_ids, patient_ids, labels );

    return( OK );
}

public  Status  output_labels_as_tags(
    FILE    *file,
    Volume  volume,
    int     desired_label,
    Real    size,
    int     patient_id )
{
    Status          status;
    int             x, y, z;
    int             n_tags, label, sizes[N_DIMENSIONS];
    Real            x_world, y_world, z_world;
    Real            **tags, *weights;
    int             *structure_ids, *patient_ids;

    get_volume_sizes( volume, sizes );

    n_tags = 0;

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                label = get_volume_auxiliary_data( volume, x, y, z ) &
                        LOWER_AUXILIARY_BITS;

                if( label == desired_label || (desired_label < 0 && label > 0) )
                {
                    convert_voxel_to_world( volume, (Real) x, (Real) y, (Real)z,
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
    Volume  volume )
{
    int             label, ind[N_DIMENSIONS];
    Real            x_voxel, y_voxel, z_voxel;
    marker_struct   marker;

    while( io_tag_point( file, READ_FILE, volume, 1.0, &marker ) == OK )
    {
        convert_world_to_voxel( volume,
                                Point_x(marker.position),
                                Point_y(marker.position),
                                Point_z(marker.position),
                                &x_voxel, &y_voxel, &z_voxel );

        ind[X] = ROUND( x_voxel );
        ind[Y] = ROUND( y_voxel );
        ind[Z] = ROUND( z_voxel );

        label = marker.structure_id;
        if( label >= 0 && label <= LOWER_AUXILIARY_BITS &&
            cube_is_within_volume( volume, ind ) )
        {
            set_volume_auxiliary_data( volume, ind[X], ind[Y], ind[Z],
                                       label | ACTIVE_BIT );
        }
    }

    return( OK );
}
