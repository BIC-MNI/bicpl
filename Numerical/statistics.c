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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Numerical/statistics.c,v 1.5 1995-07-31 13:45:25 david Exp $";
#endif

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
    int                              i;
    char                             ignored;
    Real                             x, sum_x, sum_xx, variance;
    PRIORITY_QUEUE_STRUCT( char )    heap;

    *min_value = 0.0;
    *max_value = 0.0;
    sum_x = 0.0;
    sum_xx = 0.0;

    if( median != NULL )
    {
        INITIALIZE_PRIORITY_QUEUE( heap );
    }

    for_less( i, 0, n )
    {
        x = samples[i];
        if( i == 0 )
        {
            *min_value = x;
            *max_value = x;
        }
        else if( x < *min_value )
            *min_value = x;
        else if( x > *max_value )
            *max_value = x;

        sum_x += x;
        sum_xx += x * x;

        if( median != NULL )
        {
            INSERT_IN_PRIORITY_QUEUE( heap, (char) 0, x );
        }
    }

    if( median != NULL )
    {
        for_less( i, 0, (n+1)/2 )
            REMOVE_FROM_PRIORITY_QUEUE( heap, ignored, *median );

        DELETE_PRIORITY_QUEUE( heap );
    }

#ifdef lint
    if( ignored != (char) 0 ) { ignored = 0; }
#endif

    if( n == 0 )
        *mean_value = 0.0;
    else
        *mean_value = sum_x / (Real) n;

    if( n == 1 )
        variance = 0.0;
    else
        variance = (sum_xx - sum_x * sum_x / (Real) n) / (Real) (n - 1);

    *std_dev = sqrt( variance );
}
