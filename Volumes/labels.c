#include  <mni.h>

#define  ACTIVE_BIT                128
#define  LOWER_AUXILIARY_BITS      127

public  int  get_max_label()
{
    return( LOWER_AUXILIARY_BITS );
}

public  int  get_active_bit()
{
    return( ACTIVE_BIT );
}

public  Volume  create_label_volume(
    Volume  volume )
{
    Volume   label_volume;

    label_volume = copy_volume_definition_no_alloc( volume, NC_BYTE, FALSE,
                                                    0.0, 255.0 );

    set_volume_real_range( label_volume, 0.0, 255.0 );

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
    BOOLEAN        value )
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
    BOOLEAN        value )
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
    void            *void_ptr;
    unsigned char   *label_ptr;

    check_alloc_label_data( volume );

    GET_VOXEL_PTR( void_ptr, volume,
                   voxel[0], voxel[1], voxel[2], voxel[3], voxel[4] );
    label_ptr = (unsigned char *) void_ptr;

    if( !value )
        *label_ptr |= bit;
    else if( (*label_ptr & bit) != 0 )
        *label_ptr ^= bit;
}

public  BOOLEAN  get_voxel_activity_flag(
    Volume          volume,
    int             voxel[] )
{
    return( get_voxel_label_bit( volume, voxel, ACTIVE_BIT ) );
}

public  void  set_voxel_activity_flag(
    Volume          volume,
    int             voxel[],
    BOOLEAN         value )
{
    set_voxel_label_bit( volume, voxel, ACTIVE_BIT, value );
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
        if( get_voxel_activity_flag( volume, ind ) )
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
    Status   status;

    check_alloc_label_data( label_volume );

    status = input_volume( filename, 3, XYZ_dimension_names,
                           NC_BYTE, FALSE, 0.0, 0.0,
                           FALSE, &label_volume, NULL );

    return( status );
}

public  Status  save_label_volume(
    char     filename[],
    Volume   label_volume )
{
    Status   status;

    check_alloc_label_data( label_volume );

    status = output_volume( filename, NC_UNSPECIFIED, FALSE, 0.0, 0.0,
                            label_volume, "Label volume", NULL );

    return( status );
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
    int                x, y, z, nx, ny, nz, sizes[MAX_DIMENSIONS];
    void               *void_ptr;
    unsigned char      *label_ptr;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        print( "io_volume_label_bit:  volume must be 3D\n" );
        return( ERROR );
    }

    status = OK;

    get_volume_sizes( volume, sizes );

    nx = sizes[X];
    ny = sizes[Y];
    nz = sizes[Z];

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
    int             i, c, label, ind[MAX_DIMENSIONS];
    Real            voxel[MAX_DIMENSIONS];
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
                                voxel );

        for_less( c, 0, get_volume_n_dimensions(volume) )
        {
            ind[c] = ROUND( voxel[c] );
        }

        label = structure_ids[i];
        if( label >= 0 && label <= LOWER_AUXILIARY_BITS &&
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
                label = get_volume_label_data( label_volume, ind ) &
                        LOWER_AUXILIARY_BITS;

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

    check_alloc_label_data( label_volume );

    while( io_tag_point( file, READ_FILE, volume, 1.0, &marker ) == OK )
    {
        convert_world_to_voxel( volume,
                                Point_x(marker.position),
                                Point_y(marker.position),
                                Point_z(marker.position), voxel );

        for_less( c, 0, get_volume_n_dimensions(volume) )
            ind[c] = ROUND( voxel[c] );

        label = marker.structure_id;
        if( label >= 0 && label <= LOWER_AUXILIARY_BITS &&
            int_voxel_is_within_volume( volume, ind ) )
        {
            set_volume_label_data( label_volume, ind, label );
        }
    }

    return( OK );
}
