#include  <internal_volume_io.h>
#include  <bicpl.h>

private  void     evaluate_sync_interpolation(
    Volume    volume,
    Real      x,
    Real      y,
    Real      z,
    Real      *value );

private  void    initialize_sinc_approx(
    Real   max_d2,
    int    n_lookup );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  iter, n_iters, continuity, n_lookup, dim;
    int                  sizes[N_DIMENSIONS];
    Real                 separations[N_DIMENSIONS];
    Real                 x, y, z, value, max_d2, delta;
    STRING               input_filename;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( NULL, &input_filename ) )
    {
        print( "Need args.\n" );
        return( 1 );
    }

    (void) get_int_argument( 0, &continuity );
    (void) get_int_argument( 10, &n_iters );
    (void) get_int_argument( 1000000, &n_lookup );

    if( input_volume( input_filename, 3, File_order_dimension_names,
                      NC_UNSPECIFIED, FALSE, 0.0, 0.0, TRUE, &volume,
                      (minc_input_options *) NULL ) != OK )
        return( 1 );

    start_timing();

    get_volume_sizes( volume, sizes );
    get_volume_separations( volume, separations );

    max_d2 = 0.0;
    for_less( dim, 0, N_DIMENSIONS )
    {
        delta = (Real) sizes[dim] * FABS(separations[dim]);
        max_d2 += delta * delta;
    }

    initialize_sinc_approx( max_d2, n_lookup );

    end_timing( "Initializing Sinc", 1 );

    x = 0.2;
    y = 0.3;
    z = 0.4;

    start_timing();

    for_less( iter, 0, n_iters )
    {
        if( continuity <= 2 )
        {
            evaluate_volume_in_world( volume, x, y, z, continuity, FALSE, 0.0,
                                      &value,
                                      NULL, NULL, NULL,
                                      NULL, NULL, NULL, NULL, NULL, NULL );
        }
        else
        {
            evaluate_sync_interpolation( volume, x, y, z, &value );
        }
    }

    end_timing( "Interpolation", n_iters );

    return( 0 );
}

private  Real  sinc(
    Real   d_squared )
{
    Real   d;

    if( d_squared == 0.0 )
        return( 1.0 );

    d = sqrt( d_squared );

    return( sin( d ) / d );
}

typedef  struct
{
    double  scale;
    double trans;
} lookup_struct;

static   int             lookup_size;
static   Real            max_sinc_arg, sinc_factor;
static   lookup_struct   *precomp_sinc;

private  void    initialize_sinc_approx(
    Real   max_d2,
    int    n_lookup )
{
    int    i;
    Real   next_sinc, this_sinc;

    lookup_size = n_lookup;
    max_sinc_arg = max_d2;
    ALLOC( precomp_sinc, lookup_size );

    next_sinc = 0.0;
    for_less( i, 0, lookup_size )
    {
        this_sinc = next_sinc;
        next_sinc = sinc( (Real) (i+1) / (Real) lookup_size * max_sinc_arg );

        precomp_sinc[i].scale = next_sinc - this_sinc;
        precomp_sinc[i].trans = this_sinc -
                                precomp_sinc[i].scale * (Real) i;
    }

    sinc_factor = (Real) lookup_size / max_sinc_arg;
}

private  Real  evaluate_sinc(
    Real   d_squared )
{
    lookup_struct  *lookup;


/*
    if( d_squared >= max_sinc_arg )
    {
        return( sinc( d_squared ) );
    }

    x = sinc_factor * d_squared;
*/

    lookup = &precomp_sinc[(int) d_squared];

    return( lookup->trans + lookup->scale * d_squared );
}

private  void     evaluate_sync_interpolation(
    Volume    volume,
    Real      x,
    Real      y,
    Real      z,
    Real      *value )
{
    int    v0, v1, v2, sizes[N_DIMENSIONS], ind, dim;
    Real   delta, d, dd, ddd;
    Real   *d0, *d1, *d2;
    Real   voxel[N_DIMENSIONS], *slice, sum_value, sum_weight, weight;
    Real   separations[N_DIMENSIONS];
    lookup_struct  *lookup;

    convert_world_to_voxel( volume, x, y, z, voxel );

    get_volume_separations( volume, separations );
    for_less( dim, 0, N_DIMENSIONS )
        separations[dim] = FABS( separations[dim] );

    get_volume_sizes( volume, sizes );
    ALLOC( slice, sizes[2] );

    ALLOC( d0, sizes[0] );
    ALLOC( d1, sizes[1] );
    ALLOC( d2, sizes[2] );
    for_less( v0, 0, sizes[0] )
    {
        delta = ((Real) v0 - voxel[0]) * separations[0];
        d0[v0] = sinc_factor * delta * delta;
    }
    for_less( v1, 0, sizes[1] )
    {
        delta = ((Real) v1 - voxel[1]) * separations[1];
        d1[v1] = sinc_factor * delta * delta;
    }
    for_less( v2, 0, sizes[2] )
    {
        delta = ((Real) v2 - voxel[2]) * separations[2];
        d2[v2] = sinc_factor * delta * delta;
    }

    sum_value = 0.0;
    sum_weight = 0.0;

    for_less( v0, 0, sizes[0] )
    {
        d = d0[v0];
        for_less( v1, 0, sizes[1] )
        {
            get_volume_voxel_hyperslab_3d( volume, v0, v1, 0, 1, 1, sizes[2],
                                           slice );
            ind = 0;
            dd = d + d1[v1];
            for_less( v2, 0, sizes[2] )
            {
                ddd = dd + d2[v2];
                lookup = &precomp_sinc[(int) ddd];
                weight = lookup->trans + lookup->scale * ddd;

                sum_weight += weight;
                sum_value += weight * slice[ind];
                ++ind;
            }
        }
    }

    FREE( slice );
    FREE( d0 );
    FREE( d1 );
    FREE( d2 );

    *value = CONVERT_VOXEL_TO_VALUE( volume, sum_value / sum_weight );
}
