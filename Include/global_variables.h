/* This a sample global variables file, which you should make in your own
   directory, and put it in the -Include path before the include directory
   for this file */

START_GLOBALS
    DEF_GLOBAL( Test_int, int, 1000 )
    DEF_GLOBAL( Test_colour, Colour, 0 )
    DEF_GLOBAL( Test_real, Real, 123.4 )
    DEF_GLOBAL( Test_boolean, Boolean, TRUE )
    DEF_GLOBAL_STRING( Test_string, "Test string" )
    DEF_GLOBAL3( Test_point, Point, 1.0, 2.0, 3.0 )
    DEF_GLOBAL3( Test_vector, Vector, -1.0, -1.0, -1.0 )
    DEF_GLOBAL5( Test_surfprop, Surfprop, 0.3, 0.4, 0.4, 40.0, 1.0 )
END_GLOBALS
