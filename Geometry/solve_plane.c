
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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/solve_plane.c,v 1.3 1996-09-13 13:47:22 david Exp $";
#endif

#include  <geom.h>
#include  <internal_volume_io.h>

#define  FIXED_SIZE  20

public  BOOLEAN  get_interpolation_weights_2d(
    Real   x,
    Real   y,
    int    n_points,
    Real   xs[],
    Real   ys[],
    Real   weights[] )
{
    int   i;
    Real  av_fact, bv_fact, constant, n;
    Real  aa, ab, ac, bb, bc, dx, dy;
    Real  acbb, abac, aanbb4, denom, aabc, ab2n;

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
        ab += 2.0 * dx * dy;
        ac += 2.0 * dx;
        bb += dy * dy;
        bc += 2.0 * dy;
    }

    n = (Real) n_points;
    aanbb4 = 4.0 * aa * n * bb;
    acbb = ac * bb;
    aabc = aa * bc;
    ab2n = ab * ab * n;
    abac = ab * ac;

    denom = 2.0 * (aanbb4 - ac * acbb - bc * aabc - ab2n + abac * bc);

    if( denom == 0.0 )
        return( FALSE );

    bv_fact = -2.0 * (2.0 * aabc - abac) / denom;
    av_fact = -2.0 * (2.0 * acbb - ab * bc) / denom;
    constant = (2.0 * aanbb4 - 2.0 * ab2n) / denom - x * av_fact - y * bv_fact;

    for_less( i, 0, n_points )
        weights[i] = constant + bv_fact * ys[i] + av_fact * xs[i];

    return( TRUE );
}
