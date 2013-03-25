#ifndef  DEF_numeric_prototypes
#define  DEF_numeric_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  void  initialize_amoeba(
    amoeba_struct     *amoeba,
    int               n_parameters,
    VIO_Real              initial_parameters[],
    VIO_Real              parameter_deltas[],
    amoeba_function   function,
    void              *function_data,
    VIO_Real              tolerance );

BICAPI  VIO_Real  get_amoeba_parameters(
    amoeba_struct  *amoeba,
    VIO_Real           parameters[] );

BICAPI  void  terminate_amoeba(
    amoeba_struct  *amoeba );

BICAPI  VIO_BOOL  perform_amoeba(
    amoeba_struct  *amoeba );

BICAPI  VIO_BOOL  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] );

BICAPI  VIO_BOOL  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse );

BICAPI  VIO_Real  gradient_steps_minimize_function(
    int   n_dims,
    VIO_Real  initial_parameters[],
    VIO_Real  initial_step_sizes[],
    VIO_Real  (*func) ( VIO_Real [], void * ),
    void  *func_data,
    int   n_search_dims,
    int   max_iterations,
    VIO_Real  tolerance,
    VIO_Real  solution[] );

BICAPI  void  initialize_histogram(
    histogram_struct  *histogram,
    VIO_Real              delta,
    VIO_Real              offset );

BICAPI  void  delete_histogram(
    histogram_struct  *histogram );

BICAPI  void  add_to_histogram(
    histogram_struct  *histogram,
    VIO_Real              value );

BICAPI  int  get_histogram_counts(
    histogram_struct  *histogram,
    VIO_Real              *counts[],
    VIO_Real              filter_width,
    VIO_Real              *scale_factor,
    VIO_Real              *trans_factor );

BICAPI  void  display_histogram(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size );

BICAPI  void  create_histogram_line(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size,
    VIO_Real              filter_width,
    lines_struct      *lines );

BICAPI  VIO_BOOL  least_squares(
    int     n_points,
    int     n_dims,
    VIO_Real    **points,
    VIO_Real    values[],
    VIO_Real    parameters[] );

BICAPI  void  initialize_linear_least_squares(
    linear_least_squares   *lsq,
    int                    n_parameters );

BICAPI  void  add_to_linear_least_squares(
    linear_least_squares   *lsq,
    VIO_Real                   parameter_coefs[],
    VIO_Real                   constant );

BICAPI  VIO_BOOL  get_linear_least_squares_solution(
    linear_least_squares   *lsq,
    VIO_Real                   solution[] );

BICAPI  void  delete_linear_least_squares(
    linear_least_squares   *lsq );

BICAPI  VIO_BOOL  singular_value_decomposition(
    int    m,
    int    n,
    VIO_Real   **a,
    VIO_Real   w[],
    VIO_Real   **v );

BICAPI  VIO_Real   minimize_lsq(
    int              n_parameters,
    VIO_Real             constant_term,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[],
    VIO_Real             max_step_size,
    int              n_iters,
    VIO_Real             node_values[] );

BICAPI  void  initialize_lsq_terms(
    int              n_parameters,
    VIO_Real             *constant_term,
    VIO_Real             *linear_terms[],
    VIO_Real             *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    VIO_Real             **cross_terms[] );

BICAPI  void  reset_lsq_terms(
    int              n_parameters,
    VIO_Real             *constant_term,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[] );

BICAPI  void  add_to_lsq_terms(
    int              n_parameters,
    VIO_Real             *constant_term,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[],
    int              n_in_list,
    int              list[],
    VIO_Real             weights[],
    VIO_Real             constant,
    int              alloc_increment );

BICAPI  void  realloc_lsq_terms(
    int              n_parameters,
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[] );

BICAPI  void  delete_lsq_terms(
    int              n_parameters,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[] );

BICAPI  VIO_Real   minimize_lsq_float(
    int              n_parameters,
    VIO_Real             constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    VIO_Real             max_step_size,
    int              n_iters,
    VIO_Real             node_values[] );

