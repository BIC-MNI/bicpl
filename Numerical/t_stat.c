#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

#define   N_POINTS         10000
#define   MAX_DIST         100.0
#define   N_STEPS_BETWEEN  10

private  Real  evaluate_probability_t(
    int    v,
    Real   t )
{
    Real   gamma_top, gamma_bottom, top, bottom, p;

    gamma_top = exp( gamma( ((Real) v + 1.0) / 2.0 ) );
    gamma_bottom = exp( gamma( (Real) v / 2.0 ) );

    top = gamma_top * pow( 1.0 + t * t / (Real) v, - (Real) (v+1)/ 2.0 );
    bottom = sqrt( (Real) v * PI ) * gamma_bottom;

    p = top / bottom;

    return( p );
}

private   Real  initialize_t_integral(
    int    v,
    int    n_points,
    Real   cumulative_probs[],
    Real   max_dist,
    int    n_steps_between )
{
    Real   error, cum_prob, this_p, prev_p, t, step_size;
    int    i, step;

    cumulative_probs[0] = 0.0;

    this_p = evaluate_probability_t( v, 0.0 );
    cum_prob = 0.0;
    error = 0.0;

    step_size = max_dist / (Real) (n_points-1) / (Real) n_steps_between;

    for_less( i, 0, n_points-1 )
    {
        for_less( step, 0, n_steps_between )
        {
            prev_p = this_p;

            t = (Real) (i * n_steps_between + step + 1)  /
                (Real) ((n_points-1) * n_steps_between) * max_dist;
            this_p = evaluate_probability_t( v, t );
            cum_prob += (this_p + prev_p) / 2.0 * step_size;
            error += prev_p - this_p;
        }

        cumulative_probs[i+1] = cum_prob;
    }

    error *= step_size;
    error /= 2.0;

    return( error );
}

private  Real  convert_t_stat_to_probability(
    int   n_points,
    Real  cumulative_probs[],
    Real  max_dist,
    Real  t )
{
    int   ind;
    Real  abs_t, alpha1, alpha2, value, interval_width;

    abs_t = FABS( t );

    if( abs_t >= max_dist )
        value = 0.5;
    else
    {
        interval_width = max_dist / (Real) (n_points-1);
        ind = (int) (abs_t / interval_width);
        alpha1 = abs_t / interval_width - (Real) ind;
        alpha2 = (Real) (ind+1) - abs_t / interval_width;
        value = alpha1 * cumulative_probs[ind] +
                alpha2 * cumulative_probs[ind+1];
    }

    if( t < 0.0 )
        value = 0.5 - value;
    else
        value = value + 0.5;

    return( value );
}

public  void  initialize_cumulative_t_stat(
    t_stat_struct  *stat,
    int            degrees_freedom )
{
    stat->degrees_freedom = degrees_freedom;
    stat->n_points = N_POINTS;
    stat->max_dist = MAX_DIST;

    ALLOC( stat->cumulative_probs, stat->n_points );

    (void) initialize_t_integral( stat->degrees_freedom,
                                  stat->n_points,
                                  stat->cumulative_probs,
                                  stat->max_dist,
                                  N_STEPS_BETWEEN );
}

public  void  delete_cumulative_t_stat(
    t_stat_struct  *stat )
{
    FREE( stat->cumulative_probs );
}

public  Real  get_cumulative_t_stat(
    t_stat_struct  *stat,
    Real           t )
{
    Real   prob;

    prob = convert_t_stat_to_probability( stat->n_points,
                                          stat->cumulative_probs,
                                          stat->max_dist, t );

    return( prob );
}
