/* ----------------------------- MNI Header -----------------------------------
@NAME       : matrix_basics.c
@DESCRIPTION: File containing routines for doing basic matrix calculations
@METHOD     : Contains routines :
                 transpose
                 matrix_multiply
@CALLS      : 
@CREATED    : January 31, 1992 (Peter Neelin)
@MODIFIED   : Jul 6,      1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

#include  <internal_volume_io.h>
#include  <trans.h>



/* ----------------------------- MNI Header -----------------------------------
@NAME       : transpose
@INPUT      : rows    - number of rows
              cols    - number of columns
              mat     - original matrix
@OUTPUT     : mat_transpose  - transposed matrix
                     Matrix mat_transpose can be the original matrix mat.
@RETURNS    : (nothing)
@DESCRIPTION: Transposes a matrix.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  transpose(
    int     rows,
    int     cols,
    Real    **mat,
    Real    **mat_transpose )
{
    Real   swap;
    int    i, j;

    for_less( i, 0, rows )
    {
        mat_transpose[i][i] = mat[i][i];
        for_less( j, i+1, cols )
        {
            /*--- safe transpose, in case mat_transpose == mat */

            swap = mat[i][j];
            mat_transpose[i][j] = mat[j][i];
            mat_transpose[j][i] = swap;
        }
    }
}


/* ----------------------------- mni Header -----------------------------------
@NAME       : raw_matrix_multiply
@INPUT      : ldim, mdim, ndim - dimensions of matrices. Matrix Amat has
                 dimensions (ldim x mdim), matrix Bmat has dimension
                 (mdim x ndim) and resultant matrix has dimension
                 (ldim x ndim).
              Amat - First matrix of multiply
              Bmat - Second matrix of multiply
@OUTPUT     : Cmat - Resulting matrix.  This matrix cannot
                 be either Amat or Bmat.
@RETURNS    : (nothing)
@DESCRIPTION: Multiplies two matrices.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : Feb. 26, 1990 (Weiqian Dai)
@MODIFIED   : January 31, 1992 (Peter Neelin)
                 - change to roughly NIL-abiding code and modified calling
                 sequence.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

private  void  raw_matrix_multiply(
    int     ldim,
    int     mdim,
    int     ndim, 
    Real    **Amat,
    Real    **Bmat,
    Real    **Cmat )
{
    Real   sum;
    int    i, j, k;

    /* Calculate the product */

    for_less( i, 0, ldim )
    {
        for_less( j, 0, ndim )
        {
            sum = 0.0;
            for_less( k, 0, mdim )
                sum += Amat[i][k] * Bmat[k][j];

            Cmat[i][j] = sum;
        }
    }
}


/* ----------------------------- MNI Header -----------------------------------
@NAME       : matrix_multiply
@INPUT      : ldim, mdim, ndim - dimensions of matrices. Matrix Amat has
                 dimensions (ldim x mdim), matrix Bmat has dimension
                 (mdim x ndim) and resultant matrix has dimension
                 (ldim x ndim).
              Amat - First matrix of multiply
              Bmat - Second matrix of multiply
@OUTPUT     : Cmat - Resulting matrix.  This matrix is allowed to be
                                        matrix Amat or Bmat.
@RETURNS    : (nothing)
@DESCRIPTION: Multiplies two matrices.
@METHOD     : 
@GLOBALS    : (none)
@CALLS      : (nothing special)
@CREATED    : March 2, 1992 (Peter Neelin)
@MODIFIED   : March 2, 1992 (P.N.)
                 - Changed so that calling program can use an input matrix for
                 output.
@MODIFIED   : July    4, 1995 D. MacDonald - removed recipes-style code
---------------------------------------------------------------------------- */

public  void  matrix_multiply(
    int    ldim,
    int    mdim,
    int    ndim, 
    Real   **Amat,
    Real   **Bmat,
    Real   **Cmat )
{
    int     i, j;
    Real    **Ctemp;

    /* Allocate a temporary matrix */

    ALLOC2D( Ctemp, ldim, ndim );

    /* Do the multiplication */

    raw_matrix_multiply( ldim, mdim, ndim, Amat, Bmat, Ctemp );

    /* Copy the result */

    for_less( i, 0, ldim )
        for_less( j, 0, ndim )
            Cmat[i][j] = Ctemp[i][j];

    /* Free the temporary matrix */

    FREE2D( Ctemp );
}
