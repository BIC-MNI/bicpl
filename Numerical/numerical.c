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

private  Boolean  range_may_contain_zero(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max )
{
    int     i;
    Boolean opposite_signs;
    Real    min_value, max_value, u_min_power, u_max_power;
    Real    min_power, max_power;

    opposite_signs = (u_min * u_max < 0.0);

    min_value = poly[0];
    max_value = poly[0];
    u_min_power = 1.0;
    u_max_power = 1.0;

    for_less( i, 1, n )
    {
        u_min_power *= u_min;
        u_max_power *= u_max;
        if( i % 2 == 0 && opposite_signs )
        {
            if( poly[i] >= 0.0 )
            {
                min_power = 0.0;
                max_power = poly[i] * MAX( u_min_power, u_max_power );
            }
            else
            {
                min_power = poly[i] * MAX( u_min_power, u_max_power );
                max_power = 0.0;
            }
        }
        else
        {
            if( poly[i] >= 0.0 )
            {
                min_power = poly[i] * MIN( u_min_power, u_max_power );
                max_power = poly[i] * MAX( u_min_power, u_max_power );
            }
            else
            {
                min_power = poly[i] * MAX( u_min_power, u_max_power );
                max_power = poly[i] * MIN( u_min_power, u_max_power );
            }
        }

        min_value += min_power;
        max_value += max_power;
    }

    return( min_value <= 0.0 && max_value >= 0.0 );
}

public  int  get_roots_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    accuracy,
    Real    roots[] )
{
    Boolean              done, may_contain_zero, subdivide;
    int                  n_roots, i, which, n_inside;
    Real                 u_mid, diff, min_diff, cubic[4];
    STACK_STRUCT( Real ) next_u_max;

    if( n <= 4 )
    {
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
            if( u_min <= roots[i] && roots[i] >= roots[i] )
            {
                roots[n_inside] = roots[i];
                ++n_inside;
            }
        }
        return( n_inside );
    }

    INITIALIZE_STACK( next_u_max );

    PUSH_STACK( next_u_max, u_max );

    done = FALSE;
    n_roots = 0;

    while( !done )
    {
        u_mid = (u_min + u_max) / 2.0;
        subdivide = (u_max - u_min > accuracy);
        may_contain_zero = range_may_contain_zero( n, poly, u_min, u_max );

        if( may_contain_zero && subdivide )
        {
            PUSH_STACK( next_u_max, u_max );
            u_max = u_mid;
        }
        else
        {
            if( may_contain_zero && (n_roots == 0 || roots[n_roots-1] != u_mid))
            {
                if( n_roots == n-1 )
                {
                    which = 0;
                    min_diff = 0.0;
                    for_less( i, 0, n_roots-1 )
                    {
                        diff = roots[i+1] - roots[i];
                        if( which == 0 || diff < min_diff )
                        {
                            min_diff = diff;
                            which = i;
                        }
                    }

                    for_less( i, which+1, n_roots-1 )
                        roots[i] = roots[i+1];
                    --n_roots;
                }
                roots[n_roots] = u_mid;
                ++n_roots;
            }

            if( IS_STACK_EMPTY( next_u_max ) )
                done = TRUE;
            else
            {
                u_min = u_max;
                POP_STACK( next_u_max, u_max );
            }
        }
    }

    DELETE_STACK( next_u_max );

    return( n_roots );
}
