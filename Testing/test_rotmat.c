#include  <internal_volume_io.h>
#include  <bicpl.h>

int  main(
    int   argc,
    char  *argv[] )
{
    Real        x_rot, y_rot, z_rot;
    Real        **mat, angles[3];
    int         i, n_iters, m, n;
    Transform   tx, ty, tz, transform;

    set_random_seed( 98436131 );

    n_iters = 10000;

    ALLOC2D( mat, 3, 3 );

    for_less( i, 0, n_iters )
    {
        x_rot = 0.0 + 2.0 * PI * get_random_0_to_1();
        y_rot = 0.0 + 2.0 * PI * get_random_0_to_1();
        z_rot = 0.0 + 2.0 * PI * get_random_0_to_1();

        make_rotation_transform( x_rot, X, &tx ) ;
        make_rotation_transform( y_rot, Y, &ty ) ;
        make_rotation_transform( z_rot, Z, &tz ) ;

        concat_transforms( &transform, &tx, &ty );
        concat_transforms( &transform, &transform, &tz );

        for_less( m, 0, 3 )
        for_less( n, 0, 3 )
            mat[m][n] = Transform_elem(transform,m,n);

        rotmat_to_ang( mat, angles );
    }

    return( 0 );
}
