#ifndef  DEF_numeric_prototypes
#define  DEF_numeric_prototypes

#ifdef __cplusplus
extern "C" {
#endif

public  void  initialize_amoeba(
    amoeba_struct     *amoeba,
    int               n_parameters,
    Real              initial_parameters[],
    Real              parameter_deltas[],
    amoeba_function   function,
    void              *function_data,
    Real              tolerance );

public  Real  get_amoeba_parameters(
    amoeba_struct  *amoeba,
    Real           parameters[] );

public  void  terminate_amoeba(
    amoeba_struct  *amoeba );

public  BOOLEAN  perform_amoeba(
    amoeba_struct  *amoeba );

public  BOOLEAN  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] );

public  BOOLEAN  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse );

public  Real  gradient_steps_minimize_function(
    int   n_dims,
    Real  initial_parameters[],
    Real  initial_step_sizes[],
    Real  (*func) ( Real [], void * ),
    void  *func_data,
    int   n_search_dims,
    int   max_iterations,
    Real  tolerance,
    Real  solution[] );

public  void  initialize_histogram(
    histogram_struct  *histogram,
    Real              delta,
    Real              offset );

public  void  delete_histogram(
    histogram_struct  *histogram );

public  void  add_to_histogram(
    histogram_struct  *histogram,
    Real              value );

public  int  get_histogram_counts(
    histogram_struct  *histogram,
    Real              *counts[],
    Real              filter_width,
    Real              *scale_factor,
    Real              *trans_factor );

public  void  display_histogram(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size );

public  void  create_histogram_line(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size,
    Real              filter_width,
    lines_struct      *lines );

public  BOOLEAN  least_squares(
    int     n_points,
    int     n_dims,
    Real    **points,
    Real    values[],
    Real    parameters[] );

public  void  initialize_linear_least_squares(
    linear_least_squares   *lsq,
    int                    n_parameters );

public  void  add_to_linear_least_squares(
    linear_least_squares   *lsq,
    Real                   parameter_coefs[],
    Real                   constant );

public  BOOLEAN  get_linear_least_squares_solution(
    linear_least_squares   *lsq,
    Real                   solution[] );

public  void  delete_linear_least_squares(
    linear_least_squares   *lsq );

public  BOOLEAN  singular_value_decomposition(
    int    m,
    int    n,
    Real   **a,
    Real   w[],
    Real   **v );

public  Real   minimize_lsq(
    int              n_parameters,
    Real             constant_term,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[],
    Real             max_step_size,
    int              n_iters,
    Real             node_values[] );

public  void  initialize_lsq_terms(
    int              n_parameters,
    Real             *constant_term,
    Real             *linear_terms[],
    Real             *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    Real             **cross_terms[] );

public  void  reset_lsq_terms(
    int              n_parameters,
    Real             *constant_term,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[] );

public  void  add_to_lsq_terms(
    int              n_parameters,
    Real             *constant_term,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[],
    int              n_in_list,
    int              list[],
    Real             weights[],
    Real             constant,
    int              alloc_increment );

public  void  realloc_lsq_terms(
    int              n_parameters,
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[] );

public  void  delete_lsq_terms(
    int              n_parameters,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[] );

public  Real   minimize_lsq_float(
    int              n_parameters,
    Real             constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    Real             max_step_size,
    int              n_iters,
    Real             node_values[] );

