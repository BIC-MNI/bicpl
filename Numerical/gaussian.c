#include  <internal_volume_io.h>
#include  <bicpl.h>

public  BOOLEAN  solve_linear_system(
    int   n,
    Real  **coefs,
    Real  values[],
    Real  solution[] )
{
    int       i, j, k, p;
    Real      **a, *s, val, best_val, m, sum, *tmp;
    BOOLEAN   success;

    ALLOC( a, n );
    for_less( i, 0, n )
        ALLOC( a[i], n+1 );

    ALLOC( s, n );

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            a[i][j] = coefs[i][j];
        a[i][n] = values[i];
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
        for_less( j, i, n )
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
        }

        for_less( j, i+1, n )
        {
            m = a[j][i] / a[i][i];
            for_less( k, i, n+1 )
                a[j][k] -= m * a[i][k];
        }
    }

    if( success && a[n-1][n-1] == 0.0 )
        success = FALSE;

    for( i = n-1;  i >= 0;  --i )
    {
        sum = 0.0;
        for_less( j, i+1, n )
            sum += a[i][j] * solution[j];
        solution[i] = (a[i][n] - sum) / a[i][i];
    }

    for_less( i, 0, n )
        FREE( a[i] );
    FREE( a );

    FREE( s );

    return( success );
}
