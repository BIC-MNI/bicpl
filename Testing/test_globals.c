#include <volume_io.h>
#include <bicpl.h>

#define  GLOBALS_LOOKUP_NAME  globals_list
#include  <globals.h>

static  void  print_all_globals( void )
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

int test_case(const char *input_string, 
			  const char *expected_var_name,
			  const char *before_var_value,
			  const char *after_var_value)
{
	VIO_Status status;
	VIO_STR variable_name;
	VIO_STR variable_value;

	status = get_global_variable(VIO_SIZEOF_STATIC_ARRAY(globals_list),
								 globals_list,
								 (VIO_STR)expected_var_name,
								 &variable_value);
	if (status != VIO_OK) 
	{
		fprintf(stderr, "Error %d getting global value.\n", status);
		return status;
	}
	if (strcmp(variable_value, before_var_value) != 0)
	{
		fprintf(stderr, "Value is incorrect (%s).\n", variable_value);
		return VIO_ERROR;
	}
	delete_string(variable_value);

	status = set_or_get_global_variable(VIO_SIZEOF_STATIC_ARRAY(globals_list),
										globals_list, 
										(VIO_STR)input_string,
										&variable_name, 
										&variable_value );

	if ( status != VIO_OK )
	{
		fprintf(stderr, "Error %d setting global variable.\n", status);
		return status;
	}
	if (strcmp(variable_value, after_var_value) != 0)
	{
		fprintf(stderr, "Incorrect variable value (%s).\n", variable_value);
		return VIO_ERROR;
	}
	if (strcmp(variable_name, expected_var_name) != 0)
	{
		fprintf(stderr, "Incorrect variable name (%s).\n", variable_name);
		return VIO_ERROR;
	}
	delete_string(variable_name);
	delete_string(variable_value);
	return VIO_OK;
}
	
int main(int argc, char **argv)
{
    VIO_Status status;
	VIO_STR value;
    static VIO_STR filename = "globals.input";

    Test_colour = make_Colour( 0, 255, 0 );

	status = get_global_variable(VIO_SIZEOF_STATIC_ARRAY(globals_list),
								 globals_list,
								 "Test_int",
								 &value);
	if (status != VIO_OK) 
	{
		fprintf(stderr, "Error getting global value.\n");
		return status;
	}
	if (strcmp(value, "1") != 0)
	{
		fprintf(stderr, "Global value is incorrect.\n");
	}
	delete_string(value);

    status = input_globals_file( VIO_SIZEOF_STATIC_ARRAY(globals_list),
                                 globals_list, filename );

	if (status != VIO_OK)
	{
		return status;
	}
		
	if (test_case("Test_int = 101", "Test_int", "3", "101") != VIO_OK)
	{
		return VIO_ERROR;
	}
	if (Test_int != 101)
	{
		fprintf(stderr, "Variable not properly set (%d).\n", Test_int);
		return VIO_ERROR;
	}

	// Check that directly modifying the global has the expected effect.
	Test_int++;
	if (test_case("Test_int=591A", "Test_int", "102", "591") != VIO_OK)
	{
		return VIO_ERROR;
	}

	if (test_case("  Test_real  =  0.1  ", "Test_real", "3", "0.1") != VIO_OK)
	{
		return VIO_ERROR;
	}
	// Check that directly modifying the global has the expected effect.
	Test_real = 4.9;
	if (test_case("Test_real=10.5", "Test_real", "4.9", "10.5") != VIO_OK)
	{
		return VIO_ERROR;
	}
	
	if (test_case("Test_string = \"Hello, World!\"", "Test_string", "Read from File", "Hello, World!") != VIO_OK)
	{
		return VIO_ERROR;
	}

	Test_string = create_string("Xyzzy");
	
	if (test_case("Test_string = A B C", "Test_string", "Xyzzy", "A B C") != VIO_OK)
	{
		return VIO_ERROR;
	}


	fprintf(stderr, "OK\n");
    return( VIO_OK );
}

