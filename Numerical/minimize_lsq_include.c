#include  <internal_volume_io.h>
#include  <numerical.h>
#include  <prog_utils.h>

#define   N_SAVES          3
#define   N_BETWEEN_SAVES  30

private  Real  evaluate_fit(
    int              n_parameters,
    Real             constant_term,
    LSQ_TYPE         *linear_terms,
    LSQ_TYPE         *square_terms,
    int              n_cross_terms[],
    int              *cross_parms[],
    LSQ_TYPE         *cross_terms[],
    Real             parm_values[] )
{
    int   parm, n;
    Real  fit, parm_val;

    fit = constant_term;

    for_less( parm, 0, n_parameters )
    {
        parm_val = parm_values[parm];
        fit += parm_val * ((Real) linear_terms[parm] +
                            parm_val * (Real) square_terms[parm]);

        for_less( n, 0, n_cross_terms[parm] )
            fit += parm_val * parm_values[cross_parms[parm][n]] *
                   (Real) cross_terms[parm][n];
    }

    return( fit );
}

private  void  evaluate_fit_derivative(
    int              n_parameters,
    Real             constant_term,
    LSQ_TYPE         *linear_terms,
    LSQ_TYPE         *square_terms,
    int              n_cross_terms[],
    int              *cross_parms[],
    LSQ_TYPE         *cross_terms[],
    Real             parm_values[],
    Real             derivatives[] )
{
    int   parm, n, neigh_parm;
    Real  term, parm_val;

    for_less( parm, 0, n_parameters )
        derivatives[parm] = 0.0;

    for_less( parm, 0, n_parameters )
    {
        parm_val = parm_values[parm];

        derivatives[parm] += (Real) linear_terms[parm] +
                             2.0 * parm_val * (Real) square_terms[parm];

        for_less( n, 0, n_cross_terms[parm] )
        {
            neigh_parm = cross_parms[parm][n];
            term = (Real) cross_terms[parm][n];
            derivatives[parm] += parm_values[neigh_parm] * term;
            derivatives[neigh_parm] += (Real) parm_val * term;
        }
    }
}

private  void  evaluate_fit_along_line(
    int              n_parameters,
    Real             constant_term,
    LSQ_TYPE         *linear_terms,
    LSQ_TYPE         *square_terms,
    int              n_cross_terms[],
    int              *cross_parms[],
    LSQ_TYPE         *cross_terms[],
    Real             parm_values[],
    Real             line_coefs[],
    Real             *a_ptr,
    Real             *b_ptr )
{
    int   parm, n, neigh_parm;
    Real  weight, n_line_coef, square, a, b, parm_val, line_coef;

    a = 0.0;
    b = 0.0;

    for_less( parm, 0, n_parameters )
    {
        parm_val = parm_values[parm];
        square = (Real) square_terms[parm];
        line_coef = line_coefs[parm];

        b += line_coef * ((Real) linear_terms[parm] +
                               square * 2.0 * parm_val);
        a += square * line_coef * line_coef;

        for_less( n, 0, n_cross_terms[parm] )
        {
            neigh_parm = cross_parms[parm][n];
            weight = (Real) cross_terms[parm][n];
            n_line_coef = line_coefs[neigh_parm];
            b += weight * (line_coef * parm_values[neigh_parm] +
                           n_line_coef * parm_val);
            a += weight * line_coef * n_line_coef;
        }
    }

    *a_ptr = a;
    *b_ptr = b;
}

private  void  minimize_along_line(
    int              n_parameters,
    Real             constant_term,
    LSQ_TYPE         *linear_terms,
    LSQ_TYPE         *square_terms,
    int              n_cross_terms[],
    int              *cross_parms[],
    LSQ_TYPE         *cross_terms[],
    Real             max_step_size,
    Real             parm_values[],
    Real             line_coefs[] )
{
    int   parm;
    Real  a, b, t, step_size;

    evaluate_fit_along_line( n_parameters, constant_term, linear_terms,
                             square_terms, n_cross_terms, cross_parms,
                             cross_terms, parm_values, line_coefs, &a, &b );

    if( a == 0.0 )
        return;

    t = -b / (2.0 * a);

    if( max_step_size >= 0.0 )
    {
        step_size = 0.0;
        for_less( parm, 0, n_parameters )
            step_size += t * t * line_coefs[parm] * line_coefs[parm];

        step_size = sqrt( step_size );
        if( step_size > max_step_size )
            t *= max_step_size / step_size;
    }

    for_less( parm, 0, n_parameters )
        parm_values[parm] += t * line_coefs[parm];
}

