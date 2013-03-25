#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

private  VIO_Real  function(
    VIO_Real   parameters[],
    void   *data )
{
    VIO_Real  value;

    value = parameters[0] * parameters[0] + parameters[1] * parameters[1];

    return( value );
}

int  main(
    int   argc,
    char  *argv[] )
{
    VIO_Real  m, parameters[2], solution[2], steps[2];

    parameters[0] = 1.0;
    parameters[1] = 2.0;
    steps[0] = 0.1;
    steps[1] = 0.1;

    m = gradient_steps_minimize_function( 2, parameters, steps, function, NULL,
                                          2, 0, 1.0e-8, solution );

    print( "%g: %g %g\n", m, solution[0], solution[1] );

    return( 0 );
}
