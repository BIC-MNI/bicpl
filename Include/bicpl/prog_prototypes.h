#ifndef  DEF_prog_prototypes
#define  DEF_prog_prototypes

public  int  get_n_arguments_remaining( void );

public  void  initialize_argument_processing(
    int     argc,
    char    *argv[] );

public  BOOLEAN  get_int_argument(
    int   default_value,
    int   *value );

public  BOOLEAN  get_real_argument(
    Real   default_value,
    Real   *value );

public  BOOLEAN  get_string_argument(
    STRING   default_value,
    STRING   *value );

public  BOOLEAN  get_prefix_argument(
    STRING  prefix );

public  Status  input_globals_file(
    int             n_globals_lookup,
    global_struct   globals_lookup[],
    STRING          filename );

public  Status  get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           variable_name,
    STRING           *value );

public  Status  set_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           variable_name,
    STRING           value_to_set );

public  Status  set_or_get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           input_str,
    STRING           *variable_name,
    STRING           *value_string );

public  void  delete_global_variables(
    int             n_globals_lookup,
    global_struct   globals_lookup[] );

public  void  set_random_seed( int seed );

public  int  get_random_int( int n );

public  Real  get_random_0_to_1( void );

public  void  start_timing( void );

public  void  end_timing(
    STRING   title,
    int      n );
#endif
