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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Volumes/box_filter.c,v 1.16 1996-05-17 19:35:46 david Exp $";
#endif

#define  DEBUG
#undef  DEBUG

#ifdef DEBUG
private  Real  get_correct_amount(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    Real     x_width,
    Real     y_width,
    Real     z_width );
#endif

#define  INITIALIZE_SAMPLE( half_width, incremental_flag, on_boundary_flag, receding, advancing, left_weight, right_weight ) \
    { \
        Real  min_pos, max_pos; \
 \
        min_pos = -(half_width); \
        max_pos = (half_width); \
 \
        (incremental_flag) = ((half_width) > 0.5); \
        (on_boundary_flag) = IS_INT( (half_width) + 0.5 ); \
 \
        if( on_boundary_flag ) \
        { \
            (receding) = (int) (min_pos + 0.5); \
            (advancing) = (int) (max_pos + 0.5); \
        } \
        else \
        { \
            (receding) = ROUND( min_pos ); \
            (advancing) = ROUND( max_pos ); \
 \
            if( (advancing) == (receding) ) \
                (left_weight) = 2.0 * (half_width); \
            else \
            { \
                (left_weight) = (Real) (receding) + 0.5 - min_pos; \
                (right_weight) = 1.0 - (left_weight); \
            } \
        } \
    }

#define  GET_FIRST_SAMPLE( on_boundary_flag, advancing, left_weight, data, sample ) \
    { \
        int   _I; \
 \
        (sample) = 0.0; \
        if( on_boundary_flag ) \
        { \
            for_less( _I, 0, (advancing) ) \
                (sample) += (data); \
        } \
        else \
        { \
            for_inclusive( _I, 0, (advancing)-1 ) \
                (sample) += (data); \
 \
            (sample) += (data) * (left_weight); \
        } \
    }

#define  GET_NEXT_SAMPLE( incremental_flag, on_boundary_flag, receding, advancing, left_weight, right_weight, size, data, sample ) \
    { \
        int   _I, _start, _end; \
 \
        if( incremental_flag ) \
        { \
            if( on_boundary_flag ) \
            { \
                if( (advancing) < (size) ) \
                { \
                    _I = (advancing); \
                    (sample) += (data); \
                } \
                if( (receding) >= 0 ) \
                { \
                    _I = (receding); \
                    (sample) -= (data); \
                } \
            } \
            else \
            { \
                if( (advancing) < (size) ) \
                { \
                    _I = (advancing); \
                    (sample) += (data) * (right_weight); \
                } \
                if( (advancing) < (size)-1 ) \
                { \
                    _I = (advancing) + 1; \
                    (sample) += (data) * (left_weight); \
                } \
 \
                if( (receding) >= 0 ) \
                { \
                    _I = (receding); \
                    (sample) -= (data) * (left_weight); \
                } \
                if( (receding) >= -1 ) \
                { \
                    _I = (receding) + 1; \
                    (sample) -= (data) * (right_weight); \
                } \
            } \
        } \
        else \
        { \
            if( on_boundary_flag ) \
            { \
                (sample) = 0.0; \
                if( (receding) < 0 ) \
                    _start = 0; \
                else \
                    _start = (receding); \
                _end = (advancing)-1; \
                if( (advancing) >= (size) ) \
                    _end = (size)-1; \
                for_inclusive( _I, _start, _end ) \
                    (sample) += (data); \
            } \
            else \
            { \
                _I = (receding)+1; \
                if( _I >= 0 ) \
                    (sample) = (data) * (left_weight); \
                else \
                    (sample) = 0.0; \
\
                _start = (receding) + 2; \
                if( _start < 0 ) \
                    _start = 0; \
                _end = (advancing); \
                if( _end >= (size) ) \
                    _end = (size)-1; \
\
                for_inclusive( _I, _start, _end ) \
                    (sample) += (data); \
\
                if( (advancing) > (receding) && (advancing) < (size)-1 ) \
                { \
                    _I = (advancing)+1; \
                    (sample) += (data) * (left_weight); \
                } \
            } \
        } \
    }

