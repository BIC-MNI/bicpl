
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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/t.c,v 1.1 1999-06-21 21:00:26 stever Exp $";
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

private  BOOLEAN  get_prediction_weights_2d_for_1_coord(
    int    dim,
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   x_weights[],
    Real   y_weights[] )
{
    int        p, eq, n_trans;
    Real       **coefs, *values, *solution, offset[2], angle;
    Real       xt, yt, zt;
    Transform  transform;
    BOOLEAN    solved;

    ALLOC2D( coefs, 2 * n_points+1, 2 * n_points+1 );
    ALLOC( values, 2 * n_points+1 );
    ALLOC( solution, 2 * n_points+1 );

    n_trans = 0;

    for_less( eq, 0, 2 * n_points+1 )
    {
        if( eq < n_trans )
        {
            offset[eq] = 1.0;
            offset[1-eq] = 0.0;
            make_translation_transform( offset[X], offset[Y], 0.0, &transform );
        }
        else
        {
            angle = 2.0 * PI * (Real) (eq - n_trans) /
                         (Real) (2*n_points+1-n_trans);
            make_rotation_transform( angle, Z, &transform );
        }

        transform_point( &transform, x, y, 0.0, &xt, &yt, &zt );
        values[eq] = (dim==0) ? xt : yt;

        for_less( p, 0, n_points )
        {
            transform_point( &transform, xs[p], ys[p], 0.0,
                             &xt, &yt, &zt );
            coefs[eq][2*p+0] = xt;
            coefs[eq][2*p+1] = yt;
        }
        coefs[eq][2*n_points] = 1.0;
    }

    solved = solve_linear_system( 2 * n_points+1, coefs, values, solution );

    print( "e = %g\n", solution[2*n_points] );

    for_less( p, 0, n_points )
    {
        x_weights[p] = solution[2*p+0];
        y_weights[p] = solution[2*p+1];
    }

    FREE2D( coefs );
    FREE( values );
    FREE( solution );

    return( solved );
}

public  BOOLEAN  get_prediction_weights_2d(
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   *x_weights[2],
    Real   *y_weights[2] )
{
    return( get_prediction_weights_2d_for_1_coord( 0, x, y, n_points, xs, ys,
                                               x_weights[0], x_weights[1] ) &&
            get_prediction_weights_2d_for_1_coord( 1, x, y, n_points, xs, ys,
                                               y_weights[0], y_weights[1] ) );
}
