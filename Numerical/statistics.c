#include  <internal_volume_io.h>
#include  <numerical.h>
#include  <data_structures.h>

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

    INITIALIZE_PRIORITY_QUEUE( heap );

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

        INSERT_IN_PRIORITY_QUEUE( heap, (char) 0, x );
    }

    for_less( i, 0, (n+1)/2 )
        REMOVE_FROM_PRIORITY_QUEUE( heap, ignored, *median );

#ifdef lint
    if( ignored != (char) 0 ) { ignored = 0; }
#endif

    DELETE_PRIORITY_QUEUE( heap );

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
