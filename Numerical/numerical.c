#include  <def_mni.h>
#include  <def_stack.h>

public  Boolean  numerically_close(
    Real  n1,
    Real  n2,
    Real  threshold_ratio )
{
    double  avg, diff;

    diff = n1 - n2;
    if( diff < 0.0 )  diff = -diff;

    if( n1 <= 0.0 && n2 >= 0.0 ||
        n1 >= 0.0 && n2 <= 0.0 )
    {
        return( diff <= threshold_ratio );
    }

    avg = (n1 + n2) / 2.0;

    if( avg < 0.0 )  avg = -avg;

    return( (diff / avg) <= (double) threshold_ratio );
}

public  Real  get_good_round_value(
    Real    value )
{
    Real     rounded, sign, power_of_ten, power_of_ten_times_5;

    if( value < 0.0 )
    {
        sign = -1.0;
        value = -value;
    }
    else
        sign = 1.0;

    if( value == 0.0 )
        rounded = 0.0;
    else
    {
        power_of_ten = pow( 10.0, (double) (int) log10( (double) value ) );
        power_of_ten_times_5 = 5.0 * power_of_ten;
        if( power_of_ten_times_5 > value )
            power_of_ten_times_5 /= 10.0;

        rounded = MAX( power_of_ten, power_of_ten_times_5 );
    }

    return( sign * rounded );
}

public  int  solve_quadratic(
    Real   a,
    Real   b,
    Real   c,
    Real   *solution1,
    Real   *solution2 )
{
    double  inside_sqrt;
    int     n_solutions;

    if( a == 0.0 )
    {
        if( b == 0.0 )
            n_solutions = 0;
        else
        {
            n_solutions = 1;
            *solution1 = -c / b;
        }
    }
    else
    {
        inside_sqrt = (b * b - 4.0 * a * c);

        if( inside_sqrt < 0.0 )
        {
            n_solutions = 0;
        }
        else if( inside_sqrt == 0.0 )
        {
            n_solutions = 1;
            *solution1 = - b / 2.0 / a;
        }
        else
        {
            n_solutions = 2;
            inside_sqrt = sqrt( (double) inside_sqrt );
            *solution1 = (- b - inside_sqrt) / 2.0 / a;
            *solution2 = (- b + inside_sqrt) / 2.0 / a;
        }
    }

    return( n_solutions );
}

/*
 *  Roots3And4.c
 *
 *  Utility functions to find cubic and quartic roots,
 *  coefficients are passed like this:
 *
 *      c[0] + c[1]*x + c[2]*x^2 + c[3]*x^3 + c[4]*x^4 = 0
 *
 *  The functions return the number of non-complex roots and
 *  put the values into the s array.
 *
 *  Author:         Jochen Schwarze (schwarze@isa.de)
 *
 *  Jan 26, 1990    Version for Graphics Gems
 *  Oct 11, 1990    Fixed sign problem for negative q's in SolveQuartic
 *  	    	    (reported by Mark Podlipec),
 *  	    	    Old-style function definitions,
 *  	    	    IsZero() as a macro
 *  Nov 23, 1990    Some systems do not declare acos() and cbrt() in
 *                  <math.h>, though the functions exist in the library.
 *                  If large coefficients are used, EQN_EPS should be
 *                  reduced considerably (e.g. to 1E-30), results will be
 *                  correct but multiple roots might be reported more
 *                  than once.
 */

/* epsilon surrounding for near zero values */

#define     EQN_EPS     1e-9
#define	    IsZero(x)	((x) > -EQN_EPS && (x) < EQN_EPS)

#define     cbrt(x)     ((x) > 0.0 ? pow((double)(x), 1.0/3.0) : \
                        ((x) < 0.0 ? -pow((double)-(x), 1.0/3.0) : 0.0))


