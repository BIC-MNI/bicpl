#include  <internal_volume_io.h>
#include  <bicpl.h>

private  Real   linear_coefs[2][2] = {
                                           {  1.0,  0.0 },
                                           { -1.0,  1.0 }
                                      };
private  Real   quadratic_coefs[3][3] = {
                                           {  0.5,  0.5,  0.0 },
                                           { -1.0,  1.0,  0.0 },
                                           {  0.5, -1.0,  0.5 }
                                        };

private  Real   cubic_coefs[4][4] = {
                                        {  0.0,  1.0,  0.0,  0.0 },
                                        { -0.5,  0.0,  0.5,  0.0 },
                                        {  1.0, -2.5,  2.0, -0.5 },
                                        { -0.5,  1.5, -1.5,  0.5 }
                                    };

public  void  get_linear_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 2 )
    for_less( j, 0, 2 )
        coefs[i][j] = linear_coefs[i][j];
}

public  void  get_quadratic_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 3 )
    for_less( j, 0, 3 )
        coefs[i][j] = quadratic_coefs[i][j];
}

public  void  get_cubic_spline_coefs(
    Real  **coefs )
{
    int    i, j;

    for_less( i, 0, 4 )
    for_less( j, 0, 4 )
        coefs[i][j] = cubic_coefs[i][j];
}

public  void  evaluate_univariate_catmull_spline(
    Real    u,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    Real   *bases;

    if( degree < 2 || degree > 4 )
    {
        handle_internal_error( "evaluate_univariate_catmull_spline: invalid degree" );
        return;
    }

    switch( degree )
    {
    case 2:   bases = &linear_coefs[0][0];      break;
    case 3:   bases = &quadratic_coefs[0][0];   break;
    case 4:   bases = &cubic_coefs[0][0];       break;
    }

    spline_tensor_product( 1, &u, &degree, &bases,
                           1, coefs, &n_derivs, derivs );
}

public  Real  cubic_interpolate(
    Real   u,
    Real   v0,
    Real   v1,
    Real   v2,
    Real   v3 )
{
    Real   coefs[4], value;

    coefs[0] = v0;
    coefs[1] = v1;
    coefs[2] = v2;
    coefs[3] = v3;

    evaluate_univariate_catmull_spline( u, 4, coefs, 0, &value );

    return( value );
}

public  void  evaluate_bivariate_catmull_spline(
    Real    u,
    Real    v,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    int    d, degrees[2], n_derivs_list[2];
    Real   *bases[2], positions[2];

    if( degree < 2 || degree > 4 )
    {
        handle_internal_error( "evaluate_bivariate_catmull_spline: invalid degree" );
        return;
    }

    switch( degree )
    {
    case 2:   bases[0] = &linear_coefs[0][0];      break;
    case 3:   bases[0] = &quadratic_coefs[0][0];   break;
    case 4:   bases[0] = &cubic_coefs[0][0];       break;
    }

    for_less( d, 1, 2 )
        bases[d] = bases[0];

    positions[0] = u;
    positions[1] = v;

    for_less( d, 0, 2 )
    {
        degrees[d] = degree;
        n_derivs_list[d] = n_derivs;
    }

    spline_tensor_product( 2, positions, degrees, bases, 1, coefs,
                           n_derivs_list, derivs );
}

public  void  evaluate_trivariate_catmull_spline(
    Real    u,
    Real    v,
    Real    w,
    int     degree,
    Real    coefs[],
    int     n_derivs,
    Real    derivs[] )
{
    int    d, degrees[3], n_derivs_list[3];
    Real   *bases[3], positions[3];

    if( degree < 2 || degree > 4 )
    {
        handle_internal_error( "evaluate_trivariate_catmull_spline: invalid degree" );
        return;
    }

    switch( degree )
    {
    case 2:   bases[0] = &linear_coefs[0][0];      break;
    case 3:   bases[0] = &quadratic_coefs[0][0];   break;
    case 4:   bases[0] = &cubic_coefs[0][0];       break;
    }

    for_less( d, 1, 3 )
        bases[d] = bases[0];

    positions[0] = u;
    positions[1] = v;
    positions[2] = w;

    for_less( d, 0, 3 )
    {
        degrees[d] = degree;
        n_derivs_list[d] = n_derivs;
    }

    spline_tensor_product( 3, positions, degrees, bases, 1, coefs,
                           n_derivs_list, derivs );
}
