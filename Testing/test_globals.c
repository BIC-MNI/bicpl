#include  <def_mni.h>
#include  <def_globals.h>

#undef   DEF_GLOBALS
#define  INSTANTIATE_GLOBALS
#include  <def_globals.h>
#undef   INSTANTIATE_GLOBALS

#undef   DEF_GLOBALS
#define  GLOBALS_LOOKUP   globals_lookup
#include  <def_globals.h>
#undef   GLOBALS_LOOKUP

private  void  print_all_globals( void );

main()
{
    Status         status;
    String         line, variable_name, value;
    static   char  filename[] = "globals.input";

    PRINT1( "Initial global values:\n\n" );
    print_all_globals();

    status = input_globals_file( SIZEOF_STATIC_ARRAY(globals_lookup),
                                 globals_lookup, filename );

    PRINT2( "\n\nAfter reading %s:\n\n", filename );
    print_all_globals();

    do
    {
        PRINT1( "\nNow enter either the global name or an assignment\n" );
        PRINT1( "   e.g., Test_int\n" );
        PRINT1( "   or    Test_int = 3:" );

        status = input_line( stdin, line, MAX_STRING_LENGTH );

        if( status == OK )
        {
            status = set_or_get_global_variable(
                             SIZEOF_STATIC_ARRAY(globals_lookup),
                             globals_lookup, line,
                             variable_name,
                             value );
        }

        if( status == OK )
            PRINT3( "%s: %s\n", variable_name, value );
    }
    while( status == OK );

    return( status != OK );
}

private  void  print_all_globals( void )
{
    PRINT2( "  Test_int %d\n", Test_int );
    PRINT4( "  Test_colour %g %g %g\n",
                          get_Colour_r_0_1(Test_colour),
                          get_Colour_g_0_1(Test_colour),
                          get_Colour_b_0_1(Test_colour) );
    PRINT2( "  Test_real %g\n", Test_real );
    PRINT2( "  Test_boolean %d\n", Test_boolean );
    PRINT4( "  Test_point %g %g %g\n", Point_x(Test_point),
                                               Point_y(Test_point),
                                               Point_z(Test_point) );
    PRINT4( "  Test_vector %g %g %g\n", Vector_x(Test_vector),
                                                Vector_y(Test_vector),
                                                Vector_z(Test_vector) );
    PRINT2( "  Test_string %s\n", Test_string );
    PRINT6( "  Test_surfprop %g %g %g %g %g\n",
                        Surfprop_a(Test_surfprop),
                        Surfprop_d(Test_surfprop),
                        Surfprop_s(Test_surfprop),
                        Surfprop_se(Test_surfprop),
                        Surfprop_t(Test_surfprop) );
}
