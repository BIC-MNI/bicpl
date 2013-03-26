#include  <volume_io/internal_volume_io.h>

#define  GLOBALS_LOOKUP_NAME  globals_list
#include  <globals.h>

private  void  print_all_globals( void );

main()
{
    Status           status;
    STRING           line, variable_name, value;
    static   STRING  filename = "globals.input";

    Test_colour = make_Colour( 0, 255, 0 );

    print( "Initial global values:\n\n" );
    print_all_globals();

    status = input_globals_file( VIO_SIZEOF_STATIC_ARRAY(globals_list),
                                 globals_list, filename );

    print( "\n\nAfter reading %s:\n\n", filename );
    print_all_globals();

    do
    {
        print( "\nNow enter either the global name or an assignment\n" );
        print( "   e.g., Test_int\n" );
        print( "   or    Test_int = 3:  " );

        status = input_line( stdin, &line );

        if( status == VIO_OK )
        {
            status = set_or_get_global_variable(
                         VIO_SIZEOF_STATIC_ARRAY(globals_list),
                         globals_list, line, &variable_name, &value );

            if( status == VIO_OK )
                print( "%s has the value: %s\n", variable_name, value );

            delete_string( variable_name );
            delete_string( value );
        }

        delete_string( line );
    }
    while( status == VIO_OK );

    return( status != VIO_OK );
}

private  void  print_all_globals( void )
{
    print( "  Test_int %d\n", Test_int );
    print( "  Test_colour %g %g %g\n",
                          get_Colour_r_0_1(Test_colour),
                          get_Colour_g_0_1(Test_colour),
                          get_Colour_b_0_1(Test_colour) );
    print( "  Test_real %g\n", Test_real );
    print( "  Test_boolean %d\n", Test_boolean );
    print( "  Test_point %g %g %g\n", Point_x(Test_point),
                                               Point_y(Test_point),
                                               Point_z(Test_point) );
    print( "  Test_vector %g %g %g\n", Vector_x(Test_vector),
                                                Vector_y(Test_vector),
                                                Vector_z(Test_vector) );
    print( "  Test_string %s\n", Test_string );
    print( "  Test_surfprop %g %g %g %g %g\n",
                        Surfprop_a(Test_surfprop),
                        Surfprop_d(Test_surfprop),
                        Surfprop_s(Test_surfprop),
                        Surfprop_se(Test_surfprop),
                        Surfprop_t(Test_surfprop) );
}
