#include  <internal_volume_io.h>
#include  <numerical.h>

private  Real  evaluate_fit(
    int     n_equations,
    int     n_nodes_per_equation[],
    int     *node_list[],
    Real    constants[],
    Real    *node_weights[],
    Real    node_values[] )
{
    int   eq, n;
    Real  fit, diff;

    fit = 0.0;

    for_less( eq, 0, n_equations )
    {
        diff = (Real) constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
            diff += node_weights[eq][n] * node_values[node_list[eq][n]];

        fit += diff * diff;
    }

    return( fit );
}

private  void  evaluate_fit_along_line(
    int     n_equations,
    int     n_nodes_per_equation[],
    int     *node_list[],
    Real    constants[],
    Real    *node_weights[],
    int     n_nodes,
    Real    node_values[],
    Real    line_coefs[],
    Real    *a_ptr,
    Real    *b_ptr )
{
    int   node, n, eq;
    Real  weight, constant, linear, a, b;

    a = 0.0;
    b = 0.0;

    for_less( eq, 0, n_equations )
    {
        linear = 0.0;
        constant = constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
        {
            weight = node_weights[eq][n];
            node = node_list[eq][n];
            constant += weight * node_values[node];
            linear += weight * line_coefs[node];
        }

        b += 2.0 * constant * linear;
        a += linear * linear;
    }

    *a_ptr = a;
    *b_ptr = b;
}

private  void  minimize_along_line(
    int     n_equations,
    int     n_nodes_per_equation[],
    int     *node_list[],
    Real    constants[],
    Real    *node_weights[],
    int     n_nodes,
    Real    node_values[],
    Real    line_coefs[] )
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

private  void  evaluate_fit_derivative(
    int     n_equations,
    int     n_nodes_per_equation[],
    int     *node_list[],
    Real    constants[],
    Real    *node_weights[],
    int     n_nodes,
    Real    node_values[],
    Real    derivatives[] )
{
    int   node, p, n, eq;
    Real  diff;

    for_less( p, 0, n_nodes )
        derivatives[p] = 0.0;

    for_less( eq, 0, n_equations )
    {
        diff = constants[eq];
        for_less( n, 0, n_nodes_per_equation[eq] )
            diff += node_weights[eq][n] * node_values[node_list[eq][n]];
        diff *= 2.0;

        for_less( n, 0, n_nodes_per_equation[eq] )
        {
            node = node_list[eq][n];
            derivatives[node] += diff * node_weights[eq][n];
        }
    }
}

private  void  minimize_cost(
    int     n_equations,
    int     n_nodes_per_equation[],
    int     *node_list[],
    Real    constants[],
    Real    *node_weights[],
    int     n_nodes,
    Real    node_values[] )
{
    int     n, iter, n_iters;
    Real    *next_values, *derivs;

    ALLOC( derivs, n_nodes );
    ALLOC( next_values, n_nodes );
    for_less( n, 0, n_nodes )
        next_values[n] = node_values[n];

    if( getenv( "N_ITERS" ) == NULL ||
        sscanf( getenv("N_ITERS"), "%d", &n_iters ) != 1 )
        n_iters = 50;

    for_less( iter, 0, n_iters )
    {
        evaluate_fit_derivative( n_equations, n_nodes_per_equation,
                                 node_list, constants, node_weights,
                                 n_nodes, next_values, derivs );

        minimize_along_line( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights,
                             n_nodes, next_values, derivs );

        for_less( n, 0, n_nodes )
        {
            derivs[n] = next_values[n] - node_values[n];
            next_values[n] = node_values[n];
        }

        minimize_along_line( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights,
                             n_nodes, next_values, derivs );
    }

    for_less( n, 0, n_nodes )
        node_values[n] = next_values[n];

    FREE( next_values );
    FREE( derivs );
}

public  Real   minimize_lsq(
    int              n_parameters,
    int              n_equations,
    int              n_nodes_per_equation[],
    int              *node_list[],
    Real             constants[],
    Real             *node_weights[],
    int              n_iters,
    Real             node_values[] )
{
    Real              fit;
    int               iter;
    int               next_change, update_rate;

    fit = evaluate_fit( n_equations, n_nodes_per_equation,
                        node_list, constants, node_weights, node_values );

    print( "Initial  %g\n", fit );

    update_rate = 1;
    next_change = 10;
    for_less( iter, 0, n_iters )
    {
        minimize_cost( n_equations, n_nodes_per_equation,
                       node_list, constants, node_weights,
                       n_parameters, node_values );

        fit =  evaluate_fit( n_equations, n_nodes_per_equation,
                             node_list, constants, node_weights, node_values );

        if( ((iter+1) % update_rate) == 0 )
        {
            print( "%d: %g\n", iter+1, fit );
        }

        if( iter+1 == next_change )
        {
            update_rate *= 10;
            next_change *= 100;
        }
    }

    return( fit );
}
