#include  <def_mni.h>

#define  ACTIVE_BIT                128
#define  LOWER_AUXILIARY_BITS      127

public  Volume  create_label_volume(
    int    n_dimensions,
    int    sizes[] )
{
    Volume   volume;

    volume = create_volume( n_dimensions, (char **) NULL, NC_BYTE, FALSE,
                            0.0, 0.0 );

    set_volume_sizes( volume, sizes );

    return( volume );
}

private  void  check_alloc_label_data(
    Volume  volume )
{
    if( volume->data == (void *) NULL )
        alloc_volume_data( volume );
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
    int             n_voxels, i;
    void            *void_ptr;
    unsigned char   *label_ptr;

    check_alloc_label_data( volume );

    n_voxels = get_volume_total_n_voxels( volume );
    GET_VOXEL_PTR( void_ptr, volume, 0, 0, 0, 0, 0 );
    label_ptr = (unsigned char *) void_ptr;

    for_less( i, 0, n_voxels )
    {
        *label_ptr = (unsigned char) value;
        ++label_ptr;
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
    Boolean        value )
{
    int             n_voxels, i;
    void            *void_ptr;
    unsigned char   *label_ptr;

    check_alloc_label_data( volume );

    n_voxels = get_volume_total_n_voxels( volume );
    GET_VOXEL_PTR( void_ptr, volume, 0, 0, 0, 0, 0 );
    label_ptr = (unsigned char *) void_ptr;

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
    set_all_volume_label_data_bit( volume, ACTIVE_BIT, !value );
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
    int             x,
    int             y,
    int             z,
    int             value )
{
    check_alloc_label_data( volume );

    SET_VOXEL_3D( volume, x, y, z, value );
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
    int             x,
    int             y,
    int             z )
{
    int    label;

    if( volume->data == (void *) NULL )
        return( 0 );
    else
    {
        GET_VOXEL_3D( label, volume, x, y, z );
        return( label );
    }
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_voxel_activity_flag
@INPUT      : volume
              x
              y
              z
@OUTPUT     : 
@RETURNS    : TRUE if voxel is active
@DESCRIPTION: Returns the active bit of the voxel, or if no label data,
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
    return( (get_volume_label_data( volume, x, y, z ) & ACTIVE_BIT) == 0 );
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
    void            *void_ptr;
    unsigned char   *label_ptr;

    check_alloc_label_data( volume );

    GET_VOXEL_PTR_3D( void_ptr, volume, x, y, z );
    label_ptr = (unsigned char *) void_ptr;

    if( !value )
        *label_ptr |= ACTIVE_BIT;
    else if( (*label_ptr & ACTIVE_BIT) != 0 )
        *label_ptr ^= ACTIVE_BIT;
}

public  Boolean  get_volume_voxel_activity(
    Volume     volume,
    Real       x,
    Real       y,
    Real       z,
    Boolean    activity_if_mixed )
{
    int   n_active, x_int, y_int, z_int;
    int   sizes[MAX_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    if( x < 0.0 || x > sizes[X]-1 ||
        y < 0.0 || y > sizes[Y]-1 ||
        z < 0.0 || z > sizes[Z]-1 )
        return( FALSE );

    x_int = (int) x;
    if( x_int == sizes[X] - 1 )
        x_int = sizes[X] - 2;

    y_int = (int) y;
    if( y_int == sizes[Y] - 1 )
        y_int = sizes[Y] - 2;

    z_int = (int) z;
    if( z_int == sizes[Z] - 1 )
        z_int = sizes[Z] - 2;

    n_active = 0;

    if( get_voxel_activity_flag( volume, x, y, z ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x, y, z+1 ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x, y+1, z ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x, y+1, z+1 ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x+1, y, z ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x+1, y, z+1 ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x+1, y+1, z ) )
        ++n_active;
    if( get_voxel_activity_flag( volume, x+1, y+1, z+1 ) )
        ++n_active;

    if( n_active == 0 )
        return( FALSE );
    else if( n_active == 8 )
        return( TRUE );
    else
        return( activity_if_mixed );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : io_volume_label_bit
@INPUT      : file
              io_type  - READ_FILE or WRITE_FILE
              volume
              bit
@OUTPUT     : 
@RETURNS    : OK if successful
@DESCRIPTION: Reads or writes the given bit of the label data for all
              voxels.
@CREATED    : Mar   1993           David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Status  io_volume_label_bit(
    FILE           *file,
    IO_types       io_type,
    Volume         volume,
    int            bit )
{
    Status             status;
    bitlist_3d_struct  bitlist;
    int                x, y, z, nx, ny, nz;
    void               *void_ptr;
    unsigned char      *label_ptr;

    status = OK;

    nx = volume->sizes[X];
    ny = volume->sizes[Y];
    nz = volume->sizes[Z];

    check_alloc_label_data( volume );

    create_bitlist_3d( nx, ny, nz, &bitlist );

    if( io_type == WRITE_FILE )
    {
        GET_VOXEL_PTR( void_ptr, volume, 0, 0, 0, 0, 0 );
        label_ptr = (unsigned char *) void_ptr;

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
        GET_VOXEL_PTR( void_ptr, volume, 0, 0, 0, 0, 0 );
        label_ptr = (unsigned char *) void_ptr;

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

public  Status  io_volume_activity_bit(
    FILE           *file,
    IO_types       io_type,
    Volume         volume )
{
    return( io_volume_label_bit( file, io_type, volume, ACTIVE_BIT ) );
}

public  Status  input_tags_as_labels(
    FILE    *file,
    Volume  volume,
    Volume  label_volume )
{
    int             i, label, ind[N_DIMENSIONS];
    Real            x_voxel, y_voxel, z_voxel;
    int             n_volumes, n_tag_points;
    Real            **tags1, **tags2, *weights;
    int             *structure_ids, *patient_ids;
    char            **labels;

    check_alloc_label_data( label_volume );

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
            int_voxel_is_within_volume( volume, ind ) )
        {
            set_volume_label_data( label_volume, ind[X], ind[Y], ind[Z], label);
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
    int             x, y, z;
    int             n_tags, label, sizes[N_DIMENSIONS];
    Real            x_world, y_world, z_world;
    Real            **tags, *weights;
    int             *structure_ids, *patient_ids;

    check_alloc_label_data( label_volume );
    get_volume_sizes( label_volume, sizes );

    n_tags = 0;

    for_less( x, 0, sizes[X] )
    {
        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                label = get_volume_label_data( label_volume, x, y, z ) &
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
    Volume  volume,
    Volume  label_volume )
{
    int             label, ind[N_DIMENSIONS];
    Real            x_voxel, y_voxel, z_voxel;
    marker_struct   marker;

    check_alloc_label_data( label_volume );

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
            int_voxel_is_within_volume( volume, ind ) )
        {
            set_volume_label_data( label_volume, ind[X], ind[Y], ind[Z], label);
        }
    }

    return( OK );
}
