#include  <internal_volume_io.h>
#include  <bicpl.h>

private  BOOLEAN  scaled_maximal_pivoting_gaussian_elimination_float(
    int    n,
    float  **coefs,
    int    n_values,
    float  **values )
{
    int       i, j, v, *row;
    Real      **a, **solution;
    BOOLEAN   success;

    ALLOC( row, n );
    ALLOC2D( a, n, n );
    ALLOC2D( solution, n, n_values );

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            a[i][j] = (Real) coefs[i][j];
        for_less( v, 0, n_values )
            solution[i][v] = (Real) values[v][i];
    }

    success = scaled_maximal_pivoting_gaussian_elimination( n, row, a, n_values,
                                                            solution );

    if( success )
    {
        for_less( i, 0, n )
        {
            for_less( v, 0, n_values )
            {
                values[v][i] = (float) solution[row[i]][v];
            }
        }
    }

    FREE2D( a );
    FREE2D( solution );
    FREE( row );

    return( success );
}

public  BOOLEAN  solve_linear_system_float(
    int    n,
    float  **coefs,
    float  values[],
    float  solution[] )
{
    int       i;

    for_less( i, 0, n )
        solution[i] = values[i];

    return( scaled_maximal_pivoting_gaussian_elimination_float( n, coefs, 1,
                                                                &solution ) );
}

public  BOOLEAN  invert_square_matrix_float(
    int    n,
    float  **matrix,
    float  **inverse )
{
    float     tmp;
    BOOLEAN   success;
    int       i, j;

    for_less( i, 0, n )
    {
        for_less( j, 0, n )
            inverse[i][j] = 0.0;
        inverse[i][i] = 1.0;
    }

    success = scaled_maximal_pivoting_gaussian_elimination_float( n, matrix,
                                                                  n, inverse );

    if( success )
    {
        for_less( i, 0, n-1 )
        {
            for_less( j, i+1, n )
            {
                tmp = inverse[i][j];
                inverse[i][j] = inverse[j][i];
                inverse[j][i] = tmp;
            }
        }
    }

    return( success );
}