public  int solve_cubic(
    Real   a,
    Real   b,
    Real   c,
    Real   d,
    Real   s[ 3 ] )
{
    int     i, num;
    double  sub;
    double  A, B, C;
    double  sq_A, p, q;
    double  cb_p, D;

    if( IsZero(a) )
    {
        return( solve_quadratic( b, c, d, &s[0], &s[1] ) );
    }

    /* normal form: x^3 + Ax^2 + Bx + C = 0 */

    A = b / a;
    B = c / a;
    C = d / a;

    /*  substitute x = y - A/3 to eliminate quadric term:
	x^3 +px + q = 0 */

    sq_A = A * A;
    p = 1.0/3 * (- 1.0/3 * sq_A + B);
    q = 1.0/2 * (2.0/27 * A * sq_A - 1.0/3 * A * B + C);

    /* use Cardano's formula */

    cb_p = p * p * p;
    D = q * q + cb_p;

    if (IsZero(D))
    {
	if (IsZero(q)) /* one triple solution */
	{
	    s[ 0 ] = 0;
	    num = 1;
	}
	else /* one single and one double solution */
	{
	    double u = cbrt(-q);
	    s[ 0 ] = 2 * u;
	    s[ 1 ] = - u;
	    num = 2;
	}
    }
    else if (D < 0) /* Casus irreducibilis: three real solutions */
    {
	double phi = 1.0/3 * acos(-q / sqrt(-cb_p));
	double t = 2 * sqrt(-p);

	s[ 0 ] =   t * cos(phi);
	s[ 1 ] = - t * cos(phi + M_PI / 3);
	s[ 2 ] = - t * cos(phi - M_PI / 3);
	num = 3;
    }
    else /* one real solution */
    {
	double sqrt_D = sqrt(D);
	double u = cbrt(sqrt_D - q);
	double v = - cbrt(sqrt_D + q);

	s[ 0 ] = u + v;
	num = 1;
    }

    /* resubstitute */

    sub = 1.0/3 * A;

    for (i = 0; i < num; ++i)
	s[ i ] -= sub;

    return num;
}

private  Real  evaluate_polynomial(
    int     n,
    Real    poly[],
    Real    u )
{
    int     i;
    Real    val;

    val = 0.0;

    for( i = n-1;  i >= 0;  --i )
        val = val * u + poly[i];

    return( val );
}

private  int  get_roots_of_cubic(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    roots[] )
{
    int      n_roots, i, n_inside;
    Real     cubic[4];

    for_less( i, 0, 4 )
    {
        if( i >= n )
            cubic[i] = 0.0;
        else
            cubic[i] = poly[i];
    }

    n_roots = solve_cubic( cubic[3], cubic[2], cubic[1], cubic[0], roots );
    n_inside = 0;
    for_less( i, 0, n_roots )
    {
        if( u_min <= roots[i] && roots[i] <= u_max )
        {
            roots[n_inside] = roots[i];
            ++n_inside;
        }
    }

    return( n_inside );
}

#ifdef OLD
private  void  check_interval(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    val_min,
    Real    u_max,
    Real    val_max,
    Real    accuracy,
    int     *n_roots,
    Real    roots[] )
{
    int    i, which;
    Real   u_mid, min_diff, diff, val_mid;

    u_mid = (u_max + u_min) / 2.0;

    if( u_max - u_min <= accuracy )
    {
        if( *n_roots == n-1 )
        {
            which = 0;
            min_diff = 0.0;
            for_less( i, 0, *n_roots-1 )
            {
                diff = roots[i+1] - roots[i];
                if( which == 0 || diff < min_diff )
                {
                    min_diff = diff;
                    which = i;
                }
            }

            for_less( i, which+1, *n_roots-1 )
                roots[i] = roots[i+1];
            --(*n_roots);
        }

        roots[*n_roots] = u_mid;
        ++(*n_roots);
    }
    else
    {
        val_mid = evaluate_polynomial( n, poly, u_mid );
        if( val_min * val_mid <= 0.0 )
        {
            check_interval( n, poly, u_min, val_min, u_mid, val_mid,
                            accuracy, n_roots, roots );
        }
        if( val_mid * val_max <= 0.0 )
        {
            check_interval( n, poly, u_mid, val_mid, u_max, val_max,
                            accuracy, n_roots, roots );
        }
    }
}

