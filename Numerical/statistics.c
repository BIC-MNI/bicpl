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
#include  <numerical.h>
#include  <data_structures.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Numerical/statistics.c,v 1.9 1996-02-23 20:45:37 david Exp $";
#endif

#define  DEFAULT_N_MEDIAN_BOXES    100000

#define  MAX_SAMPLES_RECORDED      100000

/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_statistics
@INPUT      : n
              samples
@OUTPUT     : min_value
              max_value
              mean_value
              std_dev
              median     - may be null pointer if median not desired
@RETURNS    : 
@DESCRIPTION: Computes the most basic statistics on a set of n samples.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1993    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  void  compute_statistics(
    int      n,
    Real     samples[],
    Real     *min_value,
    Real     *max_value,
    Real     *mean_value,
    Real     *std_dev,
    Real     *median )
{
    int                   i;
    Real                  x, min_median, max_median;
    statistics_struct     stats;
    BOOLEAN               median_is_exact, done;

    if( median != NULL )
    {
        min_median = 0.0;
        max_median = 0.0;

        for_less( i, 0, n )
        {
            x = samples[i];
            if( i == 0 )
            {
                min_median = x;
                max_median = x;
            }
            else if( x < min_median )
                min_median = x;
            else if( x > max_median )
                max_median = x;
        }
    }
    else
    {
        min_median = 0.0;
        max_median = -1.0;
    }

    initialize_statistics( &stats, min_median, max_median );

    done = FALSE;

    while( !done )
    {
        for_less( i, 0, n )
            add_sample_to_statistics( &stats, samples[i] );

        get_statistics( &stats, NULL, mean_value, median, &median_is_exact,
                        min_value, max_value, std_dev );

        if( median != NULL && !median_is_exact )
            restart_statistics_with_narrower_median_range( &stats );
        else
            done = TRUE;
    }

    terminate_statistics( &stats );
}

public  void  initialize_statistics(
    statistics_struct  *stats,
    Real               median_lower_bound,
    Real               median_upper_bound )
{
    int    i;

    stats->n_samples = 0;
    stats->sum_x = 0.0;
    stats->sum_xx = 0.0;

    stats->min_median_range = median_lower_bound;
    stats->max_median_range = median_upper_bound;

    if( median_lower_bound < median_upper_bound )
    {
        stats->n_median_boxes = DEFAULT_N_MEDIAN_BOXES;
        ALLOC( stats->median_box_counts, stats->n_median_boxes );
        ALLOC( stats->median_box_values, stats->n_median_boxes );

        for_less( i, 0, stats->n_median_boxes )
            stats->median_box_counts[i] = 0;

        stats->n_below_median_range = 0;
        stats->n_above_median_range = 0;
    }
}

public  void  add_sample_to_statistics(
    statistics_struct  *stats,
    Real               sample )
{
    int         median_box;

    if( stats->n_samples == 0 )
    {
        stats->min_value = sample;
        stats->max_value = sample;
    }
    else if( sample < stats->min_value )
        stats->min_value = sample;
    else if( sample > stats->max_value )
        stats->max_value = sample;

    if( stats->n_samples < MAX_SAMPLES_RECORDED )
    {
        SET_ARRAY_SIZE( stats->samples, stats->n_samples,
                        stats->n_samples+1, DEFAULT_CHUNK_SIZE );
        stats->samples[stats->n_samples] = sample;
    }
    else if( stats->n_samples == MAX_SAMPLES_RECORDED )
        FREE( stats->samples );

    stats->n_samples += 1;

    stats->sum_x += sample;
    stats->sum_xx += sample * sample;

    if( stats->min_median_range < stats->max_median_range )
    {
        if( sample < stats->min_median_range )
            ++stats->n_below_median_range;
        else if( sample >= stats->max_median_range )
            ++stats->n_above_median_range;
        else
        {
            median_box = stats->n_median_boxes *
                         (sample - stats->min_median_range)/
                         (stats->max_median_range - stats->min_median_range);

            ++stats->median_box_counts[median_box];
            if( stats->median_box_counts[median_box] == 1 )
                stats->median_box_values[median_box] = sample;
        }
    }
}

