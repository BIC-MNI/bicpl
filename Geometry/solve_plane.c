
/* ----------------------------- MNI Header -----------------------------------
@NAME       : 
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: 
@METHOD     : 
@GLOBALS    : 
@CALLS      :  
@CREATED    : Sep. 10, 1996    David MacDonald
@MODIFIED   : 
---------------------------------------------------------------------------- */


#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/solve_plane.c,v 1.5 1996-09-13 19:36:50 david Exp $";
#endif

#include  <internal_volume_io.h>
#include  <geom.h>
#include  <trans.h>

public  BOOLEAN  get_interpolation_weights_2d(
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   weights[] )
{
    int   i;
    Real  x_fact, y_fact, constant, n;
    Real  aa, ab, ac, bb, bc, dx, dy;
    Real  acbb, abac, aabb, denom, aabc, abab;

    aa = 0.0;
    ab = 0.0;
    ac = 0.0;
    bb = 0.0;
    bc = 0.0;

    for_less( i, 0, n_points )
    {
        dx = xs[i] - x;
        dy = ys[i] - y;
        aa += dx * dx;
        ab += dx * dy;
        ac += dx;
        bb += dy * dy;
        bc += dy;
    }

    n = (Real) n_points;
    aabb = aa * bb;
    acbb = ac * bb;
    aabc = aa * bc;
    abab = ab * ab;
    abac = ab * ac;

    denom = -aabb * n + ac * acbb + bc * aabc + abab*n - 2.0 * abac * bc;

    if( denom == 0.0 )
        return( FALSE );

    x_fact = (acbb - ab * bc) / denom;
    y_fact = (aabc - abac) / denom;
    constant = (abab - aabb) / denom - x * x_fact - y * y_fact;

    for_less( i, 0, n_points )
        weights[i] = constant + y_fact * ys[i] + x_fact * xs[i];

    return( TRUE );
}

#include <prog_utils.h>
#include <numerical.h>

private  void  test_solution(
    int    dim,
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   x_weights[],
    Real   y_weights[],
    Real   constant )
{
    int        iter, n_iters, p;
    Real       angle, x_trans, y_trans, xt, yt, zt, correct, value;
    Transform  transform, rotation, translation;

    n_iters = 100;

    for_less( iter, 0, n_iters )
    {
        angle = 2.0 * PI * get_random_0_to_1();
        x_trans = 10.0 * get_random_0_to_1() - 5.0;
        y_trans = 10.0 * get_random_0_to_1() - 5.0;

        make_rotation_transform( angle, Z, &rotation );
        make_translation_transform( x_trans, y_trans, 0.0, &translation );
        concat_transforms( &transform, &translation, &rotation );

        value = constant;

        for_less( p, 0, n_points )
        {
            transform_point( &transform, xs[p], ys[p], 0.0, &xt, &yt, &zt );
            value += x_weights[p] * xt + y_weights[p] * yt;
        }

        transform_point( &transform, x, y, 0.0, &xt, &yt, &zt );
        correct = (dim == 0) ? xt : yt;

        if( !numerically_close( value, correct, 1.0e-6 ) )
        {
            print( "get_prediction_weights_2d_for_1_coord: %g %g\n",
                   correct, value );
            break;
        }
    }
}

private  BOOLEAN  get_prediction_weights_2d_for_1_coord(
    int    dim,
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   x_weights[],
    Real   y_weights[],
    Real   *constant )
{
    int        p, eq;
    Real       **coefs, *values, *solution, angle;
    Real       xt, yt, zt, x_centre, y_centre, x_min, x_max, inc;
    Transform  transform;
    BOOLEAN    solved;

    ALLOC2D( coefs, 2 * n_points+1, 2 * n_points+1 );
    ALLOC( values, 2 * n_points+1 );
    ALLOC( solution, 2 * n_points+1 );

    x_min = 0.0;
    x_max = 0.0;
    for_less( p, 0, n_points )
    {
        if( p == 0 || xs[p] < x_min )
            x_min = xs[p];
        if( p == 0 || xs[p] > x_max )
            x_max = xs[p];
    }

    x_centre = x_min - 2.0 * (x_max - x_min);
    y_centre = 0.0;

    inc = 0.1133422127;
    angle = 0.0;
    for_less( eq, 0, 2 * n_points+1 )
    {
        angle += inc;
        make_rotation_transform( angle, Z, &transform );

        transform_point( &transform, x - x_centre, y - y_centre, 0.0,
                         &xt, &yt, &zt );
        xt += x_centre;
        yt += y_centre;

        values[eq] = (dim==0) ? xt : yt;

        for_less( p, 0, n_points )
        {
            transform_point( &transform, xs[p] - x_centre, ys[p] - y_centre,
                             0.0, &xt, &yt, &zt );
            xt += x_centre;
            yt += y_centre;
            coefs[eq][2*p+0] = xt;
            coefs[eq][2*p+1] = yt;
        }
        coefs[eq][2*n_points] = 1.0;
    }

    solved = solve_linear_system( 2 * n_points+1, coefs, values, solution );

    for_less( p, 0, n_points )
    {
        x_weights[p] = solution[2*p+0];
        y_weights[p] = solution[2*p+1];
    }
    *constant = solution[2*n_points];

    FREE2D( coefs );
    FREE( values );
    FREE( solution );

    test_solution( dim, x, y, n_points, xs, ys, x_weights, y_weights,
                   *constant );

    return( solved );
}

public  BOOLEAN  get_prediction_weights_2d(
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   *x_weights[2],
    Real   *x_constant,
    Real   *y_weights[2],
    Real   *y_constant )
{
    return( get_prediction_weights_2d_for_1_coord( 0, x, y, n_points, xs, ys,
                          x_weights[0], x_weights[1], x_constant ) &&
            get_prediction_weights_2d_for_1_coord( 1, x, y, n_points, xs, ys,
                          y_weights[0], y_weights[1], y_constant ) );
}
