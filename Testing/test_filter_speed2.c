#include  <volume_io/internal_volume_io.h>
#include  <bicpl.h>

private  void     evaluate_sync_interpolation(
    Volume    volume,
    VIO_Real      x,
    VIO_Real      y,
    VIO_Real      z,
    VIO_Real      *value );

private  void    initialize_sinc_approx(
    VIO_Real   max_d2,
    int    n_lookup );

int  main(
    int   argc,
    char  *argv[] )
{
    Volume               volume;
    int                  iter, n_iters, continuity, n_lookup, dim;
    int                  sizes[N_DIMENSIONS];
    VIO_Real                 separations[N_DIMENSIONS];
    VIO_Real                 x, y, z, value, max_d2, delta;
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
        delta = (VIO_Real) sizes[dim] * FABS(separations[dim]);
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

private  VIO_Real  sinc(
    VIO_Real   d_squared )
{
    VIO_Real   d;

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
static   VIO_Real            max_sinc_arg, sinc_factor;
static   lookup_struct   *precomp_sinc;

private  void    initialize_sinc_approx(
    VIO_Real   max_d2,
    int    n_lookup )
{
    int    i;
    VIO_Real   next_sinc, this_sinc;

    lookup_size = n_lookup;
    max_sinc_arg = max_d2;
    ALLOC( precomp_sinc, lookup_size );

    next_sinc = 0.0;
    for_less( i, 0, lookup_size )
    {
        this_sinc = next_sinc;
        next_sinc = sinc( (VIO_Real) (i+1) / (VIO_Real) lookup_size * max_sinc_arg );

        precomp_sinc[i].scale = next_sinc - this_sinc;
        precomp_sinc[i].trans = this_sinc -
                                precomp_sinc[i].scale * (VIO_Real) i;
    }

    sinc_factor = (VIO_Real) lookup_size / max_sinc_arg;
}

private  VIO_Real  evaluate_sinc(
    VIO_Real   d_squared )
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

#define  DO( i ) \
            values[i*offset+i] * slice[i]

#define  DO( i ) \
            *((VIO_Real *) ((char *) values + i * offset+i)) * \
            *((VIO_Real *) ((char *) slice + i))

private  void     evaluate_sync_interpolation(
    Volume    volume,
    VIO_Real      x,
    VIO_Real      y,
    VIO_Real      z,
    VIO_Real      *value )
{
    int    v0, v1, v2, sizes[N_DIMENSIONS], ind, dim;
    long   which, *offsets, offset;
    VIO_Real   delta, d, dd, ddd, *values;
    VIO_Real   *d0, *d1, *d2;
    VIO_Real   voxel[N_DIMENSIONS], *slice, sum_value, sum_weight, weight;
    VIO_Real   separations[N_DIMENSIONS];
    lookup_struct  *lookup;

    convert_world_to_voxel( volume, x, y, z, voxel );

    get_volume_separations( volume, separations );
    for_less( dim, 0, N_DIMENSIONS )
        separations[dim] = FABS( separations[dim] );

    get_volume_sizes( volume, sizes );
    ALLOC( slice, sizes[2] );

    ALLOC( offsets, sizes[2] );
    for_less( v2, 0, sizes[2] )
        offsets[v2] = 0;

    ALLOC( d0, sizes[0] );
    ALLOC( d1, sizes[1] );
    ALLOC( d2, sizes[2] );
    for_less( v0, 0, sizes[0] )
    {
        delta = ((VIO_Real) v0 - voxel[0]) * separations[0];
        d0[v0] = sinc_factor * delta * delta;
    }
    for_less( v1, 0, sizes[1] )
    {
        delta = ((VIO_Real) v1 - voxel[1]) * separations[1];
        d1[v1] = sinc_factor * delta * delta;
    }
    for_less( v2, 0, sizes[2] )
    {
        delta = ((VIO_Real) v2 - voxel[2]) * separations[2];
        d2[v2] = sinc_factor * delta * delta;
    }

    sum_value = 0.0;
    sum_weight = 0.0;
    sum_weight = 1.0;

    ALLOC( values, 1000 );
    for_less( v0, 0, 1000 )
        values[v0] = 1.0;

    get_volume_voxel_hyperslab_3d( volume, 0, 0, 0, 1, 1, sizes[2], slice );
    offset = 0;

    for_less( v0, 0, sizes[0] )
    {
        d = d0[v0];
        for_less( v1, 0, sizes[1] )
        {
            ind = 0;
            dd = d + d1[v1];
            if( dd == 1214114343 )
                offset = 1;
            which = 0;
            sum_value +=
            DO( 0 ) +
            DO( 8 ) +
            DO( 16 ) +
            DO( 24 ) +
            DO( 32 ) +
            DO( 40 ) +
            DO( 48 ) +
            DO( 56 ) +
            DO( 64 ) +
            DO( 72 ) +
            DO( 80 ) +
            DO( 88 ) +
            DO( 96 ) +
            DO( 104 ) +
            DO( 112 ) +
            DO( 120 ) +
            DO( 128 ) +
            DO( 136 ) +
            DO( 144 ) +
            DO( 152 ) +
            DO( 160 ) +
            DO( 168 ) +
            DO( 176 ) +
            DO( 184 ) +
            DO( 192 ) +
            DO( 200 ) +
            DO( 208 ) +
            DO( 216 ) +
            DO( 224 ) +
            DO( 232 ) +
            DO( 240 ) +
            DO( 248 ) +
            DO( 256 ) +
            DO( 264 ) +
            DO( 272 ) +
            DO( 280 ) +
            DO( 288 ) +
            DO( 296 ) +
            DO( 304 ) +
            DO( 312 );
        }
    }

    FREE( values );
    FREE( slice );
    FREE( d0 );
    FREE( d1 );
    FREE( d2 );
    FREE( offsets );

    *value = CONVERT_VOXEL_TO_VALUE( volume, sum_value / sum_weight );
}
