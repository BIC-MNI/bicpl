#ifndef  DEF_prog_prototypes
#define  DEF_prog_prototypes

#ifdef __cplusplus
extern "C" {
#endif

BICAPI  int  get_n_arguments_remaining( void );

BICAPI  void  initialize_argument_processing(
    int     argc,
    char    *argv[] );

BICAPI  VIO_BOOL  get_int_argument(
    int   default_value,
    int   *value );

BICAPI  VIO_BOOL  get_real_argument(
    VIO_Real   default_value,
    VIO_Real   *value );

BICAPI  VIO_BOOL  get_string_argument(
    STRING   default_value,
    STRING   *value );

BICAPI  VIO_BOOL  get_prefix_argument(
    STRING  prefix );

BICAPI  Status  input_globals_file(
    int             n_globals_lookup,
    global_struct   globals_lookup[],
    STRING          filename );

BICAPI  Status  get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           variable_name,
    STRING           *value );

BICAPI  Status  set_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           variable_name,
    STRING           value_to_set );

BICAPI  Status  set_or_get_global_variable(
    int              n_globals_lookup,
    global_struct    globals_lookup[],
    STRING           input_str,
    STRING           *variable_name,
    STRING           *value_string );

BICAPI  void  delete_global_variables(
    int             n_globals_lookup,
    global_struct   globals_lookup[] );

BICAPI  void  set_random_seed( int seed );

BICAPI  int  get_random_int( int n );

BICAPI  VIO_Real  get_random_0_to_1( void );

BICAPI  void  start_timing( void );

BICAPI  void  end_timing(
    STRING   title,
    int      n );

#ifdef __cplusplus
}
#endif

#endif
