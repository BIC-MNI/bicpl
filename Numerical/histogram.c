#include  <histogram.h>
#include  <alloc.h>

public  void  initialize_histogram(
    histogram_struct  *histogram,
    Real              delta,
    Real              offset )
{
    histogram->delta = delta;
    histogram->offset = offset;

    histogram->min_index = 0;
    histogram->max_index = -1;
}

public  void  delete_histogram(
    histogram_struct  *histogram )
{
    if( histogram->max_index >= histogram->min_index )
    {
        FREE( histogram->counts );
    }
}

private  int  get_histogram_index(
    histogram_struct  *histogram,
    Real              value )
{
    int    ind;

    ind = FLOOR( (value - histogram->offset) / histogram->delta );

    return( ind );
}

private  Real  convert_index_to_value(
    histogram_struct  *histogram,
    int               ind )
{
    return( ind * histogram->delta + histogram->offset );
}

public  void  add_to_histogram(
    histogram_struct  *histogram,
    Real              value )
{
    int   ind, i, prev_size, new_size;

    ind = get_histogram_index( histogram, value );

    if( histogram->min_index > histogram->max_index )
    {
        ALLOC( histogram->counts, 1 );
        histogram->counts[0] = 1;
        histogram->min_index = ind;
        histogram->max_index = ind;
    }
    else if( ind < histogram->min_index )
    {
        prev_size = histogram->max_index - histogram->min_index + 1;
        new_size = histogram->max_index - ind + 1;
        SET_ARRAY_SIZE( histogram->counts, prev_size, new_size, 1 );

        for( i = histogram->max_index;  i >= histogram->min_index;  --i )
        {
            histogram->counts[i-ind] =
                         histogram->counts[i-histogram->min_index];
        }

        for_less( i, ind, histogram->min_index )
            histogram->counts[i-ind] = 0;

        histogram->counts[0] = 1;

        histogram->min_index = ind;
    }
    else if( ind > histogram->max_index )
    {
        prev_size = histogram->max_index - histogram->min_index + 1;
        new_size = ind - histogram->min_index + 1;
        SET_ARRAY_SIZE( histogram->counts, prev_size, new_size, 1 );

        for_inclusive( i, histogram->max_index + 1, ind )
            histogram->counts[i-histogram->min_index] = 0;

        histogram->counts[ind-histogram->min_index] = 1;

        histogram->max_index = ind;
    }
    else
        ++histogram->counts[ind-histogram->min_index];
}

private  void  get_histogram_range(
    histogram_struct  *histogram,
    Real              *min_value,
    Real              *max_value )
{
    *min_value = convert_index_to_value( histogram, histogram->min_index );
    *max_value = convert_index_to_value( histogram, histogram->max_index + 1 );
}

private  int  get_histogram_max_count(
    histogram_struct  *histogram )
{
    int   ind, max_count;

    max_count = 0;
    for_inclusive( ind, histogram->min_index, histogram->max_index )
    {
         if( ind == 0 ||
             histogram->counts[ind-histogram->min_index] > max_count )
             max_count = histogram->counts[ind-histogram->min_index];
    }

    return( max_count );
}

public  void  display_histogram(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size )
{
    int   x, y, *n_chars, max_count, left_ind, right_ind, ind;
    Real  min_value, max_value, left, right, left_side, right_side;
    Real  weight, sum_weight, sum_count;

    ALLOC( n_chars, x_size );

    get_histogram_range( histogram, &min_value, &max_value );
    max_count = get_histogram_max_count( histogram );

    for_less( x, 0, x_size )
    {
        left = min_value + (Real) x / (Real) x_size * (max_value - min_value);
        right = min_value + (Real) (x+1) / (Real) x_size *
                (max_value - min_value);

        left_ind = get_histogram_index( histogram, left );
        right_ind = get_histogram_index( histogram, left );

        sum_weight = 0.0;
        sum_count = 0.0;

        for_inclusive( ind, left_ind, right_ind )
        {
            left_side = convert_index_to_value( histogram, ind );
            right_side = convert_index_to_value( histogram, ind+1 );

            weight = MIN( right_side, right ) - MAX( left_side, left );

            sum_count += weight *
                         (Real) histogram->counts[ind-histogram->min_index];
            sum_weight += weight;
        }

        n_chars[x] = ROUND(sum_count / sum_weight *
                           (Real) y_size / (Real) max_count );
    }

    for( y = y_size-1;  y >= 0;  --y )
    {
        for_less( x, 0, x_size )
        {
            if( y < n_chars[x] )
                 print( "#" );
            else
                 print( " " );
        }

        print( "\n" );
    }

    print( "%g to %g with max count = %d\n", min_value, max_value, max_count );

    FREE( n_chars );
}
