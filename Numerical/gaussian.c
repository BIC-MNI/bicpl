#ifndef lint
static char rcsid[] = "$Header";
#endif

#include  <internal_volume_io.h>
#include  <numerical.h>

private  BOOLEAN  scaled_maximal_pivoting_gaussian_elimination_float(
    int   n,
    float **coefs,
    int   n_values,
    float **values )
{
    int       i, j, k, p, v;
    float     **a, *s, tmp_real, val, best_val, m, *tmp, scale_factor;
    BOOLEAN   success;

    ALLOC( a, n );
    for_less( i, 0, n )
        ALLOC( a[i], n );

    ALLOC( s, n );

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            a[i][j] = coefs[i][j];
    }

    for_less( i, 0, n )
    {
        s[i] = ABS( a[i][0] );
        for_less( j, 1, n )
        {
            if( ABS(a[i][j]) > s[i] )
               s[i] = ABS(a[i][j]);
        }
    }

    success = TRUE;

    for_less( i, 0, n-1 )
    {
        p = i;
        best_val = a[i][i] / s[i];
        best_val = ABS( best_val );
        for_less( j, i+1, n )
        {
            val = a[j][i] / s[j];
            val = ABS( val );
            if( val > best_val )
            {
                best_val = val;
                p = j;
            }
        }

        if( a[p][i] == 0.0 )
        {
            success = FALSE;
            break;
        }

        if( i != p )
        {
            tmp = a[i];
            a[i] = a[p];
            a[p] = tmp;
            tmp_real = s[i];
            s[i] = s[p];
            s[p] = tmp_real;
        }

        for_less( j, i+1, n )
        {
            m = a[j][i] / a[i][i];
            for_less( k, i+1, n )
                a[j][k] -= m * a[i][k];
            for_less( v, 0, n_values )
                values[v][j] -= m * values[v][i];
        }
    }

    if( success && a[n-1][n-1] == 0.0 )
        success = FALSE;

    if( success )
    {
        for( i = n-1;  i >= 0;  --i )
        {
            for_less( j, i+1, n )
            {
                scale_factor = a[i][j];
                for_less( v, 0, n_values )
                    values[v][i] -= scale_factor * values[v][j];
            }

            for_less( v, 0, n_values )
                values[v][i] /= a[i][i];
        }
    }

    for_less( i, 0, n )
        FREE( a[i] );
    FREE( a );

    FREE( s );

    return( success );
}

public  BOOLEAN  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] )
{
    int       i;

    for_less( i, 0, n )
        solution[i] = values[i];

    return( scaled_maximal_pivoting_gaussian_elimination_float( n, coefs, 1,
                                                                &solution ) );
}

public  BOOLEAN  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse )
{
    float     tmp;
    BOOLEAN   success;
    int       i, j;

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            inverse[i][j] = 0.0;
        inverse[i][i] = 1.0;
    }

    success = scaled_maximal_pivoting_gaussian_elimination_float( n, matrix,
                                                                  n, inverse );

    if( success )
    {
        for_less( i, 0, n-1 )
        {
            for_less( j, i+1, n )
            {
                tmp = inverse[i][j];
                inverse[i][j] = inverse[j][i];
                inverse[j][i] = tmp;
            }
        }
    }

    return( success );
}
