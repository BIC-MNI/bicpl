#include  <internal_volume_io.h>
#include  <bicpl.h>

private  void  multiply_matrices(
    int    x1,
    int    y1,
    Real   m1[],
    int    sa1,
    int    sb1,
    int    y2,
    Real   m2[],
    int    sa2,
    int    sb2,
    Real   prod[],
    int    sap,
    int    sbp )
{
    int   i, j, k;
    Real  sum, *m1_ptr, *m2_ptr, *prod_ptr;

    for_less( i, 0, x1 )
    {
        prod_ptr = &prod[i*sap];
        for_less( j, 0, y2 )
        {
            sum = 0.0;
            m1_ptr = &m1[i*sa1];
            m2_ptr = &m2[j*sb2];
            for_less( k, 0, y1 )
            {
                sum += (*m1_ptr) * (*m2_ptr);
                m1_ptr += sb1;
                m2_ptr += sa2;
            }
            *prod_ptr = sum;
            prod_ptr += sbp;
        }
    }
}

public  void  spline_tensor_product(
    int     n_dims,
    Real    positions[],
    int     degrees[],     /* [n_dims] */
    Real    *bases[],      /* [n_dims][degress[dim]*degrees[dim]] */
    int     n_values,
    Real    coefs[],       /* [n_values*degrees[0]*degrees[1]*...] */
    int     n_derivs[],    /* [n_dims] */
    Real    results[] )    /* [n_values*n_derivs[0]*n_derivs[1]*...] */
{
    int     i, deriv, d, k, total_values, src;
    Real    **us, **weights, *tmp_results[2], *r;

    /*--- check arguments */

    total_values = n_values;
    for_less( d, 0, n_dims )
        total_values *= degrees[d];

    ALLOC( tmp_results[0], total_values );
    ALLOC( tmp_results[1], total_values );

    for_less( i, 0, total_values )
        tmp_results[0][i] = coefs[i];

    src = 0;

    /*--- do each dimension */

    for_less( d, 0, n_dims )
    {
        ALLOC2D( us, 1 + n_derivs[d], degrees[d] );
        ALLOC2D( weights, 1 + n_derivs[d], degrees[d] );

        us[0][0] = 1.0;
        for_less( k, 1, degrees[d] )
            us[0][k] = us[0][k-1] * positions[d];

        for_inclusive( deriv, 1, n_derivs[d] )
        {
            for_less( k, 0, deriv )
                us[deriv][k] = 0.0;
   
            for_less( k, deriv, degrees[d] )
                us[deriv][k] = us[deriv-1][k-1] * (Real) k;
        }

        multiply_matrices( 1 + n_derivs[d], degrees[d], &us[0][0],
                           degrees[d], 1,
                           degrees[d], bases[d], degrees[d], 1,
                           &weights[0][0], degrees[d], 1 );

        total_values = n_values;
        for_less( i, 0, d )
            total_values *= 1 + n_derivs[i];
        for_less( i, d+1, n_dims )
            total_values *= degrees[i];

        if( d == n_dims-1 )
            r = results;
        else
            r = tmp_results[1-src];

        multiply_matrices( 1 + n_derivs[d], degrees[d], &weights[0][0],
                           degrees[d], 1,
                           total_values, tmp_results[src], total_values, 1,
                           r, 1, 1 + n_derivs[d] );

        FREE2D( us );
        FREE2D( weights );

        src = 1 - src;
    }

    FREE( tmp_results[0] );
    FREE( tmp_results[1] );
}
