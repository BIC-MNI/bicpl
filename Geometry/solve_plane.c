
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
static char rcsid[] = "$Header: /private-cvsroot/libraries/bicpl/Geometry/solve_plane.c,v 1.2 1996-09-13 13:16:19 david Exp $";
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
    Real  av_fixed[FIXED_SIZE], bv_fixed[FIXED_SIZE], cv_fixed[FIXED_SIZE];
    Real  aa, ab, ac, bb, bc, cc, *av, *bv, *cv, dx, dy;
    Real  ab2, acbb, abac, abbc, aabc2, acbb2, aabb4, denom;

    if( n_points > FIXED_SIZE )
    {
        ALLOC( av, n_points );
        ALLOC( bv, n_points );
        ALLOC( cv, n_points );
    }
    else
    {
        av = av_fixed;
        bv = bv_fixed;
        cv = cv_fixed;
    }

    aa = 0.0;
    ab = 0.0;
    ac = 0.0;
    bb = 0.0;
    bc = 0.0;
    cc = 0.0;

    for_less( i, 0, n_points )
    {
        av[i] = 0.0;
        bv[i] = 0.0;
        cv[i] = 0.0;
    }

    for_less( i, 0, n_points )
    {
        dx = xs[i] - x;
        dy = ys[i] - y;
        aa += dx * dx;
        ab += 2.0 * dx * dy;
        ac += 2.0 * dx;
        bb += dy * dy;
        bc += 2.0 * dy;
        cc += 1.0;
        av[i] += -2.0 * dx;
        bv[i] += -2.0 * dy;
        cv[i] += -2.0;
    }

    ab2 = ab * ab;
    acbb = ac * bb;
    abac = ab * ac;
    abbc = ab * bc;
    aabc2 = 2.0 * aa * bc;
    acbb2 = 2.0 * ac * bb;
    aabb4 = 4.0 * aa * bb;
    denom = -2.0 *
            (-ab2 * cc - ac * acbb + abbc * ac + aabb4 * cc - aa * bc * bc);

    if( denom == 0.0 )
        return( FALSE );

    for_less( i, 0, n_points )
    {
        weights[i] = ((aabb4 - ab2) * cv[i] +
                      (abac - aabc2) * bv[i] +
                      (abbc - acbb2) * av[i]) /
                      denom;
    }

    if( n_points > FIXED_SIZE )
    {
        FREE( av );
        FREE( bv );
        FREE( cv );
    }

    return( TRUE );
}
