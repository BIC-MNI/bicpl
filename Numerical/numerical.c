#include  <internal_volume_io.h>
#include  <numerical.h>

/* ----------------------------- MNI Header -----------------------------------
@NAME       : numerically_close
@INPUT      : n1
              n2
              threshold_ratio
@OUTPUT     : 
@RETURNS    : TRUE if the numbers are within the threshold ratio
@DESCRIPTION: Decides if two numbers are close to each other.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  BOOLEAN  numerically_close(
    Real  n1,
    Real  n2,
    Real  threshold_ratio )
{
    double  avg, diff;

    diff = n1 - n2;
    if( diff < 0.0 )  diff = -diff;

    if( n1 <= threshold_ratio && n1 >= -threshold_ratio &&
        n2 <= threshold_ratio && n2 >= -threshold_ratio )
    {
        return( diff <= threshold_ratio );
    }

    avg = (n1 + n2) / 2.0;

    if( avg == 0.0 )
        return( diff <= (double) threshold_ratio );

    if( avg < 0.0 )  avg = -avg;

    return( (diff / avg) <= (double) threshold_ratio );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_good_round_value
@INPUT      : value
@OUTPUT     : 
@RETURNS    : a similar value
@DESCRIPTION: Finds an approximation to the value that has fewer digits.
@METHOD     : Finds the closest power of 10 or 5 times a power of ten less than
              the value.
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

public  Real  round_to_nearest_multiple(
    Real    value,
    Real    multiple_value )
{
    int      i;

    multiple_value = ABS( multiple_value );

    i = ROUND( value / multiple_value );

    return( (Real) i * multiple_value );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : solve_quadratic
@INPUT      : a
              b
              c
@OUTPUT     : solution1
              solution2
@RETURNS    : number of solutions 0, 1, or 2
@DESCRIPTION: Solves a quadratic equation for its zeroes.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_roots_of_cubic
@INPUT      : n    - degrees  (4 for cubic, 3 for quadratic, 2 for linear)
              poly - coefficients of polynomial
              u_min
              u_max
@OUTPUT     : roots
@RETURNS    : number of roots
@DESCRIPTION: Finds the roots of the up-to-cubic polynomial within the
              range [u_min,u_max]
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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
        if( u_min > u_max || u_min <= roots[i] && roots[i] <= u_max )
        {
            roots[n_inside] = roots[i];
            ++n_inside;
        }
    }

    return( n_inside );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : evaluate_polynomial
@INPUT      : n     - n coefficients
              poly  - coefficients
              u
@OUTPUT     : 
@RETURNS    : value
@DESCRIPTION: Evaluates the n-th order polynomial at u.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  Real  evaluate_polynomial(
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : interval_value_range
@INPUT      : n
              poly
              u_min
              u_max
@OUTPUT     : min_val_ptr
              max_val_ptr
@RETURNS    : 
@DESCRIPTION: Finds the min and max value of the given interval.  Actually
              the range may be a subset of the returned values, but the
              main thing is that all values of the polynomial within u_min
              and u_max are guaranteed to be within the values
              min_val_ptr and max_val_ptr.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  void  interval_value_range(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    *min_val_ptr,
    Real    *max_val_ptr )
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

    *min_val_ptr = min_val;
    *max_val_ptr = max_val;
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : interval_may_contain_zero
@INPUT      : n
              poly
              u_min
              u_max
@OUTPUT     : 
@RETURNS    : TRUE if the interval may contain zero
@DESCRIPTION: Checks if the polynomial may have a zero value within the
              given u range.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  BOOLEAN  interval_may_contain_zero(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max )
{
    Real   min_val, max_val;

    interval_value_range( n, poly, u_min, u_max, &min_val, &max_val );

    return( min_val <= 0.0 && max_val >= 0.0 );
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_interval
@INPUT      : n
              poly
              u_min
              u_max
              accuracy
@OUTPUT     : n_roots
              roots
@RETURNS    : 
@DESCRIPTION: Recursively checks intervals on the polynomial for zeros.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

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

    if( interval_may_contain_zero( n, poly, u_min, u_max ) )
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : get_roots_of_polynomial
@INPUT      : n
              poly
              u_min
              u_max
              accuracy
@OUTPUT     : roots
@RETURNS    : # roots
@DESCRIPTION: Finds the roots of the polynomial on the given range.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

public  int  get_roots_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
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

/* ----------------------------- MNI Header -----------------------------------
@NAME       : polynomial_may_include_range
@INPUT      : n
              poly
              u_min
              u_max
              min_val
              max_val
@OUTPUT     : 
@RETURNS    : TRUE if polynomial may contain range
@DESCRIPTION: Checks if the polynomial may evaluate to a value in the given
              range in the given parameter interval.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : 1993            David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */

private  BOOLEAN  polynomial_may_include_range(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    min_val,
    Real    max_val )
{
    Real  min_range, max_range;

    interval_value_range( n, poly, u_min, u_max, &min_range, &max_range );

    return( max_val >= min_range && min_val <= max_range );
}

private  BOOLEAN  check_range(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    min_val,
    Real    max_val,
    Real    accuracy,
    Real    *u_min_range,
    Real    *u_max_range )
{
    Real     u_mid;
    BOOLEAN  left_has_value, right_has_value;

    if( u_max - u_min <= accuracy )
    {
        *u_min_range = u_min;
        *u_max_range = u_max;
        return( TRUE );
    }

    u_mid = (u_min + u_max) / 2.0;

    left_has_value = polynomial_may_include_range( n, poly, u_min, u_mid,
                                                   min_val, max_val );
    right_has_value = polynomial_may_include_range( n, poly, u_mid, u_max,
                                                    min_val, max_val );

    if( left_has_value && right_has_value )
    {
        *u_min_range = u_min;
        *u_max_range = u_max;
        return( TRUE );
    }
    else if( left_has_value && !right_has_value )
    {
        return( check_range( n, poly, u_min, u_mid, min_val, max_val,
                             accuracy, u_min_range, u_max_range ) );
    }
    else if( !left_has_value && right_has_value )
    {
        return( check_range( n, poly, u_mid, u_max, min_val, max_val,
                             accuracy, u_min_range, u_max_range ) );
    }
    else
        return( FALSE );
}

public  BOOLEAN  get_range_of_polynomial(
    int     n,
    Real    poly[],
    Real    u_min,
    Real    u_max,
    Real    min_val,
    Real    max_val,
    Real    accuracy,
    Real    *u_min_range,
    Real    *u_max_range )
{
    BOOLEAN  found;

    if( polynomial_may_include_range( n, poly, u_min, u_max, min_val, max_val ))
    {
        found = check_range( n, poly, u_min, u_max, min_val, max_val, accuracy,
                             u_min_range, u_max_range );
    }
    else
        found = FALSE;

    return( found );
}