public  void  initialize_lsq_terms_float(
    int              n_parameters,
    Real             *constant_term,
    float            *linear_terms[],
    float            *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

public  void  reset_lsq_terms_float(
    int              n_parameters,
    Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

public  void  add_to_lsq_terms_float(
    int              n_parameters,
    Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    int              n_in_list,
    int              list[],
    Real             weights[],
    Real             constant,
    int              alloc_increment );

public  void  realloc_lsq_terms_float(
    int              n_parameters,
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

public  void  delete_lsq_terms_float(
    int              n_parameters,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

public  void  create_lsq_hypersurface_float(
    STRING           filename,
    int              parm1,
    int              parm2,
    int              x_size,
    int              y_size,
    Real             x_min,
    Real             x_max,
    Real             y_min,
    Real             y_max,
    Real             scale,
    int              n_parameters,
    Real             constant,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    Real             parameters[] );

public  BOOLEAN  numerically_close(
    Real  n1,
    Real  n2,
    Real  threshold_ratio );

public  Real  get_good_round_value(
    Real    value );

public  Real  round_to_nearest_multiple(
    Real    value,
    Real    multiple_value );

public  int  solve_quadratic(
    Real   a,
    Real   b,
    Real   c,
    Real   *solution1,
    Real   *solution2 );

public  int solve_cubic(
    Real   a,
    Real   b,
    Real   c,
    Real   d,
    Real   s[ 3 ] );

public  Real  evaluate_polynomial(
    int     n,
    Real    poly[],
    Real    u );

public  int  get_roots_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    accuracy,
    Real    roots[] );

public  BOOLEAN  get_range_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    min_val,
    Real    max_val,
    Real    accuracy,
    Real    *u_min_range,
    Real    *u_max_range );

public  void  initialize_quadratic(
    int              n_parameters,
    Real             *constant_term,
    float            *linear_terms[],
    float            *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

public  void  zero_quadratic(
    int              n_parameters,
    Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

public  void  add_to_quadratic_cross_term(
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[],
    int              parm1,
    int              parm2,
    Real             value,
    int              alloc_increment );

public  void  realloc_quadratic_cross_terms(
    int              n_parameters,
    int              n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

public  void  delete_quadratic(
    int              n_parameters,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

public  Real   evaluate_quadratic(
    int           n_parameters,
    float         parameters[],
    Real          constant,
    float         linear[],
    float         square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    float         *cross_terms[] );

public  void   evaluate_quadratic_deriv(
    int           n_parameters,
    float         parameters[],
    float         linear[],
    float         square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    float         *cross_terms[],
    float         deriv[] );

public  void  initialize_quadratic_real(
    int              n_parameters,
    Real             *constant_term,
    Real             *linear_terms[],
    Real             *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    Real             **cross_terms[] );

public  void  zero_quadratic_real(
    int              n_parameters,
    Real             *constant_term,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[] );

public  void  add_to_quadratic_cross_term_real(
    int              *n_cross_terms[],
    int              **cross_parms[],
    Real             **cross_terms[],
    int              parm1,
    int              parm2,
    Real             value,
    int              alloc_increment );

public  void  realloc_quadratic_cross_terms_real(
    int              n_parameters,
    int              n_cross_terms[],
    int              **cross_parms[],
    Real             **cross_terms[] );

public  void  delete_quadratic_real(
    int              n_parameters,
    Real             linear_terms[],
    Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    Real             *cross_terms[] );

public  Real   evaluate_quadratic_real(
    int           n_parameters,
    Real          parameters[],
    Real          constant,
    Real          linear[],
    Real          square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    Real          *cross_terms[] );

public  void   evaluate_quadratic_deriv_real(
    int           n_parameters,
    Real          parameters[],
    Real          linear[],
    Real          square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    Real          *cross_terms[],
    Real          deriv[] );

public  void  compute_statistics(
    int      n,
    Real     samples[],
    Real     *min_value,
    Real     *max_value,
    Real     *mean_value,
    Real     *std_dev,
    Real     *median );

public  void  initialize_statistics(
    statistics_struct  *stats,
    Real               median_lower_bound,
    Real               median_upper_bound );

public  void  add_sample_to_statistics(
    statistics_struct  *stats,
    Real               sample );

public  void  restart_statistics_with_narrower_median_range(
    statistics_struct  *stats );

public  void  get_statistics(
    statistics_struct  *stats,
    int                *n_samples,
    Real               *mean,
    Real               *median,
    Real               *median_error,
    Real               *min_value,
    Real               *max_value,
    Real               *std_deviation );

public  void  terminate_statistics(
    statistics_struct  *stats );

public  void  compute_mean_and_variance(
    int   n,
    Real  samples[],
    Real  *mean,
    Real  *variance );

public  Real  compute_two_means_t_statistic(
    int    n1,
    Real   samples1[],
    int    n2,
    Real   samples2[] );

public  void  initialize_cumulative_t_stat(
    t_stat_struct  *stat,
    int            degrees_freedom );

public  void  delete_cumulative_t_stat(
    t_stat_struct  *stat );

public  Real  get_cumulative_t_stat(
    t_stat_struct  *stat,
    Real           t );

#ifdef __cplusplus
}
#endif

#endif
