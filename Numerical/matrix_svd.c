#include <internal_volume_io.h>
#include <numerical.h>

#define  MAX_ITERATIONS  30

private  Real   at,bt,ct;

#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
(ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))

#define TAKE_SIGN( a, b ) ((b) >= 0.0 ? fabs(a) : -fabs(a))

public  BOOLEAN  singular_value_decomposition(
    int    m,
    int    n,
    Real   **a,
    Real   w[],
    Real   **v )
{
    BOOLEAN     flag;
    int         i, iter, j, jj, k, l, nm;
    Real        c, f, h, s, x, y, z;
    Real        anorm, g, scale, t;
    Real        *rv1;

    if( m < n )
    {
        print_error("singular_value_decomposition(): matrix must have zero rows added to it.\n");
        return( FALSE );
    }

    ALLOC( rv1, n+1 );

    anorm = 0.0;
    scale = 0.0;
    g = 0.0;

    for_less( i, 1, n+1 )
    {
        l = i + 1;
        rv1[i] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;

        if( i <= m )
        {
            for_less( k, i, m+1 )
                scale += fabs( a[k][i] );

            if( scale != 0.0 )
            {
                for_less( k, i, m+1 )
                {
                    a[k][i] /= scale;
                    s += a[k][i] * a[k][i];
                }

                f  =a[i][i];
                g = -TAKE_SIGN( sqrt(s), f );
                h = f * g - s;
                a[i][i] = f - g;

                if( i != n )
                {
                    for_less( j, l, n+1 )
                    {
                        s = 0.0;
                        for_less( k, i, m + 1 )
                            s += a[k][i] * a[k][j];
                        f = s / h;
                        for_less( k, i, m + 1 )
                            a[k][j] += f * a[k][i];
                    }
                }

                for_less( k, i, m+1)
                    a[k][i] *= scale;
            }
        }

        w[i] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;
        if( i <= m && i != n )
        {
            for_less( k, l, n+1 )
                scale += fabs( a[i][k] );

            if( scale != 0.0 )
            {
                for_less( k, l, n+1 )
                {
                    a[i][k] /= scale;
                    s += a[i][k] * a[i][k];
                }

                f = a[i][l];
                g = -TAKE_SIGN( sqrt(s), f );
                h = f * g - s;
                a[i][l] = f - g;
                for_less( k, l, n+1 )
                    rv1[k] = a[i][k] / h;
                if( i != m )
                {
                    for_less( j, l, m+1 )
                    {
                        s = 0.0;
                        for_less( k, l, n+1 )
                            s += a[j][k] * a[i][k];

                        for_less( k, l, n+1 )
                            a[j][k] += s * rv1[k];
                    }
                }

                for_less( k, l, n+1 )
                     a[i][k] *= scale;
            }
        }

        t = fabs(w[i] )+ fabs(rv1[i]);

        if( t > anorm )
            anorm = t;
    }

    for_down( i, n, 1 )
    {
        if( i < n )
        {
            if( g != 0.0 )
            {
                for_less( j, l, n+1 )
                    v[j][i] = (a[i][j] / a[i][l]) / g;

                for_less( j, l, n+1 )
                {
                    s = 0.0;
                    for_less( k, l, n+1 )
                        s += a[i][k] * v[k][j];

                    for_less( k, l, n+1 )
                        v[k][j] += s * v[k][i];
                }
            }

            for_less( j, l, n+1 )
            {
                v[i][j] = 0.0;
                v[j][i] = 0.0;
            }
        }

        v[i][i] = 1.0;
        g = rv1[i];
        l = i;
    }

    for_down( i, n, 1 )
    {
        l = i + 1;
        g = w[i];
        if( i < n )
        {
            for_less( j, l, n+1 )
                a[i][j] = 0.0;
        }

        if( g != 0.0 )
        {
            g = 1.0 / g;
            if( i != n )
            {
                for_less( j, l, n+1 )
                {
                    s = 0.0;
                    for_less( k, l, m+1 )
                        s += a[k][i] * a[k][j];
                    f = (s / a[i][i]) * g;
                    for_less( k, i, m+1 )
                        a[k][j] += f * a[k][i];
                }
            }

            for_less( j, i, m+1 )
                a[j][i] *= g;
        }
        else
        {
            for_less( j, i, m + 1 )
                a[j][i] = 0.0;
        }

        a[i][i] += 1.0;
    }

    for_down( k, n, 1 )
    {
        for_less( iter, 0, MAX_ITERATIONS )
        {
            flag = TRUE;
            for_down( l, k, 1 )
            {
                nm = l - 1;
                if( fabs(rv1[l]) + anorm == anorm )
                {
                    flag = FALSE;
                    break;
                }

                if( fabs(w[nm]) + anorm == anorm)
                    break;
            }

            if( flag )
            {
                c = 0.0;
                s = 1.0;

                for_less( i, l, k+1 )
                {
                    f = s * rv1[i];

                    if( fabs(f)+anorm != anorm )
                    {
                        g = w[i];
                        h = PYTHAG( f, g );
                        w[i] = h;
                        h = 1.0 / h;
                        c = g * h;
                        s = -f * h;
                        for_less( j, 1, m+1 )
                        {
                            y = a[j][nm];
                            z = a[j][i];
                            a[j][nm] = y*c + z*s;
                            a[j][i] = z*c - y*s;
                        }
                    }
                }
            }

            z = w[k];
            if( l == k )
            {
                if( z < 0.0 )
                {
                    w[k] = -z;
                    for_less( j, 1, n+1 )
                        v[j][k] = -v[j][k];
                }
                break;
            }

            if( iter == MAX_ITERATIONS-1 )
            {
                print_error( "No convergence in %d SVD iterations.\n",
                             MAX_ITERATIONS );
                FREE( rv1 );
                return( FALSE );
            }

            x = w[l];
            nm = k - 1;
            y = w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y-z)*(y+z)+(g-h)*(g+h)) / (2.0*h*y);
            g = PYTHAG( f, 1.0 );
            f = ((x-z)*(x+z)+h*((y/(f+TAKE_SIGN(g,f)))-h)) / x;
            c =1.0;
            s =1.0;

            for_less( j, l, nm+1 )
            {
                i = j + 1;
                g = rv1[i];
                y = w[i];
                h = s * g;
                g = c * g;
                z = PYTHAG( f, h );
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x*c + g*s;
                g = g*c - x*s;
                h = y * s;
                y = y * c;
                for_less( jj, 1, n+1 )
                {
                    x = v[jj][j];
                    z = v[jj][i];
                    v[jj][j] = x*c + z*s;
                    v[jj][i] = z*c - x*s;
                }

                z = PYTHAG( f, h );
                w[j] = z;

                if( z )
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }

                f = c*g + s*y;
                x = c*y - s*g;

                for_less( jj, 1, m+1 )
                {
                    y = a[jj][j];
                    z = a[jj][i];
                    a[jj][j] = y*c + z*s;
                    a[jj][i] = z*c - y*s;
                }
            }

            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = x;
        }
    }

    FREE( rv1 );

    return( TRUE );
}
