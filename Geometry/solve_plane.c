
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
static char rcsid[] = "$Header: /static-cvsroot/libraries/bicpl/Geometry/solve_plane.c,v 1.14 2005-08-17 22:30:25 bert Exp $";
#endif

#include "bicpl_internal.h"

BICAPI  VIO_BOOL  get_interpolation_weights_2d(
    VIO_Real   x,
    VIO_Real   y,
    int    n_points,
    VIO_Real   xs[],
    VIO_Real   ys[],
    VIO_Real   weights[] )
{
    int   i;
    VIO_Real  x_fact, y_fact, constant, n;
    VIO_Real  aa, ab, ac, bb, bc, dx, dy;
    VIO_Real  acbb, abac, aabb, denom, aabc, abab;

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

    n = (VIO_Real) n_points;
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

#ifdef   DEBUG
#define  DEBUG

static  void  test_solution(
    int    dim,
    VIO_Real   x,
    VIO_Real   y,
    int    n_points,
    VIO_Real   xs[],
    VIO_Real   ys[],
    VIO_Real   x_weights[],
    VIO_Real   y_weights[],
    VIO_Real   constant )
{
    int        iter, n_iters, p;
    VIO_Real       angle, x_trans, y_trans, xt, yt, zt, correct, value;
    VIO_Transform  transform, rotation, translation;

    n_iters = 100;

    for_less( iter, 0, n_iters )
    {
        angle = 2.0 * M_PI * get_random_0_to_1();
        x_trans = 10.0 * get_random_0_to_1() - 5.0;
        y_trans = 10.0 * get_random_0_to_1() - 5.0;

        make_rotation_transform( angle, VIO_Z, &rotation );
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
#endif

static  VIO_BOOL   get_two_point_prediction(
    VIO_Real   x,
    VIO_Real   y,
    VIO_Real   x1,
    VIO_Real   y1,
    VIO_Real   x2,
    VIO_Real   y2,
    VIO_Real   *xwx1,
    VIO_Real   *xwy1,
    VIO_Real   *xwx2,
    VIO_Real   *xwy2,
    VIO_Real   *ywx1,
    VIO_Real   *ywy1,
    VIO_Real   *ywx2,
    VIO_Real   *ywy2 )
{
    VIO_Real   sx, sy, s, t, cax, cay, s_len;

    sx = x2 - x1;
    sy = y2 - y1;
    cax = x - x1;
    cay = y - y1;

    s_len = sx * sx + sy * sy;
    if( s_len == 0.0 )
        return( FALSE );

    s = (cax * sx + cay * sy) / s_len;
    t = (cax * (-sy) + cay * sx) / s_len;

    *xwx1 = 1.0 - s;
    *xwy1 = t;
    *xwx2 = s;
    *xwy2 = -t;
    *ywx1 = -t;
    *ywy1 = 1.0 - s;
    *ywx2 = t;
    *ywy2 = s;

    return( TRUE );
}

BICAPI  VIO_BOOL  get_prediction_weights_2d(
    VIO_Real   x,
    VIO_Real   y,
    int    n_points,
    VIO_Real   xs[],
    VIO_Real   ys[],
    VIO_Real   *x_weights[2],
    VIO_Real   *x_constant,
    VIO_Real   *y_weights[2],
    VIO_Real   *y_constant )
{
    int   dim, p, p1, p2, n_pairs;
    VIO_Real  xwx1, xwy1, xwx2, xwy2, ywx1, ywy1, ywx2, ywy2;

    *x_constant = 0.0;
    *y_constant = 0.0;

    for_less( dim, 0, 2 )
    {
        for_less( p, 0, n_points )
        {
            x_weights[dim][p] = 0.0;
            y_weights[dim][p] = 0.0;
        }
    }

    n_pairs = 0;
    for_less( p1, 0, n_points-1 )
    {
        for_less( p2, p1+1, n_points )
        {
            if( get_two_point_prediction( x, y, xs[p1], ys[p1], xs[p2], ys[p2],
                                          &xwx1, &xwy1, &xwx2, &xwy2,
                                          &ywx1, &ywy1, &ywx2, &ywy2 ) )
            {
                x_weights[0][p1] += xwx1;
                x_weights[1][p1] += xwy1;
                x_weights[0][p2] += xwx2;
                x_weights[1][p2] += xwy2;
                y_weights[0][p1] += ywx1;
                y_weights[1][p1] += ywy1;
                y_weights[0][p2] += ywx2;
                y_weights[1][p2] += ywy2;
                ++n_pairs;
            }
        }
    }

    for_less( dim, 0, 2 )
    {
        for_less( p, 0, n_points )
        {
            x_weights[dim][p] /= (VIO_Real) n_pairs;
            y_weights[dim][p] /= (VIO_Real) n_pairs;
        }
    }

#ifdef DEBUG
    test_solution( 0, x, y, n_points, xs, ys, x_weights[0], x_weights[1],
                   *x_constant );
    test_solution( 1, x, y, n_points, xs, ys, y_weights[0], y_weights[1],
                   *y_constant );
#endif

    return( TRUE );
}

#ifdef DEBUG
static  void  test_solution_3d(
    int    dim,
    VIO_Real   x,
    VIO_Real   y,
    VIO_Real   z,
    int    n_points,
    VIO_Real   xs[],
    VIO_Real   ys[],
    VIO_Real   zs[],
    VIO_Real   *weights[3] )
{
    int        iter, n_iters, p;
    VIO_Real       y_angle, z_angle, x_trans, y_trans, z_trans;
    VIO_Real       correct, value, ps[3];
    VIO_Transform  transform, y_rotation, z_rotation, translation;

    n_iters = 100;

    for_less( iter, 0, n_iters )
    {
        z_angle = 2.0 * M_PI * get_random_0_to_1();
        y_angle = 2.0 * M_PI * get_random_0_to_1();
        x_trans = 10.0 * get_random_0_to_1() - 5.0;
        y_trans = 10.0 * get_random_0_to_1() - 5.0;
        z_trans = 10.0 * get_random_0_to_1() - 5.0;

        make_rotation_transform( y_angle, VIO_Y, &y_rotation );
        make_rotation_transform( z_angle, VIO_Z, &z_rotation );
        make_translation_transform( x_trans, y_trans, z_trans, &translation );
        concat_transforms( &transform, &translation, &y_rotation );
        concat_transforms( &transform, &transform, &z_rotation );

        value = 0.0;

        for_less( p, 0, n_points )
        {
            transform_point( &transform, xs[p], ys[p], zs[p],
                             &ps[0], &ps[1], &ps[2] );
            value += weights[0][p] * ps[0];
            value += weights[1][p] * ps[1];
            value += weights[2][p] * ps[2];
        }

        transform_point( &transform, x, y, z, &ps[0], &ps[1], &ps[2] );
        correct = ps[dim];

        if( !numerically_close( value, correct, 1.0e-6 ) )
        {
            print( "get_prediction_weights_3d: %d: %g %g\n",
                   iter, correct, value );
            break;
        }
    }
}
#endif

#define  FACTOR 1.0e-6

BICAPI  VIO_BOOL  get_prediction_weights_3d(
    VIO_Real   x,
    VIO_Real   y,
    VIO_Real   z,
    int    n_points,
    VIO_Real   xs[],
    VIO_Real   ys[],
    VIO_Real   zs[],
    VIO_Real   *x_weights[3],
    VIO_Real   *y_weights[3],
    VIO_Real   *z_weights[3] )
{
    int                    p, p1, dim, n_iters, iter;
    VIO_Real                   *parms, *coords[VIO_N_DIMENSIONS];
    VIO_Real                   coord[VIO_N_DIMENSIONS];
    VIO_Real                   len, max_len, sum;
    linear_least_squares   lsq;
    VIO_BOOL                solved;
    VIO_Real                   y_angle, z_angle, x_trans, y_trans, z_trans;
    VIO_Transform              transform, y_rotation, z_rotation, translation;

    ALLOC( parms, n_points-1 );

    ALLOC( coords[0], n_points );
    ALLOC( coords[1], n_points );
    ALLOC( coords[2], n_points );

    max_len = 0.0;
    for_less( p, 0, n_points )
    {
        len = xs[p] * xs[p] + ys[p] * ys[p] + zs[p] * zs[p];
        max_len = MAX( max_len, len );
    }

    max_len = sqrt( max_len );

    initialize_linear_least_squares( &lsq, n_points-1 );

    n_iters = 100;
    n_iters = 1;

    for_less( iter, 0, n_iters )
    {
        z_angle = 2.0 * M_PI * get_random_0_to_1();
        y_angle = 2.0 * M_PI * get_random_0_to_1();
        x_trans = 10.0 * get_random_0_to_1() - 5.0;
        y_trans = 10.0 * get_random_0_to_1() - 5.0;
        z_trans = 10.0 * get_random_0_to_1() - 5.0;

        make_rotation_transform( y_angle, VIO_Y, &y_rotation );
        make_rotation_transform( z_angle, VIO_Z, &z_rotation );
        make_translation_transform( x_trans, y_trans, z_trans, &translation );
        concat_transforms( &transform, &translation, &y_rotation );
        concat_transforms( &transform, &transform, &z_rotation );

        if( iter == 0 )
            make_identity_transform( &transform );

        for_less( p, 0, n_points )
        {
            transform_point( &transform, xs[p], ys[p], zs[p],
                             &coords[0][p], &coords[1][p], &coords[2][p] );
                             
        }

        transform_point( &transform, x, y, z,
                         &coord[0], &coord[1], &coord[2] );

        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            for_less( p, 0, n_points-1 )
                parms[p] = coords[dim][p] - coords[dim][n_points-1];

            add_to_linear_least_squares( &lsq, parms,
                                         coord[dim] - coords[dim][n_points-1] );
        }
    }

    for_less( p, 0, n_points-1 )
    {
        for_less( p1, 0, n_points-1 )
            parms[p1] = 0.0;
        parms[p] = FACTOR * max_len;

        add_to_linear_least_squares( &lsq, parms, 0.0 );
    }

    for_less( p, 0, n_points-1 )
        parms[p] = 1.0;

    add_to_linear_least_squares( &lsq, parms, 1.0 );

    solved = get_linear_least_squares_solution( &lsq, parms );

    if( solved )
    {
        for_less( p, 0, n_points )
        for_less( dim, 0, VIO_N_DIMENSIONS )
        {
            x_weights[dim][p] = 0.0;
            y_weights[dim][p] = 0.0;
            z_weights[dim][p] = 0.0;
        }

        sum = 0.0;
        for_less( p, 0, n_points-1 )
        {
            sum += parms[p];
            x_weights[0][p] = parms[p];
            y_weights[1][p] = parms[p];
            z_weights[2][p] = parms[p];
        }

        x_weights[0][n_points-1] = 1.0 - sum;
        y_weights[1][n_points-1] = 1.0 - sum;
        z_weights[2][n_points-1] = 1.0 - sum;
    }

    delete_linear_least_squares( &lsq );
    FREE( parms );
    FREE( coords[0] );
    FREE( coords[1] );
    FREE( coords[2] );

    if( !solved )
        return( FALSE );


#ifdef DEBUG
    test_solution_3d( 0, x, y, z, n_points, xs, ys, zs, x_weights );
    test_solution_3d( 1, x, y, z, n_points, xs, ys, zs, y_weights );
    test_solution_3d( 2, x, y, z, n_points, xs, ys, zs, z_weights );
#endif

    return( TRUE );
}
