#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Real          coefs[] = { 1.0, 2.0, 1.0, 3.0 };
    Real          u, values[3];

    while( input_real( stdin, &u ) == OK )
    {
        evaluate_univariate_catmull_spline( u, 4, coefs, 2, values );
        print( "%g: %g %g %g\n", u, values[0], values[1], values[2] );
    }

    return( 0 );
}