public  int  get_roots_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    step,
    Real    accuracy,
    Real    roots[] )
{
    Real       u1, u2, val1, val2;
    int        n_roots;

    if( n <= 4 )
        return( get_roots_of_cubic( n, poly, u_min, u_max, roots ) );

    n_roots = 0;

    val2 = evaluate_polynomial( n, poly, u_min );

    for( u1 = u_min;  u1 < u_max;  u1 += step )
    {
        val1 = val2;
        u2 = u1 + step;
        val2 = evaluate_polynomial( n, poly, u2 );

        if( val1 * val2 <= 0.0 )
            check_interval( n, poly, u1, val1, u2, val2, accuracy,
                            &n_roots, roots );
    }

    return( n_roots );
}
#else

private  Boolean  interval_contains_zero(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max )
{
    int    i;
    Real   min_val, max_val, t1, t2, t3, t4;

    min_val = 0.0;
    max_val = 0.0;

    if( u_min >= 0.0 )
    {
        for( i = n-1;  i >= 0;  --i )
        {
            if( i != n-1 )
            {
                if( min_val < 0.0 )
                {
                    min_val *= u_max;
                    if( max_val < 0.0 )
                        max_val *= u_min;
                    else
                        max_val *= u_max;
                }
                else
                {
                    min_val *= u_min;
                    max_val *= u_max;
                }

                min_val += poly[i];
                max_val += poly[i];
            }
            else
            {
                min_val = poly[i];
                max_val = poly[i];
            }
        }
    }
    else
    {
        for( i = n-1;  i >= 0;  --i )
        {
            t1 = min_val * u_min;
            t2 = max_val * u_min;
            t3 = min_val * u_max;
            t4 = max_val * u_max;
            min_val = t1;
            max_val = t1;
            if( t2 < min_val )
                min_val = t2;
            else if( t2 > max_val )
                max_val = t2;
            if( t3 < min_val )
                min_val = t3;
            else if( t3 > max_val )
                max_val = t3;
            if( t4 < min_val )
                min_val = t4;
            else if( t4 > max_val )
                max_val = t4;
            min_val += poly[i];
            max_val += poly[i];
        }
    }

    return( min_val <= 0.0 && max_val >= 0.0 );
}

private  void  check_interval(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    accuracy,
    int     *n_roots,
    Real    roots[] )
{
    int    i, which;
    Real   u_mid, min_diff, diff;

    if( interval_contains_zero( n, poly, u_min, u_max ) )
    {
        u_mid = (u_max + u_min) / 2.0;
        if( u_max - u_min > accuracy )
        {
            check_interval( n, poly, u_min, u_mid, accuracy, n_roots, roots );
            check_interval( n, poly, u_mid, u_max, accuracy, n_roots, roots );
        }
        else
        {
            if( *n_roots == n-1 )
            {
                which = 0;
                min_diff = 0.0;
                for_less( i, 0, *n_roots-1 )
                {
                    diff = roots[i+1] - roots[i];
                    if( which == 0 || diff < min_diff )
                    {
                        min_diff = diff;
                        which = i;
                    }
                }

                for_less( i, which+1, *n_roots-1 )
                    roots[i] = roots[i+1];
                --(*n_roots);
            }

            roots[*n_roots] = u_mid;
            ++(*n_roots);
        }
    }
}

public  int  get_roots_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    step,
    Real    accuracy,
    Real    roots[] )
{
    int        n_roots;

    if( n <= 4 )
        return( get_roots_of_cubic( n, poly, u_min, u_max, roots ) );

    n_roots = 0;

    check_interval( n, poly, u_min, u_max, accuracy, &n_roots, roots );

    return( n_roots );
}
#endif
