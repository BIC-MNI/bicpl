#include  <def_mni.h>

#define  N_STD_DEVIATIONS  3.0
#define  N_SAMPLES         9

public  int  get_slice_weights_for_filter(
    Volume         volume,
    int            axis,
    Real           position,
    Filter_types   filter_type,
    Real           full_width_half_max,
    Real           *positions[],
    Real           *weights[] )
{
    int   i, n_slices, sizes[N_DIMENSIONS], size;
    int   slice, first_slice, last_slice, s;
    Real  sum_weights, pos, frac, min_limit, max_limit, start, end, weight;
    Real  half_width, sigma, start_interval, end_interval, x;

    get_volume_sizes( volume, sizes );
    size = sizes[axis];

    n_slices = 0;

    switch( filter_type )
    {
    case NEAREST_NEIGHBOUR:
        if( position < -0.5 )
            pos = 0.0;
        else if( position >= (Real) size - 0.5 )
            pos = (Real) (size - 1);
        else
            pos = ROUND( position );
        ALLOC( *positions, 1 );
        ALLOC( *weights, 1 );
        (*positions)[0] = pos;
        (*weights)[0] = 1.0;
        n_slices = 1;
        break;

    case LINEAR_INTERPOLATION:
        if( position < 0.0 )
            pos = 0.0;
        else if( position > (Real) (size - 1) )
            pos = (Real) (size - 1);
        else
            pos = position;

        ALLOC( *positions, 2 );
        ALLOC( *weights, 2 );

        frac = FRACTION( pos );
        (*positions)[0] = (Real) (int) pos;
        (*weights)[0] = frac;
        n_slices = 1;

        if( frac > 0.0 )
        {
            (*positions)[1] = (Real) ((int) pos + 1);
            (*weights)[1] = 1.0 - frac;
            n_slices = 2;
        }
        break;

    case BOX_FILTER:
    case TRIANGLE_FILTER:
    case GAUSSIAN_FILTER:
        if( position < -0.5 )
            pos = -0.5;
        else if( position > (Real) size - 0.5 )
            pos = (Real) size - 0.5;
        else
            pos = position;

        switch( filter_type )
        {
        case BOX_FILTER:       half_width = full_width_half_max / 2.0;  break;
        case TRIANGLE_FILTER:  half_width = full_width_half_max;  break;
        case GAUSSIAN_FILTER:
            sigma = full_width_half_max / 2.0 / sqrt(log(2.0));
            half_width = N_STD_DEVIATIONS * sigma;
            break;
        }
        min_limit = pos - half_width;
        max_limit = pos + half_width;
        first_slice = (int) (min_limit + 0.5);
        if( first_slice < 0 )
            first_slice = 0;
        else if( first_slice >= size )
            first_slice = size-1;

        last_slice = (int) (max_limit + 0.5);
        if( last_slice < 0 )
            last_slice = 0;
        else if( last_slice >= size )
            last_slice = size-1;

        if( IS_INT(max_limit+0.5) && last_slice > first_slice )
            --last_slice;

        n_slices = last_slice - first_slice + 1;
        ALLOC( *positions, n_slices );
        ALLOC( *weights, n_slices );

        for_inclusive( slice, first_slice, last_slice )
        {
            (*positions)[slice-first_slice] = (Real) slice;
            if( slice == first_slice )
                start_interval = min_limit;
            else
                start_interval = (Real) slice - 0.5;
            if( slice == last_slice )
                end_interval = max_limit;
            else
                end_interval = (Real) slice + 0.5;

            switch( filter_type )
            {
            case BOX_FILTER:
                weight = end_interval - start_interval;
                break;
            case TRIANGLE_FILTER:
                weight = 0.0;
                if( start_interval < pos )
                {
                    end = MIN( end_interval, pos);
                    weight = (end - start_interval) *
                             ((start_interval + end) / 2.0 - min_limit) /
                             (pos - min_limit);
                }
                if( end_interval > pos )
                {
                    start = MAX( start_interval, pos);
                    weight += (end_interval - start) *
                             (max_limit - (start + end_interval) / 2.0) /
                             (max_limit - pos);
                }
                break;
            case GAUSSIAN_FILTER:
                weight = 0.0;
                if( end_interval > start_interval )
                {
                    for_less( s, 0, N_SAMPLES )
                    {
                        x = start_interval + (end_interval - start_interval) *
                            ((Real) s + 0.5) / (Real) N_SAMPLES - pos;
                        weight += (end_interval - start_interval) *
                                  exp ( - x * x / sigma / sigma );
                    }
                }
                break;
            }

            (*weights)[slice-first_slice] = weight;
        }

        break;
    }

    if( n_slices < 1 )
    {
        HANDLE_INTERNAL_ERROR( "get_slice_weights_for_filter" );
    }

    sum_weights = 0.0;
    for_less( i, 0, n_slices )
        sum_weights += (*weights)[i];

    for_less( i, 0, n_slices )
    {
        if( sum_weights == 0.0 )
            (*weights)[i] = 1.0 / (Real) n_slices;
        else
            (*weights)[i] /= sum_weights;
    }

    return( n_slices );
}
