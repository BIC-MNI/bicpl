#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

private  void  check(
    Volume   volume,
    BOOLEAN  world_space,
    int      continuity,
    Real     x,
    Real     y,
    Real     z );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    BOOLEAN              world_space;
    int                  continuity;
    Real                 x, y, z;
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
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    print( "Enter x, y, z: " );
    while( input_real( stdin, &x ) == OK &&
           input_real( stdin, &y ) == OK &&
           input_real( stdin, &z ) == OK )
    {
        check( volume, world_space, continuity, x, y, z );
        print( "\nEnter x, y, z: " );
    }

    return( 0 );
}

#define  TOLERANCE  0.01
#define  STEP       1.0e-4

private  Real  approx_deriv(
    Volume   volume,
    BOOLEAN  world_space,
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      axis )
{
    Real  val1, val2, orig;

    orig = pos[axis];

    pos[axis] = orig - STEP;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          pos[X], pos[Y], pos[Z], continuity, &val1,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );
    else
        evaluate_3D_volume( volume,
                          pos[X], pos[Y], pos[Z], continuity, &val1,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig + STEP;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          pos[X], pos[Y], pos[Z], continuity, &val2,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );
    else
        evaluate_3D_volume( volume,
                          pos[X], pos[Y], pos[Z], continuity, &val2,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL,
                          (Real *) NULL, (Real *) NULL, (Real *) NULL );

    pos[axis] = orig;

    return( (val2 - val1) / STEP / 2.0 );
}

private  Real  approx_deriv2(
    Volume   volume,
    BOOLEAN  world_space,
    int      continuity,
    Real     pos[N_DIMENSIONS],
    int      a1,
    int      a2 )
{
    Real  val1, val2, orig;

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
    BOOLEAN  world_space,
    int      continuity,
    Real     x,
    Real     y,
    Real     z )
{
    Real   value, pos[N_DIMENSIONS];
    Real   deriv[N_DIMENSIONS], deriv2[N_DIMENSIONS][N_DIMENSIONS];
    Real   true_deriv[N_DIMENSIONS], true_deriv2[N_DIMENSIONS][N_DIMENSIONS];
    int    c, c2;

    if( world_space )
        evaluate_3D_volume_in_world( volume,
                          x, y, z, continuity, &value,
                          &true_deriv[X], &true_deriv[Y], &true_deriv[Z],
                          &true_deriv2[X][X], &true_deriv2[X][Y],
                          &true_deriv2[X][Z], &true_deriv2[Y][Y],
                          &true_deriv2[Y][Z], &true_deriv2[Z][Z] );
    else
        evaluate_3D_volume( volume,
                          x, y, z, continuity, &value,
                          &true_deriv[X], &true_deriv[Y], &true_deriv[Z],
                          &true_deriv2[X][X], &true_deriv2[X][Y],
                          &true_deriv2[X][Z], &true_deriv2[Y][Y],
                          &true_deriv2[Y][Z], &true_deriv2[Z][Z] );

    pos[X] = x;
    pos[Y] = y;
    pos[Z] = z;

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
