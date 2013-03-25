#ifndef  DEF_prog_prototypes
#define  DEF_prog_prototypes

public  int  get_n_arguments_remaining( void );

public  void  initialize_argument_processing(
    int     argc,
    char    *argv[] );

public  VIO_BOOL  get_int_argument(
    int   default_value,
    int   *value );

public  VIO_BOOL  get_real_argument(
    VIO_Real   default_value,
    VIO_Real   *value );

public  VIO_BOOL  get_string_argument(
    char   default_value[],
    char   *value[] );

public  VIO_BOOL  get_prefix_argument(
    char  prefix[] );

public  Status  input_globals_file(
    int             n_globals_lookup,
    global_struct   globals_lookup[],
    char            filename[] );

public  Status  get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    char             variable_name[],
    char             value[] );

public  Status  set_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    char             variable_name[],
    char             value_to_set[] );

public  Status  set_or_get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    char             input_str[],
    char             variable_name[],
    char             value_string[] );

public  void  set_random_seed( int seed );

public  int  get_random_int( int n );

public  VIO_Real  get_random_0_to_1( void );

public  void  start_timing();

public  void  end_timing(
    char   title[],
    int    n );
#endif
