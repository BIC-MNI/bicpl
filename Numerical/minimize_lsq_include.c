#include  <internal_volume_io.h>
#include  <numerical.h>
#include  <prog_utils.h>

#define   N_SAVES          3
#define   N_BETWEEN_SAVES  30

private  Real  evaluate_fit(
    int         n_equations,
    int         n_nodes_per_equation[],
    int         *node_list[],
    LSQ_TYPE    constants[],
    LSQ_TYPE    *node_weights[],
    Real        node_values[] )
{
    int   eq, n;
    Real  fit, diff;

    fit = 0.0;

    for_less( eq, 0, n_equations )
    {
        diff = (Real) constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
        {
            diff += (Real) node_weights[eq][n] *
                    (Real) node_values[node_list[eq][n]];
        }

        fit += diff * diff;
    }

    return( fit );
}

private  void  evaluate_fit_derivative(
    int         n_equations,
    int         n_nodes_per_equation[],
    int         *node_list[],
    LSQ_TYPE    constants[],
    LSQ_TYPE    *node_weights[],
    int         n_nodes,
    Real        node_values[],
    Real        derivatives[] )
{
    int   p, n, eq;
    Real  diff;

    for_less( p, 0, n_nodes )
        derivatives[p] = 0.0;

    for_less( eq, 0, n_equations )
    {
        diff = (Real) constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
        {
            diff += (Real) node_weights[eq][n] *
                    (Real) node_values[node_list[eq][n]];
        }
        diff *= 2.0;

        for_less( n, 0, n_nodes_per_equation[eq] )
            derivatives[node_list[eq][n]] += diff * (Real) node_weights[eq][n];
    }
}

private  void  evaluate_fit_along_line(
    int          n_equations,
    int          n_nodes_per_equation[],
    int          *node_list[],
    LSQ_TYPE    constants[],
    LSQ_TYPE    *node_weights[],
    int         n_nodes,
    Real        node_values[],
    Real        line_coefs[],
    Real        *a_ptr,
    Real        *b_ptr )
{
    int   node, n, eq;
    Real  weight, constant, linear, a, b;

    a = 0.0;
    b = 0.0;

    for_less( eq, 0, n_equations )
    {
        linear = 0.0;
        constant = (Real) constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
        {
            weight = (Real) node_weights[eq][n];
            node = node_list[eq][n];
            constant += weight * node_values[node];
            linear += weight * line_coefs[node];
        }

        b += constant * linear;
        a += linear * linear;
    }

    *a_ptr = a;
    *b_ptr = 2.0 * b;
}

private  void  minimize_along_line(
    int         n_equations,
    int         n_nodes_per_equation[],
    int         *node_list[],
    LSQ_TYPE    constants[],
    LSQ_TYPE    *node_weights[],
    int         n_nodes,
    Real        node_values[],
    Real        line_coefs[] )
{
    int   node;
    Real  a, b, t;

    evaluate_fit_along_line( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights,
                             n_nodes, node_values, line_coefs, &a, &b );

    if( a == 0.0 )
        return;

    t = -b / (2.0 * a);

    for_less( node, 0, n_nodes )
        node_values[node] += t * line_coefs[node];
}

private  Real   private_minimize_lsq(
    int              n_parameters,
    int              n_equations,
    int              n_nodes_per_equation[],
    int              *node_list[],
    LSQ_TYPE         constants[],
    LSQ_TYPE         *node_weights[],
    int              n_iters,
    Real             node_values[] )
{
    Real              fit;
    int               iter, s, p;
    int               update_rate;
    Real              *saves[N_SAVES], *swap, *derivs;
    Real              last_update_time, current_time;

    for_less( s, 0, N_SAVES )
    {
        ALLOC( saves[s], n_parameters );
        for_less( p, 0, n_parameters )
            saves[s][p] = node_values[p];
    }

    ALLOC( derivs, n_parameters );

    fit = evaluate_fit( n_equations, n_nodes_per_equation,
                        node_list, constants, node_weights, node_values );

    print( "Initial  %g\n", fit );
    (void) flush_file( stdout );

    update_rate = 1;
    last_update_time = current_cpu_seconds();

    for_less( iter, 0, n_iters )
    {
        evaluate_fit_derivative( n_equations,
                                 n_nodes_per_equation,
                                 node_list, constants, node_weights,
                                 n_parameters, node_values, derivs );

        minimize_along_line( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights,
                             n_parameters, node_values, derivs );

        s = get_random_int( N_SAVES );
        for_less( p, 0, n_parameters )
        {
            derivs[p] = node_values[p] - saves[s][p];
            node_values[p] = saves[s][p];
        }

        minimize_along_line( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights,
                             n_parameters, node_values, derivs );

        if( ((iter+1) % update_rate) == 0 || iter == n_iters - 1 )
        {
            fit =  evaluate_fit( n_equations,
                             n_nodes_per_equation,
                             node_list, constants, node_weights, node_values );
            print( "%d: %g\n", iter+1, fit );
            (void) flush_file( stdout );
            current_time = current_cpu_seconds();
            if( current_time - last_update_time < 1.0 )
                update_rate *= 10;
            last_update_time = current_time;
        }

        if( (iter % N_BETWEEN_SAVES) == 0 )
        {
            swap = saves[0];
            for_less( s, 0, N_SAVES-1 )
                saves[s] = saves[s+1];
            saves[N_SAVES-1] = swap;
            for_less( p, 0, n_parameters )
                saves[s][p] = node_values[p];
        }
    }

    for_less( s, 0, N_SAVES )
        FREE( saves[s] );

    FREE( derivs );

    return( fit );
}