/* ----------------------------- MNI Header -----------------------------------
@NAME       : create_box_filtered_volume
@INPUT      : volume           - volume to filter
              nc_data_type     - if NC_UNSPECIFIED, this and next 3 args ignored
              sign_flag
              real_min_value
              real_max_value
              x_width          - full width of box filter
              y_width
              z_width
@OUTPUT     : 
@RETURNS    : filtered volume
@DESCRIPTION: Filters a volume with a box filter, creating a new volume with
              the same number of samples per dimension.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Volume  create_box_filtered_volume(
    Volume   volume,
    nc_type  nc_data_type,
    BOOLEAN  sign_flag,
    Real     real_min_value,
    Real     real_max_value,
    Real     x_width,
    Real     y_width,
    Real     z_width )
{
    Volume             resampled_volume;
    int                i, x, y, z, n_x_cached, x_cache_end;
    int                sizes[MAX_DIMENSIONS];
#ifdef DEBUG
    Real               correct_voxel;
#endif
    Real               total_volume, value, sum, sample, *buffer;
    int                start, end;
    int                x_init_recede_index, y_init_recede_index;
    int                z_init_recede_index;
    int                x_init_advance_index, y_init_advance_index;
    int                z_init_advance_index;
    int                x_recede_index, y_recede_index, z_recede_index;
    int                x_advance_index, y_advance_index, z_advance_index;
    BOOLEAN            x_inc_flag, y_inc_flag, z_inc_flag;
    BOOLEAN            x_bound_flag, y_bound_flag, z_bound_flag;
    Real               x_left_weight, x_right_weight;
    Real               y_left_weight, y_right_weight;
    Real               z_left_weight, z_right_weight;
    float              ***volume_cache, **slice, *row;
    progress_struct    progress;

    if( get_volume_n_dimensions(volume) != 3 )
    {
        handle_internal_error(
           "create_box_filtered_volume: volume must be 3D.\n" );
    }

    x_width = FABS( x_width );
    y_width = FABS( y_width );
    z_width = FABS( z_width );

    get_volume_sizes( volume, sizes );

    resampled_volume = copy_volume_definition( volume, nc_data_type, sign_flag,
                                               real_min_value, real_max_value );

    total_volume = x_width * y_width * z_width;

    x_width /= 2.0;
    y_width /= 2.0;
    z_width /= 2.0;

    INITIALIZE_SAMPLE( x_width, x_inc_flag, x_bound_flag,
                       x_init_recede_index, x_init_advance_index,
                       x_left_weight, x_right_weight )
    INITIALIZE_SAMPLE( y_width, y_inc_flag, y_bound_flag,
                       y_init_recede_index, y_init_advance_index,
                       y_left_weight, y_right_weight )
    INITIALIZE_SAMPLE( z_width, z_inc_flag, z_bound_flag,
                       z_init_recede_index, z_init_advance_index,
                       z_left_weight, z_right_weight )

    ALLOC( volume_cache, sizes[X] );
    n_x_cached = x_init_advance_index - x_init_recede_index + 2;
    for_less( x, 0, n_x_cached )
        ALLOC2D( volume_cache[x], sizes[Y], sizes[Z] );

    ALLOC( buffer, sizes[Z] );

    for_less( x, 0, n_x_cached )
    {
        for_less( y, 0, sizes[Y] )
        {
            get_volume_value_hyperslab_3d( volume, x, y, 0, 1, 1, sizes[Z],
                                           buffer );

            for_less( z, 0, sizes[Z] )
                volume_cache[x][y][z] = (float) buffer[z];
        }
    }

    x_cache_end = n_x_cached-1;

    initialize_progress_report( &progress, FALSE, sizes[X] * sizes[Y],
                                "Box Filtering" );

    ALLOC2D( slice, sizes[Y], sizes[Z] );

    ALLOC( row, sizes[Z] );

    for_less( y, 0, sizes[Y] )
    {
        for_less( z, 0, sizes[Z] )
        {
            GET_FIRST_SAMPLE( x_bound_flag, x_init_advance_index,
                              x_left_weight,
                              (Real) volume_cache[_I][y][z], sample );
            slice[y][z] = (float) sample;
        }
    }

    x_recede_index = x_init_recede_index;
    x_advance_index = x_init_advance_index;

    for_less( x, 0, sizes[X] )
    {
        if( x_advance_index+1 > x_cache_end )
        {
            start = x_cache_end + 1;
            if( start < n_x_cached )
                start = n_x_cached;

            end = x_advance_index+1;
            if( end >= sizes[X] )
                end = sizes[X] - 1;

            for_inclusive( i, start, end )
                volume_cache[i] = volume_cache[i-n_x_cached];

            for_inclusive( i, x_cache_end+1, end )
            {
                for_less( y, 0, sizes[Y] )
                {
                    get_volume_value_hyperslab_3d( volume, i, y, 0,
                                                   1, 1, sizes[Z], buffer );
                    for_less( z, 0, sizes[Z] )
                        volume_cache[i][y][z] = (float) buffer[z];
                }
            }

            x_cache_end = x_advance_index+1;
        }

        for_less( z, 0, sizes[Z] )
        {
            GET_FIRST_SAMPLE( y_bound_flag, y_init_advance_index,
                              y_left_weight, (Real) slice[_I][z], sample )
            row[z] = (float) sample;
        }
        y_recede_index = y_init_recede_index;
        y_advance_index = y_init_advance_index;

        for_less( y, 0, sizes[Y] )
        {
            GET_FIRST_SAMPLE( z_bound_flag, z_init_advance_index,
                              z_left_weight, (Real) row[_I], sum )
            z_recede_index = z_init_recede_index;
            z_advance_index = z_init_advance_index;

            for_less( z, 0, sizes[Z] )
            {
#ifdef DEBUG
                correct_voxel = get_correct_amount( volume, x, y, z,
                                                    x_width, y_width, z_width );

                if( !numerically_close( sum, correct_voxel, 0.001 ) )
                    handle_internal_error( "Dang" );
#endif

                value = sum / total_volume;
                set_volume_real_value( resampled_volume, x, y, z, 0, 0, value );

                if( z == sizes[Z]-1 )
                    continue;

                GET_NEXT_SAMPLE( z_inc_flag, z_bound_flag, z_recede_index,
                                 z_advance_index, z_left_weight, z_right_weight,
                                 sizes[Z], (Real) row[_I], sum )

                ++z_recede_index;
                ++z_advance_index;
            }

            if( y == sizes[Y]-1 )
                continue;

            for_less( z, 0, sizes[Z] )
            {
                sample = (Real) row[z];
                GET_NEXT_SAMPLE( y_inc_flag, y_bound_flag, y_recede_index,
                                 y_advance_index, y_left_weight, y_right_weight,
                                 sizes[Y], (Real) slice[_I][z], sample )
                row[z] = (float) sample;
            }

            ++y_recede_index;
            ++y_advance_index;

            update_progress_report( &progress, x * sizes[Y] + y + 1 );
        }

        if( x == sizes[X] - 1 )
            continue;

        for_less( y, 0, sizes[Y] )
        {
            for_less( z, 0, sizes[Z] )
            {
                sample = (Real) slice[y][z];
                GET_NEXT_SAMPLE( x_inc_flag, x_bound_flag, x_recede_index,
                                 x_advance_index, x_left_weight, x_right_weight,
                                 sizes[X], (Real) volume_cache[_I][y][z],
                                 sample )
                slice[y][z] = (float) sample;
            }
        }

        ++x_recede_index;
        ++x_advance_index;
    }

    terminate_progress_report( &progress );

    for_less( x, 0, n_x_cached )
        FREE2D( volume_cache[x] );

    FREE( volume_cache );

    FREE2D( slice );
    FREE( row );

    return( resampled_volume );
}

#ifdef DEBUG
private  void  get_voxel_range(
    int      size,
    Real     min_pos,
    Real     max_pos,
    int      *min_voxel,
    int      *max_voxel,
    Real     *start_weight,
    Real     *end_weight )
{
    if( min_pos < -0.5 )
        min_pos = -0.5;
    if( max_pos > (Real) size - 0.5 )
        max_pos = (Real) size - 0.5;

    *min_voxel = ROUND( min_pos );
    *max_voxel = ROUND( max_pos );

    if( (Real) (*max_voxel) - 0.5 == max_pos )
        --(*max_voxel);

    if( *min_voxel == *max_voxel )
        *start_weight = max_pos - min_pos;
    else
    {
        *start_weight = ((Real) (*min_voxel) + 0.5) - min_pos;
        *end_weight = max_pos - ((Real) (*max_voxel) - 0.5);
    }
}
    
private  Real  get_amount_in_box(
    Volume    volume,
    int       x_min_voxel,
    int       x_max_voxel,
    int       y_min_voxel,
    int       y_max_voxel,
    int       z_min_voxel,
    int       z_max_voxel,
    Real      x_weight_start,
    Real      x_weight_end,
    Real      y_weight_start,
    Real      y_weight_end,
    Real      z_weight_start,
    Real      z_weight_end )
{
    int     x, y, z;
    Real    sum, z_sum, x_sum, value;

    sum = 0.0;

    for_inclusive( z, z_min_voxel, z_max_voxel )
    {
        z_sum = 0.0;

        for_inclusive( x, x_min_voxel, x_max_voxel )
        {
            x_sum = 0.0;

            for_inclusive( y, y_min_voxel, y_max_voxel )
            {
                value = get_volume_real_value( volume, x, y, z, 0, 0 );

                if( y == y_min_voxel )
                    value *= y_weight_start;
                else if( y == y_max_voxel )
                    value *= y_weight_end;

                x_sum += value;
            }

            if( x == x_min_voxel )
                x_sum *= x_weight_start;
            else if( x == x_max_voxel )
                x_sum *= x_weight_end;

            z_sum += x_sum;
        }

        if( z == z_min_voxel )
            z_sum *= z_weight_start;
        else if( z == z_max_voxel )
            z_sum *= z_weight_end;

        sum += z_sum;
    }

    return( sum );
}

private  Real  get_correct_amount(
    Volume   volume,
    int      x,
    int      y,
    int      z,
    Real     x_width,
    Real     y_width,
    Real     z_width )
{
    int     sizes[MAX_DIMENSIONS];
    Real    x_start_weight, x_end_weight;
    Real    y_start_weight, y_end_weight;
    Real    z_start_weight, z_end_weight;
    Real    sum;
    int     x_min_voxel, x_max_voxel;
    int     y_min_voxel, y_max_voxel;
    int     z_min_voxel, z_max_voxel;

    get_volume_sizes( volume, sizes );

    get_voxel_range( sizes[X], x - x_width, x + x_width,
                    &x_min_voxel, &x_max_voxel, &x_start_weight, &x_end_weight);
    get_voxel_range( sizes[Y], y - y_width, y + y_width,
                    &y_min_voxel, &y_max_voxel, &y_start_weight, &y_end_weight);
    get_voxel_range( sizes[Z], z - z_width, z + z_width,
                    &z_min_voxel, &z_max_voxel, &z_start_weight, &z_end_weight);

    sum = get_amount_in_box( volume,
                             x_min_voxel, x_max_voxel,
                             y_min_voxel, y_max_voxel,
                             z_min_voxel, z_max_voxel,
                             x_start_weight, x_end_weight,
                             y_start_weight, y_end_weight,
                             z_start_weight, z_end_weight );

    return( sum );
}
#endif