private  void  get_median(
    statistics_struct  *stats,
    Real               *min_range,
    Real               *max_range )
{
    int   box_index, median_index;

    median_index = stats->n_samples / 2;

    if( stats->n_samples <= MAX_SAMPLES_RECORDED )
    {
        int  i, j, best_index;
        Real tmp;

        for_less( i, 0, stats->n_samples-1 )
        {
            best_index = i;
            for_less( j, i+1, stats->n_samples )
            {
                if( stats->samples[j] < stats->samples[best_index] )
                    best_index = j;
            }

            tmp = stats->samples[best_index];
            stats->samples[best_index] = stats->samples[i];
            stats->samples[i] = tmp;
        }

        *min_range = stats->samples[median_index];
        *max_range = stats->samples[median_index];

        return;
    }

    if( stats->min_median_range >= stats->max_median_range )
    {
        *min_range = -1.0e30;
        *max_range = 1.0e30;
        return;
    }

    if( median_index < stats->n_below_median_range )
    {
        *min_range = stats->min_value;
        *max_range = stats->min_median_range;
        return;
    }

    median_index -= stats->n_below_median_range;

    box_index = 0;
    while( box_index < stats->n_median_boxes &&
           median_index >= stats->median_box_counts[box_index] )
    {
        median_index -= stats->median_box_counts[box_index];
        ++box_index;
    }

    if( box_index == stats->n_median_boxes )
    {
        *min_range = stats->max_median_range;
        *max_range = stats->max_value;
    }
    else
    {
        if( stats->median_box_counts[box_index] == 1 )
        {
            *min_range = stats->median_box_values[box_index];
            *max_range = stats->median_box_values[box_index];
        }
        else
        {
            *min_range = stats->min_median_range +
                         (stats->max_median_range - stats->min_median_range) *
                          (Real) box_index / (Real) stats->n_median_boxes;
            *max_range = stats->min_median_range +
                         (stats->max_median_range - stats->min_median_range) *
                          (Real) (box_index+1) / (Real) stats->n_median_boxes;
        }
    }
}

public  void  restart_statistics_with_narrower_median_range(
    statistics_struct  *stats )
{
    Real   min_median_range, max_median_range;

    get_median( stats, &min_median_range, &max_median_range );

    if( min_median_range == max_median_range )
    {
        min_median_range = stats->min_median_range;
        max_median_range = stats->max_median_range;
        print_error( "Median range already narrow enough.\n" );
    }

    terminate_statistics( stats );

    initialize_statistics( stats, min_median_range, max_median_range );
}

public  void  get_statistics(
    statistics_struct  *stats,
    int                *n_samples,
    Real               *mean,
    Real               *median,
    BOOLEAN            *median_is_exact,
    Real               *min_value,
    Real               *max_value,
    Real               *std_deviation )
{
    BOOLEAN  exact;
    int      n;
    Real     sum_x, sum_xx, min_median_range, max_median_range, variance;

    if( n_samples != NULL )
        *n_samples = stats->n_samples;

    if( stats->n_samples <= 0 )
    {
        if( median_is_exact != NULL )
            *median_is_exact = TRUE;
        return;
    }

    if( median != NULL )
    {
        get_median( stats, &min_median_range, &max_median_range );

        exact = (min_median_range == max_median_range);

        if( median_is_exact != NULL )
            *median_is_exact = exact;

        if( exact )
            *median = min_median_range;
        else
            *median = (min_median_range + max_median_range) / 2.0;
    }

    if( min_value != NULL )
        *min_value = stats->min_value;

    if( max_value != NULL )
        *max_value = stats->max_value;

    n = stats->n_samples;
    sum_x = stats->sum_x;
    sum_xx = stats->sum_xx;

    if( mean != NULL )
        *mean = sum_x / (Real) n;

    if( n == 1 )
        variance = 0.0;
    else
        variance = (sum_xx - sum_x * sum_x / (Real) n) / (Real) (n - 1);

    if( std_deviation != NULL )
    {
        if( variance <= 0.0 )
            *std_deviation = 0.0;
        else
            *std_deviation = sqrt( variance );
    }
}

public  void  terminate_statistics(
    statistics_struct  *stats )
{
    if( stats->n_samples > 0 && stats->n_samples <= MAX_SAMPLES_RECORDED )
        FREE( stats->samples );

    if( stats->min_median_range < stats->max_median_range )
    {
        FREE( stats->median_box_counts );
        FREE( stats->median_box_values );
    }
}
