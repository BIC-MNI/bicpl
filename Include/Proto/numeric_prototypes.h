#ifndef  DEF_numeric_prototypes
#define  DEF_numeric_prototypes

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

public  void  least_squares(
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

public  void  compute_statistics(
    int      n,
    Real     samples[],
    Real     *min_value,
    Real     *max_value,
    Real     *mean_value,
    Real     *std_dev,
    Real     *median );
#endif