BICAPI  void  initialize_lsq_terms_float(
    int              n_parameters,
    VIO_Real             *constant_term,
    float            *linear_terms[],
    float            *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

BICAPI  void  reset_lsq_terms_float(
    int              n_parameters,
    VIO_Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

BICAPI  void  add_to_lsq_terms_float(
    int              n_parameters,
    VIO_Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    int              n_in_list,
    int              list[],
    VIO_Real             weights[],
    VIO_Real             constant,
    int              alloc_increment );

BICAPI  void  realloc_lsq_terms_float(
    int              n_parameters,
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

BICAPI  void  delete_lsq_terms_float(
    int              n_parameters,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

BICAPI  void  create_lsq_hypersurface_float(
    STRING           filename,
    int              parm1,
    int              parm2,
    int              x_size,
    int              y_size,
    VIO_Real             x_min,
    VIO_Real             x_max,
    VIO_Real             y_min,
    VIO_Real             y_max,
    VIO_Real             scale,
    int              n_parameters,
    VIO_Real             constant,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[],
    VIO_Real             parameters[] );

BICAPI  VIO_BOOL  numerically_close(
    VIO_Real  n1,
    VIO_Real  n2,
    VIO_Real  threshold_ratio );

BICAPI  VIO_Real  get_good_round_value(
    VIO_Real    value );

BICAPI  VIO_Real  round_to_nearest_multiple(
    VIO_Real    value,
    VIO_Real    multiple_value );

BICAPI  int  solve_quadratic(
    VIO_Real   a,
    VIO_Real   b,
    VIO_Real   c,
    VIO_Real   *solution1,
    VIO_Real   *solution2 );

BICAPI  int solve_cubic(
    VIO_Real   a,
    VIO_Real   b,
    VIO_Real   c,
    VIO_Real   d,
    VIO_Real   s[ 3 ] );

BICAPI  VIO_Real  evaluate_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u );

BICAPI  int  get_roots_of_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u_min,
    VIO_Real    u_max,
    VIO_Real    accuracy,
    VIO_Real    roots[] );

BICAPI  VIO_BOOL  get_range_of_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u_min,
    VIO_Real    u_max,
    VIO_Real    min_val,
    VIO_Real    max_val,
    VIO_Real    accuracy,
    VIO_Real    *u_min_range,
    VIO_Real    *u_max_range );

BICAPI  void  initialize_quadratic(
    int              n_parameters,
    VIO_Real             *constant_term,
    float            *linear_terms[],
    float            *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

BICAPI  void  zero_quadratic(
    int              n_parameters,
    VIO_Real             *constant_term,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

BICAPI  void  add_to_quadratic_cross_term(
    int              *n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[],
    int              parm1,
    int              parm2,
    VIO_Real             value,
    int              alloc_increment );

BICAPI  void  realloc_quadratic_cross_terms(
    int              n_parameters,
    int              n_cross_terms[],
    int              **cross_parms[],
    float            **cross_terms[] );

BICAPI  void  delete_quadratic(
    int              n_parameters,
    float            linear_terms[],
    float            square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    float            *cross_terms[] );

BICAPI  VIO_Real   evaluate_quadratic(
    int           n_parameters,
    float         parameters[],
    VIO_Real          constant,
    float         linear[],
    float         square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    float         *cross_terms[] );

BICAPI  void   evaluate_quadratic_deriv(
    int           n_parameters,
    float         parameters[],
    float         linear[],
    float         square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    float         *cross_terms[],
    float         deriv[] );

BICAPI  void  initialize_quadratic_real(
    int              n_parameters,
    VIO_Real             *constant_term,
    VIO_Real             *linear_terms[],
    VIO_Real             *square_terms[],
    int              *n_cross_terms[],
    int              **cross_parms[],
    VIO_Real             **cross_terms[] );

BICAPI  void  zero_quadratic_real(
    int              n_parameters,
    VIO_Real             *constant_term,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[] );

BICAPI  void  add_to_quadratic_cross_term_real(
    int              *n_cross_terms[],
    int              **cross_parms[],
    VIO_Real             **cross_terms[],
    int              parm1,
    int              parm2,
    VIO_Real             value,
    int              alloc_increment );

BICAPI  void  realloc_quadratic_cross_terms_real(
    int              n_parameters,
    int              n_cross_terms[],
    int              **cross_parms[],
    VIO_Real             **cross_terms[] );

BICAPI  void  delete_quadratic_real(
    int              n_parameters,
    VIO_Real             linear_terms[],
    VIO_Real             square_terms[],
    int              n_cross_terms[],
    int              *cross_parms[],
    VIO_Real             *cross_terms[] );

BICAPI  VIO_Real   evaluate_quadratic_real(
    int           n_parameters,
    VIO_Real          parameters[],
    VIO_Real          constant,
    VIO_Real          linear[],
    VIO_Real          square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    VIO_Real          *cross_terms[] );

BICAPI  void   evaluate_quadratic_deriv_real(
    int           n_parameters,
    VIO_Real          parameters[],
    VIO_Real          linear[],
    VIO_Real          square[],
    int           n_cross_terms[],
    int           *cross_parms[],
    VIO_Real          *cross_terms[],
    VIO_Real          deriv[] );

BICAPI  void  compute_statistics(
    int      n,
    VIO_Real     samples[],
    VIO_Real     *min_value,
    VIO_Real     *max_value,
    VIO_Real     *mean_value,
    VIO_Real     *std_dev,
    VIO_Real     *median );

BICAPI  void  initialize_statistics(
    statistics_struct  *stats,
    VIO_Real               median_lower_bound,
    VIO_Real               median_upper_bound );

BICAPI  void  add_sample_to_statistics(
    statistics_struct  *stats,
    VIO_Real               sample );

BICAPI  void  restart_statistics_with_narrower_median_range(
    statistics_struct  *stats );

BICAPI  void  get_statistics(
    statistics_struct  *stats,
    int                *n_samples,
    VIO_Real               *mean,
    VIO_Real               *median,
    VIO_Real               *median_error,
    VIO_Real               *min_value,
    VIO_Real               *max_value,
    VIO_Real               *std_deviation );

BICAPI  void  terminate_statistics(
    statistics_struct  *stats );

BICAPI  void  compute_mean_and_variance(
    int   n,
    VIO_Real  samples[],
    VIO_Real  *mean,
    VIO_Real  *variance );

BICAPI  VIO_Real  compute_two_means_t_statistic(
    int    n1,
    VIO_Real   samples1[],
    int    n2,
    VIO_Real   samples2[] );

BICAPI  void  initialize_cumulative_t_stat(
    t_stat_struct  *stat,
    int            degrees_freedom );

BICAPI  void  delete_cumulative_t_stat(
    t_stat_struct  *stat );

BICAPI  VIO_Real  get_cumulative_t_stat(
    t_stat_struct  *stat,
    VIO_Real           t );

#ifdef __cplusplus
}
#endif

#endif
