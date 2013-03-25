#ifndef  DEF_numeric_prototypes
#define  DEF_numeric_prototypes

public  void  initialize_amoeba(
    amoeba_struct     *amoeba,
    int               n_parameters,
    VIO_Real              initial_parameters[],
    VIO_Real              parameter_deltas[],
    amoeba_function   function,
    void              *function_data,
    VIO_Real              tolerance );

public  VIO_Real  get_amoeba_parameters(
    amoeba_struct  *amoeba,
    VIO_Real           parameters[] );

public  void  terminate_amoeba(
    amoeba_struct  *amoeba );

public  VIO_BOOL  perform_amoeba(
    amoeba_struct  *amoeba );

public  VIO_BOOL  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] );

public  VIO_BOOL  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse );

public  void  initialize_histogram(
    histogram_struct  *histogram,
    VIO_Real              delta,
    VIO_Real              offset );

public  void  delete_histogram(
    histogram_struct  *histogram );

public  void  add_to_histogram(
    histogram_struct  *histogram,
    VIO_Real              value );

public  int  get_histogram_counts(
    histogram_struct  *histogram,
    VIO_Real              *counts[],
    VIO_Real              filter_width,
    VIO_Real              *scale_factor,
    VIO_Real              *trans_factor );

public  void  display_histogram(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size );

public  void  create_histogram_line(
    histogram_struct  *histogram,
    int               x_size,
    int               y_size,
    VIO_Real              filter_width,
    lines_struct      *lines );

public  void  least_squares(
    int     n_points,
    int     n_dims,
    VIO_Real    **points,
    VIO_Real    values[],
    VIO_Real    parameters[] );

public  void  initialize_linear_least_squares(
    linear_least_squares   *lsq,
    int                    n_parameters );

public  void  add_to_linear_least_squares(
    linear_least_squares   *lsq,
    VIO_Real                   parameter_coefs[],
    VIO_Real                   constant );

public  VIO_BOOL  get_linear_least_squares_solution(
    linear_least_squares   *lsq,
    VIO_Real                   solution[] );

public  void  delete_linear_least_squares(
    linear_least_squares   *lsq );

public  VIO_BOOL  singular_value_decomposition(
    int    m,
    int    n,
    VIO_Real   **a,
    VIO_Real   w[],
    VIO_Real   **v );

public  VIO_BOOL  numerically_close(
    VIO_Real  n1,
    VIO_Real  n2,
    VIO_Real  threshold_ratio );

public  VIO_Real  get_good_round_value(
    VIO_Real    value );

public  VIO_Real  round_to_nearest_multiple(
    VIO_Real    value,
    VIO_Real    multiple_value );

public  int  solve_quadratic(
    VIO_Real   a,
    VIO_Real   b,
    VIO_Real   c,
    VIO_Real   *solution1,
    VIO_Real   *solution2 );

public  int solve_cubic(
    VIO_Real   a,
    VIO_Real   b,
    VIO_Real   c,
    VIO_Real   d,
    VIO_Real   s[ 3 ] );

public  VIO_Real  evaluate_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u );

public  int  get_roots_of_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u_min,
    VIO_Real    u_max,
    VIO_Real    accuracy,
    VIO_Real    roots[] );

public  VIO_BOOL  get_range_of_polynomial(
    int     n,
    VIO_Real    poly[],
    VIO_Real    u_min,
    VIO_Real    u_max,
    VIO_Real    min_val,
    VIO_Real    max_val,
    VIO_Real    accuracy,
    VIO_Real    *u_min_range,
    VIO_Real    *u_max_range );

public  void  compute_statistics(
    int      n,
    VIO_Real     samples[],
    VIO_Real     *min_value,
    VIO_Real     *max_value,
    VIO_Real     *mean_value,
    VIO_Real     *std_dev,
    VIO_Real     *median );
#endif
