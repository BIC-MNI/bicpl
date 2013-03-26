#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

private  void  check(
    Volume   volume,
    VIO_BOOL  world_space,
    int      continuity,
    VIO_Real     x,
    VIO_Real     y,
    VIO_Real     z );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    VIO_BOOL              world_space;
    int                  continuity;
    VIO_Real                 x, y, z;
    char                 *input_filename;
    static char          *dim_names[] = { MIxspace, MIyspace, MIzspace };

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &input_filename ) )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    (void) get_int_argument( 0, &continuity );
    world_space = (argc > 3);

    if( input_volume( input_filename, 3, dim_names, NC_UNSPECIFIED, FALSE,
                      0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL ) != VIO_OK )
        return( 1 );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == VIO_OK &&
           input_real( stdin, &y ) == VIO_OK &&
           input_real( stdin, &z ) == VIO_OK )
    {
        check( volume, world_space, continuity, x, y, z );
        print( "\nEnter x, y, z: " );
    }

    return( 0 );
}

#define  TOLERANCE  0.01
#define  STEP       1.0e-4

private  VIO_Real  approx_deriv(
    Volume   volume,
    VIO_BOOL  world_space,
    int      continuity,
    VIO_Real     pos[N_DIMENSIONS],
    int      axis )
{
    VIO_Real  val1, val2, orig;

    orig = pos[axis];

    pos[axis] = orig - STEP;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          pos[VIO_X], pos[VIO_Y], pos[VIO_Z], continuity, &val1,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL );
    else
        evaluate_3D_volume( volume,
                          pos[VIO_X], pos[VIO_Y], pos[VIO_Z], continuity, &val1,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL );

    pos[axis] = orig + STEP;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          pos[VIO_X], pos[VIO_Y], pos[VIO_Z], continuity, &val2,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL );
    else
        evaluate_3D_volume( volume,
                          pos[VIO_X], pos[VIO_Y], pos[VIO_Z], continuity, &val2,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL,
                          (VIO_Real *) NULL, (VIO_Real *) NULL, (VIO_Real *) NULL );

    pos[axis] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  VIO_Real  approx_deriv2(
    Volume   volume,
    VIO_BOOL  world_space,
    int      continuity,
    VIO_Real     pos[N_DIMENSIONS],
    int      a1,
    int      a2 )
{
    VIO_Real  val1, val2, orig;

    orig = pos[a2];

    pos[a2] = orig - STEP;
    val1 = approx_deriv( volume, world_space, continuity, pos, a1 );
    pos[a2] = orig + STEP;
    val2 = approx_deriv( volume, world_space, continuity, pos, a1 );
    pos[a2] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  void  check(
    Volume   volume,
    VIO_BOOL  world_space,
    int      continuity,
    VIO_Real     x,
    VIO_Real     y,
    VIO_Real     z )
{
    VIO_Real   value, pos[N_DIMENSIONS];
    VIO_Real   deriv[N_DIMENSIONS], deriv2[N_DIMENSIONS][N_DIMENSIONS];
    VIO_Real   true_deriv[N_DIMENSIONS], true_deriv2[N_DIMENSIONS][N_DIMENSIONS];
    int    c, c2;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          x, y, z, continuity, &value,
                          &true_deriv[VIO_X], &true_deriv[VIO_Y], &true_deriv[VIO_Z],
                          &true_deriv2[VIO_X][VIO_X], &true_deriv2[VIO_X][VIO_Y],
                          &true_deriv2[VIO_X][VIO_Z], &true_deriv2[VIO_Y][VIO_Y],
                          &true_deriv2[VIO_Y][VIO_Z], &true_deriv2[VIO_Z][VIO_Z] );
    else
        evaluate_3D_volume( volume,
                          x, y, z, continuity, &value,
                          &true_deriv[VIO_X], &true_deriv[VIO_Y], &true_deriv[VIO_Z],
                          &true_deriv2[VIO_X][VIO_X], &true_deriv2[VIO_X][VIO_Y],
                          &true_deriv2[VIO_X][VIO_Z], &true_deriv2[VIO_Y][VIO_Y],
                          &true_deriv2[VIO_Y][VIO_Z], &true_deriv2[VIO_Z][VIO_Z] );

    pos[VIO_X] = x;
    pos[VIO_Y] = y;
    pos[VIO_Z] = z;

    for_less( c, 0, N_DIMENSIONS )
    {
        deriv[c] = approx_deriv( volume, world_space, continuity, pos, c );
        if( continuity > 0 )
            for_less( c2, c, N_DIMENSIONS )
                deriv2[c][c2] = approx_deriv2( volume, world_space, continuity, pos, c, c2 );
    }

    for_less( c, 0, N_DIMENSIONS )
    {
        if( !numerically_close( deriv[c], true_deriv[c], TOLERANCE ) )
            print( "Deriv %d %g %g\n", c, deriv[c], true_deriv[c] );

        if( continuity > 0 )
            for_less( c2, c, N_DIMENSIONS )
                if( !numerically_close(deriv2[c][c2],
                                       true_deriv2[c][c2],TOLERANCE) )
                    print( "Deriv2 %d %d %g %g\n", c, c2, deriv2[c][c2],
                           true_deriv2[c][c2] );
    }

}
