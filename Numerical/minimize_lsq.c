#include  <internal_volume_io.h>
#include  <numerical.h>
#include  <prog_utils.h>

#define  LSQ_TYPE      Real
#define  MINIMIZE_LSQ  minimize_lsq

#include  "minimize_lsq_include.c"

public  Real   minimize_lsq(
    int              n_parameters,
    int              n_equations,
    int              n_nodes_per_equation[],
    int              *node_list[],
    Real             constants[],
    Real             *node_weights[],
    Real             max_step_size,
    int              n_iters,
    Real             node_values[] )
{
    return( private_minimize_lsq( n_parameters, n_equations,
                                  n_nodes_per_equation, node_list,
                                  constants, node_weights, max_step_size,
                                  n_iters, node_values ) );
}