private  Real   private_minimize_lsq(
    int              n_parameters,
    Real             constant_term,
    LSQ_TYPE         *linear_terms,
    LSQ_TYPE         *square_terms,
    int              n_cross_terms[],
    int              *cross_parms[],
    LSQ_TYPE         *cross_terms[],
    Real             max_step_size,
    int              n_iters,
    Real             parm_values[] )
{
    Real              fit, len;
    int               iter, s, p, n_between_saves;
    int               update_rate;
    Real              *saves[N_SAVES], *swap, *derivs;
    Real              last_update_time, current_time;

    for_less( s, 0, N_SAVES )
    {
        ALLOC( saves[s], n_parameters );
        for_less( p, 0, n_parameters )
            saves[s][p] = parm_values[p];
    }

    n_between_saves = n_iters / N_SAVES;
    n_between_saves = MAX( n_between_saves, 1 );
    n_between_saves = MIN( n_between_saves, N_BETWEEN_SAVES );

    ALLOC( derivs, n_parameters );

    fit = evaluate_fit( n_parameters, constant_term, linear_terms,
                        square_terms, n_cross_terms, cross_parms, cross_terms,
                        parm_values );

    print( "Initial  %g\n", fit );
    (void) flush_file( stdout );

    update_rate = 1;
    last_update_time = current_cpu_seconds();

    for_less( iter, 0, n_iters )
    {
        evaluate_fit_derivative( n_parameters, constant_term, linear_terms,
                                 square_terms, n_cross_terms,
                                 cross_parms, cross_terms,
                                 parm_values, derivs );

        len = 0.0;
        for_less( p, 0, n_parameters )
            len += derivs[p] * derivs[p];

        len = sqrt( len );
        for_less( p, 0, n_parameters )
            derivs[p] /= len;

        minimize_along_line( n_parameters, constant_term, linear_terms,
                             square_terms, n_cross_terms, cross_parms,
                             cross_terms,
                             max_step_size, parm_values, derivs );

        s = get_random_int( N_SAVES );
        len = 0.0;
        for_less( p, 0, n_parameters )
        {
            derivs[p] = parm_values[p] - saves[s][p];
            len += derivs[p] * derivs[p];
            parm_values[p] = saves[s][p];
        }

        len = sqrt( len );
        for_less( p, 0, n_parameters )
            derivs[p] /= len;

        minimize_along_line( n_parameters, constant_term, linear_terms,
                             square_terms, n_cross_terms, cross_parms,
                             cross_terms,
                             max_step_size, parm_values, derivs );

#ifdef DEBUG
{
int  p, n;
static Real sum, prev_sum;

sum = constant_term;
for_less( p, 0, n_parameters )
{
    sum += (Real) linear_terms[p];
    sum += (Real) square_terms[p];
    for_less( n, 0, n_cross_terms[p] )
    {
        sum += (Real) cross_parms[p][n];
        sum += (Real) cross_terms[p][n];
    }
}

fit =  evaluate_fit( n_parameters, constant_term, linear_terms,
square_terms, n_cross_terms, cross_parms,
cross_terms, parm_values );

if( fit < 0.0 )
{
    handle_internal_error( "fit < 0" );
fit =  evaluate_fit( n_parameters, constant_term, linear_terms,
square_terms, n_cross_terms, cross_parms,
cross_terms, parm_values );
}
if( iter > 0 && sum != prev_sum )
    print( "Found it:   " );
print( "%d: %g %g\n", iter+1, fit, sum );
prev_sum = sum;
}
#endif

        if( ((iter+1) % update_rate) == 0 || iter == n_iters - 1 )
        {
            fit =  evaluate_fit( n_parameters, constant_term, linear_terms,
                                 square_terms, n_cross_terms, cross_parms,
                                 cross_terms, parm_values );

            print( "%d: %g\n", iter+1, fit );
            (void) flush_file( stdout );
            current_time = current_cpu_seconds();
            if( current_time - last_update_time < 1.0 )
                update_rate *= 10;
            last_update_time = current_time;
        }

        if( (iter % n_between_saves) == 0 )
        {
            swap = saves[0];
            for_less( s, 0, N_SAVES-1 )
                saves[s] = saves[s+1];
            saves[N_SAVES-1] = swap;
            for_less( p, 0, n_parameters )
                saves[s][p] = parm_values[p];
        }
    }

    for_less( s, 0, N_SAVES )
        FREE( saves[s] );

    FREE( derivs );

    return( fit );
}
