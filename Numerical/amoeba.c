
#include  <internal_volume_io.h>
#include  <numerical.h>

#define  FLIP_RATIO      1.0
#define  CONTRACT_RATIO  0.5
#define  STRETCH_RATIO   2.0

private  Real  get_function_value(
    amoeba_struct  *amoeba,
    float          parameters[] )
{
    return( (*amoeba->function) ( amoeba->function_data, parameters ) );
}

public  void  initialize_amoeba(
    amoeba_struct     *amoeba,
    int               n_parameters,
    float             initial_parameters[],
    float             parameter_delta,
    amoeba_function   function,
    void              *function_data,
    Real              tolerance )
{
    int    i, j;

    amoeba->n_parameters = n_parameters;
    amoeba->function = function;
    amoeba->function_data = function_data;
    amoeba->tolerance = tolerance;
    ALLOC2D( amoeba->parameters, n_parameters+1, n_parameters );
    ALLOC( amoeba->values, n_parameters+1 );

    for_less( i, 0, n_parameters+1 )
    {
        for_less( j, 0, n_parameters )
        {
            amoeba->parameters[i][j] = initial_parameters[j];
            if( i > 0 && j == i - 1 )
                amoeba->parameters[i][j] += parameter_delta;
        }

        amoeba->values[i] = get_function_value( amoeba, amoeba->parameters[i] );
    }
}

public  Real  get_amoeba_parameters(
    amoeba_struct  *amoeba,
    float          parameters[] )
{
    int   i, j, low;

    low = 0;
    for_less( i, 1, amoeba->n_parameters+1 )
    {
        if( amoeba->values[i] < amoeba->values[low] )
            low = i;
    }

    for_less( j, 0, amoeba->n_parameters )
        parameters[j] = amoeba->parameters[low][j];

    return( amoeba->values[low] );
}

public  void  terminate_amoeba(
    amoeba_struct  *amoeba )
{
    FREE2D( amoeba->parameters );
    FREE( amoeba->values );
}

private  Real  try_amoeba(
    amoeba_struct  *amoeba,
    Real           sum[],
    int            high,
    Real           fac )
{
    int    j;
    Real   y_try, fac1, fac2;
    float  *parameters;

    ALLOC( parameters, amoeba->n_parameters );

    fac1 = (1.0 - fac) / amoeba->n_parameters;
    fac2 = fac - fac1;

    for_less( j, 0, amoeba->n_parameters )
        parameters[j] = sum[j] * fac1 + amoeba->parameters[high][j] * fac2;

    y_try = get_function_value( amoeba, parameters );

    if( y_try < amoeba->values[high] )
    {
        amoeba->values[high] = y_try;
        for_less( j, 0, amoeba->n_parameters )
        {
            sum[j] += parameters[j] - amoeba->parameters[high][j];
            amoeba->parameters[high][j] = parameters[j];
        }
    }

    FREE( parameters );

    return( y_try );
}

public  BOOLEAN  perform_amoeba(
    amoeba_struct  *amoeba )
{
    int     i, j, low, high, next_high;
    Real    *sum, y_try, y_save;

    if( amoeba->values[0] > amoeba->values[1] )
    {
        high = 0;
        next_high = 1;
    }
    else
    {
        high = 1;
        next_high = 0;
    }

    low = next_high;

    for_less( i, 2, amoeba->n_parameters+1 )
    {
        if( amoeba->values[i] < amoeba->values[low] )
            low = i;
        else if( amoeba->values[i] > amoeba->values[high] )
        {
            next_high = high;
            high = i;
        }
        else if( amoeba->values[i] > amoeba->values[next_high] )
            next_high = i;
    }

    if( numerically_close( amoeba->values[low], amoeba->values[high],
                           amoeba->tolerance ) )
    {
        return( TRUE );
    }

    ALLOC( sum, amoeba->n_parameters );

    for_less( j, 0, amoeba->n_parameters )
    {
        sum[j] = 0.0;
        for_less( i, 0, amoeba->n_parameters+1 )
            sum[j] += amoeba->parameters[i][j];
    }

    y_try = try_amoeba( amoeba, sum, high, -FLIP_RATIO );

    if( y_try <= amoeba->values[low] )
        y_try = try_amoeba( amoeba, sum, high, STRETCH_RATIO );
    else if( y_try >= amoeba->values[next_high] )
    {
        y_save = amoeba->values[high];
        y_try = try_amoeba( amoeba, sum, high, CONTRACT_RATIO );

        if( y_try >= y_save )
        {
            for_less( i, 0, amoeba->n_parameters+1 )
            {
                if( i != low )
                {
                    for_less( j, 0, amoeba->n_parameters )
                    {
                        amoeba->parameters[i][j] = (amoeba->parameters[i][j] +
                                            amoeba->parameters[low][j]) / 2.0;
                    }

                    amoeba->values[i] = get_function_value( amoeba,
                                                  amoeba->parameters[i] );
                }
            }
        }
    }

    FREE( sum );

    return( FALSE );
}
